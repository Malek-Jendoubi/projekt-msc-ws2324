/**
 * Copyright (C) 2022 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#include "bmp5_defs.h"

/******************************************************************************/
/*!                         Macro definitions                                 */

/*! BMP5 shuttle id */
#define BMP5_SHUTTLE_ID_PRIM  UINT16_C(0x1B3)
#define BMP5_SHUTTLE_ID_SEC   UINT16_C(0x1D3)

/******************************************************************************/
/*!                Static variable definition                                 */

/*! Variable that holds the I2C device address or SPI chip selection */
static uint8_t dev_addr;

/******************************************************************************/
/*!                User interface functions                                   */

/*!
 * I2C read function map to COINES platform
 */
BMP5_INTF_RET_TYPE bmp5_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    uint8_t device_addr = *(uint8_t*)intf_ptr;
    int result = 0;

    (void)intf_ptr; 

    const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(bmp581));

    if (i2c_dev == NULL)
    {
        LOG_ERR("Error reading I2C device");
    }else{
        if (!device_is_ready(i2c_dev))
        {
            LOG_ERR("I2C device not ready");
            return -1;
        }
        else
        {
            result = i2c_burst_read(i2c_dev, device_addr, reg_addr, reg_data, length);
            
            if (result!=0)/*if read unsuccessful recover i2c bus*/
            {
                i2c_recover_bus(i2c_dev);
            }
            return result;
        }
    }
}

/*!
 * I2C write function map to COINES platform
 */
BMP5_INTF_RET_TYPE bmp5_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    uint8_t device_addr = *(uint8_t*)intf_ptr;
    int result = 0;

    (void)intf_ptr; 

    const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(bmp581));

    if (i2c_dev == NULL)
    {
        LOG_ERR("Error reading I2C device");
    }else{
        if (!device_is_ready(i2c_dev))
        {
            LOG_ERR("I2C device not ready");
            return -1;
        }
        else
        {
            result = i2c_burst_write(i2c_dev, device_addr, reg_addr, reg_data, length);
            
            if (result!=0)/*if read unsuccessful recover i2c bus*/
            {
                i2c_recover_bus(i2c_dev);
            }
            return result;
        }
    }
}

/*!
 * Delay function map to COINES platform
 */
void bmp5_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    k_msleep(period);
}

/*!
 *  @brief Prints the execution status of the APIs.
 */
void bmp5_error_codes_print_result(const char api_name[], int8_t rslt)
{
    if (rslt != BMP5_OK)
    {
        printf("%s\t", api_name);
        if (rslt == BMP5_E_NULL_PTR)
        {
            printf("Error [%d] : Null pointer\r\n", rslt);
        }
        else if (rslt == BMP5_E_COM_FAIL)
        {
            printf("Error [%d] : Communication failure\r\n", rslt);
        }
        else if (rslt == BMP5_E_DEV_NOT_FOUND)
        {
            printf("Error [%d] : Device not found\r\n", rslt);
        }
        else if (rslt == BMP5_E_INVALID_CHIP_ID)
        {
            printf("Error [%d] : Invalid chip id\r\n", rslt);
        }
        else if (rslt == BMP5_E_POWER_UP)
        {
            printf("Error [%d] : Power up error\r\n", rslt);
        }
        else if (rslt == BMP5_E_POR_SOFTRESET)
        {
            printf("Error [%d] : Power-on reset/softreset failure\r\n", rslt);
        }
        else if (rslt == BMP5_E_INVALID_POWERMODE)
        {
            printf("Error [%d] : Invalid powermode\r\n", rslt);
        }
        else
        {
            /* For more error codes refer "*_defs.h" */
            printf("Error [%d] : Unknown error code\r\n", rslt);
        }
    }
}

/*!
 *  @brief Function to select the interface between SPI and I2C.
 */
int8_t bmp5_interface_init(struct bmp5_dev *bmp5_dev, uint8_t intf)
{
    int8_t rslt = BMP5_OK;
    int16_t result;

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

            /* SDO pin is made low */
        }


        uint32_t dev_config = I2C_MODE_CONTROLLER | I2C_SPEED_SET(I2C_SPEED_STANDARD); 
        struct device *dev = NULL;

        i2c_configure(DEVICE_DT_GET(DT_NODELABEL(bmp581)), dev_config);

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