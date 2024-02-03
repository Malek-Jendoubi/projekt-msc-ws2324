/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*INCLUDES*/
#include "main.h"

/* Bluetooth Stack Includes*/
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>

void bluetooth_advertiser_init();

struct bmp5_sensor_data sensor_data;

/*Define Nordic Semiconductor Company Identifier*/
#define COMPANY_ID_CODE 0x0059

/* Declare the structure for your custom data  */
typedef struct adv_mfg_data
{
    uint8_t company_code;   /* Company Identifier Code. */
    uint16_t pressure_data; /* Number of times Button 1 is pressed*/
} adv_mfg_data_type;

/* Create an LE Advertising Parameters variable */
static struct bt_le_adv_param *adv_param =
    BT_LE_ADV_PARAM(BT_LE_ADV_OPT_NONE,
                    1600, /*Minimum advertising interval N = 1000ms/0,625ms = 1600*/
                    1601,
                    NULL);

/* Define and initialize a variable of type adv_mfg_data_type */
static adv_mfg_data_type adv_mfg_data = {COMPANY_ID_CODE, 101025};

static unsigned char url_data[] = {0x17, '/', '/', 'a', 'c', 'a', 'd', 'e', 'm',
                                   'y', '.', 'n', 'o', 'r', 'd', 'i', 'c', 's',
                                   'e', 'm', 'i', '.', 'c', 'o', 'm'};

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/* Declare the advertising packet */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
    /* STEP 3 - Include the Manufacturer Specific Data in the advertising packet. */
    BT_DATA(BT_DATA_MANUFACTURER_DATA, (unsigned char *)&adv_mfg_data, sizeof(adv_mfg_data)),
};

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_URI, url_data, sizeof(url_data)),
};

int main(void)
{
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

        adv_mfg_data.pressure_data = (uint16_t)(sensor_data).pressure;
        // adv_mfg_data.pressure_data++;
        bt_le_adv_update_data(ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));

        bmp5_delay_us(1000 * 500, &dev);
    }
    return rslt;
}

void bluetooth_advertiser_init()
{ /* Enable the Bluetooth LE stack */
    int bt_err;

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

static int8_t get_sensor_data(const struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev)
{
    int8_t rslt = 0;
    uint8_t int_status = 0x1;
    /*
        long unsigned int sensor_pressure[50];
        long int sensor_temp[50];

     */
    printk("Output :\n");
    printk("Pressure (Pa), \tTemperature (deg C)\n");
        if (int_status & BMP5_INT_ASSERTED_DRDY)
        {
            rslt = bmp5_get_sensor_data(&sensor_data, osr_odr_press_cfg, dev);

            if (rslt == BMP5_OK)
            {
                printk("%lu, \t%ld\n", (long unsigned int)sensor_data.pressure, (long int)sensor_data.temperature);
            }

            bmp5_delay_us(10 * 1000, dev);
        }
    return rslt;
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