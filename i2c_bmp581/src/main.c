/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#include "bmp5.h"
#include "common.h"

/*!
 *  @brief This internal API is used to get sensor data.
 *
 *  @param[in] osr_odr_press_cfg : Structure instance of bmp5_osr_odr_press_config
 *  @param[in] dev               : Structure instance of bmp5_dev.
 *
 *  @return Status of execution.
 */
static int8_t get_sensor_data(const struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev);

/* Register the logger */
LOG_MODULE_REGISTER(BMPI2C,LOG_LEVEL_DBG);

int main(void)
{
	int8_t rslt;
    struct bmp5_dev dev;
    struct bmp5_osr_odr_press_config osr_odr_press_cfg = { 0 };

    /* Interface reference is given as a parameter
     * For I2C : BMP5_I2C_INTF
     * For SPI : BMP5_SPI_INTF
     */
    rslt = bmp5_interface_init(&dev, BMP5_I2C_INTF);


    if (rslt == BMP5_OK)
    {
        rslt = bmp5_init(&dev);

        if (rslt == BMP5_OK)
        {
            rslt = get_sensor_data(&osr_odr_press_cfg, &dev);
        }
    }

	while (1) {
      LOG_INF("nRF Connect SDK Fundamentals");
      k_msleep(2*1000); 
	}

	return 0;
}

static int8_t get_sensor_data(const struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev)
{
    int8_t rslt = 0;
    uint8_t idx = 0;
    uint8_t int_status;
    struct bmp5_sensor_data sensor_data;

    LOG_INF("\nOutput :\n\n");
    LOG_INF("Data, Pressure (Pa), Temperature (deg C)\n");

    while (idx < 50)
    {
        if (int_status & BMP5_INT_ASSERTED_DRDY)
        {
            rslt = bmp5_get_sensor_data(&sensor_data, osr_odr_press_cfg, dev);

            if (rslt == BMP5_OK)
            {
#ifdef BMP5_USE_FIXED_POINT
                LOG_INF("%d, %lu, %ld\n", idx, (long unsigned int)sensor_data.pressure,
                       (long int)sensor_data.temperature);
#else
                LOG_INF("%d, %f, %f\n", idx, sensor_data.pressure, sensor_data.temperature);
#endif
                idx++;
            }
        }
    }

    return rslt;
}