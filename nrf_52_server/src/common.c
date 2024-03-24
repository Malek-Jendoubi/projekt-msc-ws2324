/**
 * Copyright (C) 2022 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#include "bmp5_defs.h"

/******************************************************************************/
/*!                         Macro definitions                                 */
#define BMP581 DT_NODELABEL(bmp581)

/******************************************************************************/
/*!                Static variable definition                                 */

/*! Variable that holds the I2C device address or SPI chip selection */
static const struct i2c_dt_spec i2c_dev = I2C_DT_SPEC_GET(BMP581);
static uint8_t dev_addr;

/******************************************************************************/
/*!                User interface functions                                   */

/*!
 * I2C read function map to COINES platform
 */
BMP5_INTF_RET_TYPE bmp5_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    int rc = 0;
        rc = i2c_burst_read_dt(&i2c_dev, reg_addr, reg_data, length);
        if (rc != 0)
        {
            printk("i2c_burst_read_dt error\r\n"); // attempt to recover bus if tx failed for any reason
        }
        return rc;
}

/*!
 * I2C write function map to COINES platform
 */
BMP5_INTF_RET_TYPE bmp5_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    int rc = 0;
    
rc = i2c_burst_write_dt(&i2c_dev, reg_addr, reg_data, length);
        if (rc != 0)
        {
            printk("i2c_burst_write_dt error\r\n"); // attempt to recover bus if tx failed for any reason
        }
        return rc;
}


/*!
 * Delay function map to COINES platform
 */
void bmp5_delay_us(uint32_t period, void *intf_ptr)
{
    k_msleep((uint32_t)period / 1000);
}

/*!
 *  @brief Function to select the interface between SPI and I2C.
 */
int8_t bmp5_interface_init(struct bmp5_dev *bmp5_dev, uint8_t intf)
{
    int8_t rslt = BMP5_OK;
    //    int16_t result;

    if (bmp5_dev != NULL)
    {
        /* Bus configuration : I2C */
        if (intf == BMP5_I2C_INTF)
        {
            printk("I2C Interface\n");

            dev_addr = BMP5_I2C_ADDR_PRIM;
            bmp5_dev->read = bmp5_i2c_read;
            bmp5_dev->write = bmp5_i2c_write;
            bmp5_dev->intf = BMP5_I2C_INTF;
        }

        if (!device_is_ready(i2c_dev.bus))
        {
            printk("Board I2C: Device is not ready.\n");
            return -1;
        }

        k_msleep(100);
        /* Holds the I2C device addr or SPI chip selection */
        bmp5_dev->intf_ptr = &dev_addr;

        /* Configure delay in microseconds */
        bmp5_dev->delay_us = bmp5_delay_us;
    }
    else
    {
        rslt = BMP5_E_NULL_PTR;
    }
    return rslt;
}

int8_t set_config(struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev)
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
