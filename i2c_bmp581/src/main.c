/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Bluetooth Stack Include*/
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/addr.h>
/* For managing Bluetooth LE Connections */
#include <zephyr/bluetooth/conn.h>

/*INCLUDES*/
#include "main.h"
#include "timestamp.h"

/*Set to 1 to print the values from the BMP sensor*/
#define PRINT_SENSOR_VALUES 0

struct bmp5_sensor_data sensor_data;

/*Declare a ring buffer to hold the sensor data before sending them*/
#define SENSOR_READ 0xAC
#define MY_RING_BUF_WORDS 1
RING_BUF_ITEM_DECLARE(sensor_ring_buf, MY_RING_BUF_WORDS);



/*Prototype for BLE connection callbacks*/
static void on_connected(struct bt_conn *conn, uint8_t err);
static void on_disconnected(struct bt_conn *conn, uint8_t reason);
void on_le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);
void on_le_phy_updated(struct bt_conn *conn, struct bt_conn_le_phy_info *param);

/* Ring buffer put helper function*/
void rb_put(struct ring_buf sensor_ring_buf, uint8_t *my_data);


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
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
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

int main(void)
{
    /* Declare buffers for payload*/
    /* packet_ts[20] = "1483228799,101068,23" */
    char frame_ts[20] = "";
    /* packet_sensor[9] = "101068,23" */
    char frame_sensor[9] = "";
    /*Variables for the frame -- Timestamp*/
    uint64_t timestamp = 0;

    int8_t rslt;

    struct bmp5_dev dev;
    struct bmp5_osr_odr_press_config osr_odr_press_cfg = {0};

    bluetooth_advertiser_init();

    int8_t bmp5_rslt;
    /* Interface reference is given as a parameter
     * For I2C : BMP5_I2C_INTF
     * For SPI : BMP5_SPI_INTF
     */
    bmp5_rslt = bmp5_interface_init(&dev, BMP5_I2C_INTF);
    bmp5_error_codes_print_result("bmp5_interface_init", bmp5_rslt);

    if (bmp5_rslt == BMP5_OK)
    {

        bmp5_soft_reset(&dev);

        bmp5_rslt = bmp5_init(&dev);
        bmp5_error_codes_print_result("bmp5_init", bmp5_rslt);

        if (bmp5_rslt == BMP5_OK)
        {
            bmp5_rslt = set_config(&osr_odr_press_cfg, &dev);
            bmp5_error_codes_print_result("set_config", bmp5_rslt);
        }
    }

    int rb_counter = 0;

    while (1)
    {
        rb_counter ++;
        /* Get sensor data from the BMP581*/
        rslt = get_sensor_data(&osr_odr_press_cfg, &dev);
        bmp5_error_codes_print_result("get_sensor_data", rslt);

        /* Build the frame. eg:"101068,23"*/
        sprintf(frame_sensor, "%lu,%ld", (long unsigned int)sensor_data.pressure, (long int)sensor_data.temperature);

        /* Get Timestamp and add it to the frame. eg:"1483228799,101068,23"*/
        timestamp = OS_GET_TIME();
        sprintf(frame_ts, "%u,%s\n\r", (uint32_t)timestamp, frame_sensor);

        /* Print the two frames*/
        printk("frame_sensor[%d]:\t%s\n\r", strlen(frame_sensor), frame_sensor);
        printk("frame_ts[%d]:\t%s\n\r", strlen(frame_ts), frame_ts);

        rb_put(sensor_ring_buf, (uint8_t)frame_ts);

        k_msleep(1000);
    }
    return rslt;
}

