#include "bt-periph.h"
#include "main.h"

#define LED_BLUE_NODE DT_ALIAS(led1)  // LED_RED_NODE = led0 defined in the .dts file

/*Prototype for BLE connection callbacks*/
static void on_connected(struct bt_conn *conn, uint8_t err);
static void on_disconnected(struct bt_conn *conn, uint8_t reason);
void on_le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);
void on_le_phy_updated(struct bt_conn *conn, struct bt_conn_le_phy_info *param);

/* LED config*/
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);

static bool notify_mysensor_enabled;

#define DEVICE_NAME "BMP581"
#define DEVICE_NAME_LEN 6

/* Variable that holds callback for MTU negotiation */
static struct bt_gatt_exchange_params exchange_params;

/* Forward declaration of exchange_func(): */
static void exchange_func(struct bt_conn *conn, uint8_t att_err,
                          struct bt_gatt_exchange_params *params);

/*BLE Connection struct*/
struct bt_conn *my_conn = NULL;

/* ## GATT Server ## */
/* Define the configuration change callback function for the MYSENSOR characteristic */
static void mylbs_ccc_mysensor_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    notify_mysensor_enabled = (value == BT_GATT_CCC_NOTIFY);
}


/* GATT characteristic and service Declaration */
/* Notification handler*/
static ssize_t read_char(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    const char *value = attr->user_data;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));
}


/* GATT Service Declaration */
BT_GATT_SERVICE_DEFINE(
    my_lbs_svc, 
    BT_GATT_PRIMARY_SERVICE(BT_UUID_MY_SERVICE),
    /* Create and add the MYSENSOR characteristic and its CCCD  */
    /* Characteristic Declaration */
    BT_GATT_CHARACTERISTIC( BT_UUID_MY_SERVICE_SENSOR,
                            BT_GATT_CHRC_NOTIFY,
                            BT_GATT_PERM_READ,
                            read_char,
                            NULL, frame_payload),
    BT_GATT_CCC(mylbs_ccc_mysensor_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
 );

/* Create an LE Advertising Parameters variable */
static struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
    (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_IDENTITY), /* Connectable advertising and use identity address */
    BT_GAP_ADV_FAST_INT_MIN_1,                                /* 0x30 units, 48 units, 30ms */
    BT_GAP_ADV_FAST_INT_MAX_1,                                /* 0x60 units, 96 units, 60ms */
    NULL);                                                    /* Set to NULL for undirected advertising */

/* Declare the advertising packet */
static volatile uint8_t mfg_data[] = {0x00, 0x00, 0xaa, 0xbb};

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xaa, 0xfe),
    BT_DATA_BYTES(BT_DATA_SVC_DATA16,
                  0xaa, 0xfe,                       /* Eddystone UUID*/
                  'B', 'M', 'P', '5', '8', '1')     /* BMP581 */
};

/* Declare the scan response packet - Set Scan Response data */
static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

/* Function to update the connection's PHY */
static void update_phy(struct bt_conn *conn)
{
    int err;
    const struct bt_conn_le_phy_param preferred_phy = {
        .options = BT_CONN_LE_PHY_OPT_NONE,
        .pref_rx_phy = BT_GAP_LE_PHY_2M,
        .pref_tx_phy = BT_GAP_LE_PHY_2M,
    };
    err = bt_conn_le_phy_update(conn, &preferred_phy);
    if (err)
    {
        printk("bt_conn_le_phy_update() returned %d\r\n", err);
    }
}

/* Function to update the connection's data length */
static void update_data_length(struct bt_conn *conn)
{
    int err;
    struct bt_conn_le_data_len_param my_data_len = {
        .tx_max_len = BT_GAP_DATA_LEN_MAX,
        .tx_max_time = BT_GAP_DATA_TIME_MAX,
    };
    err = bt_conn_le_data_len_update(my_conn, &my_data_len);
    if (err)
    {
        printk("data_len_update failed (err %d)\r\n", err);
    }
}

/* Function to update the connection's MTU */
static void update_mtu(struct bt_conn *conn)
{
    int err;
    exchange_params.func = exchange_func;

    err = bt_gatt_exchange_mtu(conn, &exchange_params);
    if (err)
    {
        printk("bt_gatt_exchange_mtu failed (err %d)\r\n", err);
    }
}

