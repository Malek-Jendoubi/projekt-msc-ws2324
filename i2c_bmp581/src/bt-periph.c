/* Bluetooth Stack Include*/
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/addr.h>
/* For managing Bluetooth LE Connections */
#include <zephyr/bluetooth/conn.h>

/*Prototype for BLE connection callbacks*/
static void on_connected(struct bt_conn *conn, uint8_t err);
static void on_disconnected(struct bt_conn *conn, uint8_t reason);
void on_le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);
void on_le_phy_updated(struct bt_conn *conn, struct bt_conn_le_phy_info *param);

/* Variable that holds callback for MTU negotiation */
static struct bt_gatt_exchange_params exchange_params;
/* Forward declaration of exchange_func(): */
static void exchange_func(struct bt_conn *conn, uint8_t att_err,
                          struct bt_gatt_exchange_params *params);

/*BLE Connection struct*/
struct bt_conn *my_conn = NULL;

/* Create an LE Advertising Parameters variable */
static struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
    (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_IDENTITY), /* Connectable advertising and use identity address */
    BT_GAP_ADV_FAST_INT_MIN_1,                                /* 0x30 units, 48 units, 30ms */
    BT_GAP_ADV_FAST_INT_MAX_1,                                /* 0x60 units, 96 units, 60ms */
    NULL);                                                    /* Set to NULL for undirected advertising */

/* Declare the advertising packet */
static volatile uint8_t mfg_data[] = {0x00, 0x00, 0xaa, 0xbb};

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, 4),

    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
                  0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
                  0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12),
};

/* Declare the scan response packet */
static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
                  BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde,
                                     0x1523, 0x785feabcd123)),
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

/* Implement the callback functions */
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

    /* Declare a structure to store the connection parameters */
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

    /* TODO: Turn the connection status LED on */
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("Disconnected. Reason %d\n\r", reason);
    bt_conn_unref(my_conn);

    /* TODO: Turn the connection status LED off */
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

/* Declare the connection_callback structure */
struct bt_conn_cb connection_callbacks = {
    .connected = on_connected,
    .disconnected = on_disconnected,
    .le_param_updated = on_le_param_updated,
    .le_phy_updated = on_le_phy_updated,
    .le_data_len_updated = on_le_data_len_updated,
};


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

void bluetooth_advertiser_init()
{ /* Enable the Bluetooth LE stack */
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