static int8_t get_sensor_data(const struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev)
{
    int8_t rslt = 0;
    uint8_t int_status = 0x1;

    if (int_status & BMP5_INT_ASSERTED_DRDY)
    {
        rslt = bmp5_get_sensor_data(&sensor_data, osr_odr_press_cfg, dev);
    }
    return rslt;
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

static int8_t set_config(struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev)
{
    int8_t rslt;
    struct bmp5_iir_config set_iir_cfg;
    struct bmp5_int_source_select int_source_select;

    rslt = bmp5_set_power_mode(BMP5_POWERMODE_STANDBY, dev);
    bmp5_error_codes_print_result("bmp5_set_power_mode1", rslt);

    if (rslt == BMP5_OK)
    {
        /* Get default odr */
        rslt = bmp5_get_osr_odr_press_config(osr_odr_press_cfg, dev);
        bmp5_error_codes_print_result("bmp5_get_osr_odr_press_config", rslt);

        if (rslt == BMP5_OK)
        {
            /* Enable pressure */
            osr_odr_press_cfg->press_en = BMP5_ENABLE;

            rslt = bmp5_set_osr_odr_press_config(osr_odr_press_cfg, dev);
            bmp5_error_codes_print_result("bmp5_set_osr_odr_press_config", rslt);
        }

        if (rslt == BMP5_OK)
        {
            set_iir_cfg.set_iir_t = BMP5_IIR_FILTER_COEFF_1;
            set_iir_cfg.set_iir_p = BMP5_IIR_FILTER_COEFF_1;
            set_iir_cfg.shdw_set_iir_t = BMP5_ENABLE;
            set_iir_cfg.shdw_set_iir_p = BMP5_ENABLE;

            rslt = bmp5_set_iir_config(&set_iir_cfg, dev);
            bmp5_error_codes_print_result("bmp5_set_iir_config", rslt);
        }

        if (rslt == BMP5_OK)
        {
            rslt = bmp5_configure_interrupt(BMP5_PULSED, BMP5_ACTIVE_HIGH, BMP5_INTR_PUSH_PULL, BMP5_INTR_ENABLE, dev);

            bmp5_error_codes_print_result("bmp5_configure_interrupt", rslt);

            if (rslt == BMP5_OK)
            {
                /* Note : Select INT_SOURCE after configuring interrupt */
                int_source_select.drdy_en = BMP5_ENABLE;
                rslt = bmp5_int_source_select(&int_source_select, dev);
                bmp5_error_codes_print_result("bmp5_int_source_select", rslt);
            }
        }

        /* Set powermode as continous */
        rslt = bmp5_set_power_mode(BMP5_POWERMODE_CONTINOUS, dev);
        bmp5_error_codes_print_result("bmp5_set_power_mode", rslt);
    }

    return rslt;
}

/*!
 *  @brief Prints the execution status of the APIs.
 */
void bmp5_error_codes_print_result(const char api_name[], int8_t rslt)
{
    if (rslt != BMP5_OK)
    {
        printk("%s\r\n", api_name);
        switch (rslt)
        {
        case BMP5_E_NULL_PTR:
            printk("Error [%d] : Null pointer\r\n", rslt);
            break;
        case BMP5_E_COM_FAIL:
            printk("Error [%d] : Communication failure\r\n", rslt);
            break;
        case BMP5_E_DEV_NOT_FOUND:
            printk("Error [%d] : Device not found\r\n", rslt);
            break;
        case BMP5_E_INVALID_CHIP_ID:
            printk("Error [%d] : Invalid chip id\r\n", rslt);
            break;
        case BMP5_E_POWER_UP:
            printk("Error [%d] : Power up error\r\n", rslt);
            break;
        case BMP5_E_POR_SOFTRESET:
            printk("Error [%d] : Power-on reset/softreset failure\r\n", rslt);
            break;
        case BMP5_E_INVALID_POWERMODE:
            printk("Error [%d] : Invalid powermode\r\n", rslt);
            break;
        default:
            printk("Error [%d] : Unknown error code\r\n", rslt);
            break;
        }
    }
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

void rb_put(struct ring_buf sensor_ring_buf, uint8_t *my_data)
{
    uint32_t ret;

    ret = ring_buf_item_put(&sensor_ring_buf, SENSOR_READ, 0, my_data, MY_RING_BUF_WORDS);
    if (ret != MY_RING_BUF_WORDS)
    {
        printk("not enough room, partial copy.");
    }
    return ret;
}