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
#include <zephyr/sys/printk.h>
/* STEP 4 - Include the header file of the logger module */
#include <zephyr/logging/log.h>

/* STEP 5 - Register your code with the logger */
LOG_MODULE_REGISTER(Jenjoun,LOG_LEVEL_DBG);

/*Include BMP API after adding to CMakeLists*/
//#include "bmp5.h"
//#include "bmp5_defs.h"

#define I2C0_NODE DT_NODELABEL(bmp581)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NODE);

/*! @name I2C addresses */
#define BMP5_I2C_ADDR_PRIM UINT8_C(0x46)
#define BMP5_I2C_ADDR_SEC UINT8_C(0x47)

#define BMP5_CHIP_ID_PRIM UINT8_C(0x50)
#define BMP5_REG_CHIP_ID UINT8_C(0x01)

#define BMP5_REG_TEMP_DATA_START UINT8_C(0x1D)
#define BMP5_REG_PRESS_DATA_START UINT8_C(0x20)

/* read/write buffers*/
uint8_t read_buf_id[1];
uint8_t read_buf_temp[3]; /*3 bytes of data for temp*/
uint8_t read_buf_pres[3]; /*3 bytes of data for pres*/

uint8_t write_reg_buffer[1]; /*1 byte commands*/

uint8_t read_meas_buffer[6]; /*6 bytes of incoming sensor data*/

void main(void)
{
  int err;

	if (!device_is_ready(dev_i2c.bus))
	{
		LOG_INF("I2C bus %s is not ready!\n\r", dev_i2c.bus->name);
		return;
	}

	LOG_INF("I2C bus %s is ready!\n\r", dev_i2c.bus->name);

	write_reg_buffer[0] = BMP5_REG_CHIP_ID; /*address of chip ID*/
	
	err = i2c_write_read_dt(&dev_i2c, write_reg_buffer, 1, read_buf_id, 1);
	
	if (err < 0)
	{
		LOG_INF("BME CHIP ID READ FAILED: %d\n", err);
		return;
	}

	while (1) {
      LOG_INF("nRF Connect SDK Fundamentals");
      k_msleep(10*60*1000); 
	}
}