/* Implement the on connected callback functions */
void on_connected(struct bt_conn *conn, uint8_t err)
{
    if (err)
    {
        printk("Connection error %d\n\r", err);
        return;
    }
    printk("Connected\n\r");

    /* Increase the connection counter*/
    my_conn = bt_conn_ref(conn);

    /* A structure to store the connection parameters */
    struct bt_conn_info info;
    err = bt_conn_get_info(conn, &info);
    if (err)
    {
        printk("bt_conn_get_info() returned %d\n\r", err);
        return;
    }

    /* Add the connection parameters to your log */
    double connection_interval = info.le.interval * 1.25; // in ms
    uint16_t supervision_timeout = info.le.timeout * 10;  // in ms
    printk("Connection parameters: interval %.2f ms, latency %d intervals, timeout %d ms\n\r",
           connection_interval, info.le.latency, supervision_timeout);

    /* Update the PHY mode */
    update_phy(my_conn);

    /* Update the data length and MTU */
    update_data_length(my_conn);
    update_mtu(my_conn);

    /* Turn the connection status LED on */
    gpio_pin_set_dt(&led_blue, 1);
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("Disconnected. Reason %d\n\r", reason);
    bt_conn_unref(my_conn);

    /* Turn the connection status LED off */
    gpio_pin_set_dt(&led_blue, 0);
}

/* Callback for connection parameter log */
void on_le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
    double connection_interval = interval * 1.25; // in ms
    uint16_t supervision_timeout = timeout * 10;  // in ms

    printk("Connection parameters updated: interval %.2f ms, latency %d intervals, timeout %d ms\n\r", connection_interval, latency, supervision_timeout);
}

/* Callback function to log updates in the PHY */
void on_le_phy_updated(struct bt_conn *conn, struct bt_conn_le_phy_info *param)
{
    // PHY Updated
    if (param->tx_phy == BT_CONN_LE_TX_POWER_PHY_1M)
    {
        printk("PHY updated. New PHY: 1M\n\r");
    }
    else if (param->tx_phy == BT_CONN_LE_TX_POWER_PHY_2M)
    {
        printk("PHY updated. New PHY: 2M\n\r");
    }
    else if (param->tx_phy == BT_CONN_LE_TX_POWER_PHY_CODED_S8)
    {
        printk("PHY updated. New PHY: Long Range\n\r");
    }
}

/* Callback function to log updates in data length */
void on_le_data_len_updated(struct bt_conn *conn, struct bt_conn_le_data_len_info *info)
{
    uint16_t tx_len = info->tx_max_len;
    uint16_t tx_time = info->tx_max_time;
    uint16_t rx_len = info->rx_max_len;
    uint16_t rx_time = info->rx_max_time;
    printk("Data length updated. Length %d/%d bytes, time %d/%d us\r\n", tx_len, rx_len, tx_time, rx_time);
}


/* Implement callback function for MTU exchange */
static void exchange_func(struct bt_conn *conn, uint8_t att_err,
                          struct bt_gatt_exchange_params *params)
{
    printk("MTU exchange %s\r\n", att_err == 0 ? "successful" : "failed");
    if (!att_err)
    {
        uint16_t payload_mtu =
            bt_gatt_get_mtu(conn) - 3; // 3 bytes used for Attribute headers.
        printk("New MTU: %d bytes\r\n", payload_mtu);
    }
}

/* Declare the connection_callback structure */
struct bt_conn_cb connection_callbacks = {
    .connected = on_connected,
    .disconnected = on_disconnected,
    .le_param_updated = on_le_param_updated,
    .le_phy_updated = on_le_phy_updated,
    .le_data_len_updated = on_le_data_len_updated,
};

void bluetooth_advertiser_init()
{ 
    /* Configure LED */
    int ret_led;
    if (!gpio_is_ready_dt(&led_blue)){
        return;
    }
    ret_led = gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_ACTIVE);
    if (ret_led < 0){
        return;
    }
    gpio_pin_set_dt(&led_blue, 0);
   
    /* Enable the Bluetooth LE stack */
    int bt_err;
    bt_conn_cb_register(&connection_callbacks);

    bt_err = bt_enable(NULL);
    if (bt_err)
    {
        printk("Bluetooth init failed (err %d)\n", bt_err);
        return;
    }
    printk("Bluetooth initialized\n");

    /* Start advertising */
    bt_err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (bt_err)
    {
        printk("Advertising failed to start (err %d)\n", bt_err);
        return;
    }
}

void sensor_notify(char* buf)
{	
	bt_gatt_notify(NULL, &my_lbs_svc.attrs[2], buf, sizeof(uint8_t)*SIZE_PAYLOAD);
}
