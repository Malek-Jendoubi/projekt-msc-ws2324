/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#include "bmp5.h"
#include "common.h"

#define PERIOD_MS K_MSEC(10)

struct bmp5_sensor_data sensor_data[50];


/*!
 *  @brief This internal API is used to set configurations of the sensor.
 *
 *  @param[in,out] osr_odr_press_cfg : Structure instance of bmp5_osr_odr_press_config
 *  @param[in] dev                   : Structure instance of bmp5_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_config(struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev);

/*!
 *  @brief This internal API is used to get sensor data.
 *
 *  @param[in] osr_odr_press_cfg : Structure instance of bmp5_osr_odr_press_config
 *  @param[in] dev               : Structure instance of bmp5_dev.
 *
 *  @return Status of execution.
 */
static int8_t get_sensor_data(const struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev);

/*!
 *  @brief Prints the execution status of the APIs.
 *
 *  @param[in] api_name : Name of the API whose execution status has to be printed.
 *  @param[in] rslt     : Error code returned by the API whose execution status has to be printed.
 *
 *  @return void.
 */
void bmp5_error_codes_print_result(const char api_name[], int8_t rslt);

/* Register the logger */
//LOG_MODULE_REGISTER(BMPI2C, LOG_LEVEL_DBG);

int main(void)
{
    int8_t rslt;
    struct bmp5_dev dev;
    struct bmp5_osr_odr_press_config osr_odr_press_cfg = {0};

    /* Interface reference is given as a parameter
     * For I2C : BMP5_I2C_INTF
     * For SPI : BMP5_SPI_INTF
     */
    rslt = bmp5_interface_init(&dev, BMP5_I2C_INTF);
    bmp5_error_codes_print_result("bmp5_interface_init", rslt);

    if (rslt == BMP5_OK)
    {

        bmp5_soft_reset(&dev);

        rslt = bmp5_init(&dev);
        bmp5_error_codes_print_result("bmp5_init", rslt);

        if (rslt == BMP5_OK)
        {
            rslt = set_config(&osr_odr_press_cfg, &dev);
            bmp5_error_codes_print_result("set_config", rslt);
        }

    }
    while(1)
    {
        rslt = get_sensor_data(&osr_odr_press_cfg, &dev);
        bmp5_error_codes_print_result("get_sensor_data", rslt);

        bmp5_delay_us(1000*500,&dev);
    }
    return rslt;
}

static int8_t get_sensor_data(const struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev)
{
    int8_t rslt = 0;
    uint8_t idx = 0;
    uint8_t int_status = 0x1;
/* 
    long unsigned int sensor_pressure[50];
    long int sensor_temp[50];

 */
    printk("\nOutput :\n\n");
    printk("Data, \tPressure (Pa), \tTemperature (deg C)\n");

    while (idx < 50)
    {
        if (int_status & BMP5_INT_ASSERTED_DRDY)
        {
            rslt = bmp5_get_sensor_data(&sensor_data[idx], osr_odr_press_cfg, dev);

            if (rslt == BMP5_OK)
            {
#ifdef BMP5_USE_FIXED_POINT
                printk("%d, \t%lu, \t%ld\n", idx, (long unsigned int)sensor_data[idx].pressure, (long int)sensor_data[idx].temperature);
#endif
                idx++;
            }

            bmp5_delay_us(10*1000, dev);
        }
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
    printk("%s\r\n", api_name);
    if (rslt != BMP5_OK)
    {
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
    else
    {
        /* For more error codes refer "*_defs.h" */
        printk("Error[%d] :  BMP5 OK\r\n", rslt);
    }
}