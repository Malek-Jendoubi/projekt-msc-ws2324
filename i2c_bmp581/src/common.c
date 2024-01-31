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

/*! BMP5 shuttle id */
#define BMP5_SHUTTLE_ID_PRIM UINT16_C(0x46)
#define BMP5_SHUTTLE_ID_SEC UINT16_C(0x47)

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
            printk("Read"); // attempt to recover bus if tx failed for any reason
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
            printk("Read"); // attempt to recover bus if tx failed for any reason
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
            printf("I2C Interface\n");

            dev_addr = BMP5_I2C_ADDR_PRIM;
            bmp5_dev->read = bmp5_i2c_read;
            bmp5_dev->write = bmp5_i2c_write;
            bmp5_dev->intf = BMP5_I2C_INTF;
        }

        if (!device_is_ready(i2c_dev.bus))
        {
            printk("Board I2C: Device is not ready.");
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