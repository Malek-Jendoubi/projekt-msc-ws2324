/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*INCLUDES*/
#include "main.h"
#include "timestamp.h"

#include <zephyr/bluetooth/bluetooth.h> /* Bluetooth Stack Include*/
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h> /* For managing Bluetooth LE Connections */

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

struct bmp5_sensor_data sensor_data;

static void on_connected(struct bt_conn *conn, uint8_t err);
static void on_disconnected(struct bt_conn *conn, uint8_t reason);

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

static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
                  BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)),
};

/* Implement the callback functions */
void on_connected(struct bt_conn *conn, uint8_t err)
{
    if (err)
    {
        printk("Connection error %d\n\r", err);
        return;
    }
    printk("Connected\n\r");

	/* Declare a structure to store the connection parameters */
    struct bt_conn_info info;
    err = bt_conn_get_info(conn, &info);
    if (err)
    {
        printk("bt_conn_get_info() returned %d", err);
        return;
    }

    /* Add the connection parameters to your log */
    double connection_interval = info.le.interval * 1.25; // in ms
    uint16_t supervision_timeout = info.le.timeout * 10;  // in ms
    printk("Connection parameters: interval %.2f ms, latency %d intervals, timeout %d ms",
            connection_interval, info.le.latency, supervision_timeout);

    my_conn = bt_conn_ref(conn);
    /* TODO: Turn the connection status LED on */
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("Disconnected. Reason %d\n\r", reason);
    bt_conn_unref(my_conn);

    /* TODO: Turn the connection status LED off */
}

/* Declare the connection_callback structure */
struct bt_conn_cb connection_callbacks = {
    .connected = on_connected,
    .disconnected = on_disconnected,
};

int main(void)
{
    /* Declare packets for payload*/
    char packet_ts[40] = "";
    char packet_sensor[20] = "";
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

    while (1)
    {
        rslt = get_sensor_data(&osr_odr_press_cfg, &dev);
        bmp5_error_codes_print_result("get_sensor_data", rslt);

        // Build the frame. eg:"101068,23"
        sprintf(packet_sensor, "%lu,%ld", (long unsigned int)sensor_data.pressure, (long int)sensor_data.temperature);

        // Get Timestamp
        timestamp = OS_GET_TIME();
        // Build the frame. eg:"1483228799,101068,23"
        sprintf(packet_ts, "%u,%s\n\r", (uint32_t)timestamp, packet_sensor);

#if 0
        printk("packet_sensor,length[%d]:   %s\n\r", strlen(packet_sensor), packet_sensor);
        printk("packet_ts,length[%d]:       %s\n\r", strlen(packet_ts), packet_ts);
        
        adv_mfg_data.pressure_data = (uint16_t)(sensor_data).pressure;
        bt_le_adv_update_data(ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
#endif

        bmp5_delay_us(1000 * 500, &dev);
    }
    return rslt;
}

static int8_t get_sensor_data(const struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev)
{
    int8_t rslt = 0;
    uint8_t int_status = 0x1;
    /*
        long unsigned int sensor_pressure[50];
        long int sensor_temp[50];

     */
#if 0
    printk("\nOutput:\n");
    printk("Pressure (Pa), \tTemperature (deg C)\n");
#endif
    if (int_status & BMP5_INT_ASSERTED_DRDY)
    {
        rslt = bmp5_get_sensor_data(&sensor_data, osr_odr_press_cfg, dev);
#if 0
        if (rslt == BMP5_OK)
        {
            printk("%lu, %ld\n\r", (long unsigned int)sensor_data.pressure, (long int)sensor_data.temperature);
        }
#endif
        bmp5_delay_us(10 * 1000, dev);
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
        if (rslt == BMP5_E_NULL_PTR)
        {
            printk("Error [%d] : Null pointer\r\n", rslt);
        }
        else if (rslt == BMP5_E_COM_FAIL)
        {
            printk("Error [%d] : Communication failure\r\n", rslt);
        }
        else if (rslt == BMP5_E_DEV_NOT_FOUND)
        {
            printk("Error [%d] : Device not found\r\n", rslt);
        }
        else if (rslt == BMP5_E_INVALID_CHIP_ID)
        {
            printk("Error [%d] : Invalid chip id\r\n", rslt);
        }
        else if (rslt == BMP5_E_POWER_UP)
        {
            printk("Error [%d] : Power up error\r\n", rslt);
        }
        else if (rslt == BMP5_E_POR_SOFTRESET)
        {
            printk("Error [%d] : Power-on reset/softreset failure\r\n", rslt);
        }
        else if (rslt == BMP5_E_INVALID_POWERMODE)
        {
            printk("Error [%d] : Invalid powermode\r\n", rslt);
        }
        else
        {
            /* For more error codes refer "*_defs.h" */
            printk("Error [%d] : Unknown error code\r\n", rslt);
        }
    }
}