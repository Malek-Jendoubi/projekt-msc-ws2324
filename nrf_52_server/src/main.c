/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*INCLUDES*/
#include "main.h"
#include "timestamp.h"
#include "bt-periph.h"
#include "bmp5.h"

#include <string.h>

#define NOTIFICATION_PERIOD_MS 1

struct bmp5_sensor_data sensor_data;
uint32_t pressure_data = 0;


/*Variables for the frame -- Timestamp*/
uint32_t timestamp_ms = 0;


int main(void)
{
    struct bmp5_dev dev;
    struct bmp5_osr_odr_press_config osr_odr_press_cfg = {0};

    bluetooth_advertiser_init();

    int8_t bmp5_rslt;

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
        /* Get sensor data from the BMP581*/
        bmp5_rslt = get_sensor_data(&osr_odr_press_cfg, &dev);
        bmp5_error_codes_print_result("get_sensor_data", bmp5_rslt);

        /* Print the two frames
        printk("frame_sensor[%d]:%s\n\r", strlen(frame_sensor), frame_sensor);
        printk("frame_ts[%d]:%s\n\r", strlen(frame_ts), frame_ts);
        sprintf(frame_payload,"%s,%s\n\r", frame_ts, frame_sensor);
        printk("%s", frame_payload);
        */

        /* TODO: Add temperature notification on connect. using additional handle?*/
        my_lbs_send_sensor_notify(timestamp_ms);
        k_usleep(NOTIFICATION_PERIOD_MS);
        my_lbs_send_sensor_notify((uint32_t)sensor_data.pressure);
        k_usleep(NOTIFICATION_PERIOD_MS);
    }
    return 0;
}

static int8_t get_sensor_data(const struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev)
{
    int8_t rslt = 0;
    uint8_t int_status = 0x1;

    if (int_status & BMP5_INT_ASSERTED_DRDY)
    {
        /* Get Timestamp "*/
        timestamp_ms = k_uptime_get();
        /* New value for pressure in sensor_data.pressure and for temerature in sensor_data.temperature */    
        rslt = bmp5_get_sensor_data(&sensor_data, osr_odr_press_cfg, dev);
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