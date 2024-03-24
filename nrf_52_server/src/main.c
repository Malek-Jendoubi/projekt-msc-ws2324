/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*INCLUDES*/
#include "main.h"
#include "bt-periph.h"
#include "bmp5.h"

/* Declare the LED devices*/
#define LED_RED_NODE DT_ALIAS(led0)  // LED_RED_NODE = led0 defined in the .dts file

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);

#define SAMPLING_INTERVAL_MS 6 

struct bmp5_sensor_data sensor_data;
uint32_t pressure_data = 0;

/* Declare BMP5 device variables*/
struct bmp5_dev dev;
struct bmp5_osr_odr_press_config osr_odr_press_cfg = {0};

int8_t bmp5_rslt;
int ret_led;

/*Variables for the frame -- Timestamp*/
uint32_t timestamp_ms = 0;

char frame_ts[10];
char frame_sensor[8];
static char frame_payload[SIZE_PAYLOAD];

/* Build packet to send out by associating a sensor value to its pressure sensor reading*/
void new_packet()
{
    sprintf(frame_sensor, "%06lu", (unsigned long)sensor_data.pressure);
    sprintf(frame_ts, "%08lu", (unsigned long)timestamp_ms);

    /* Frame : XXXXXX,XXXXXXXX */
    sprintf(frame_payload, "%s,%s\n", frame_ts, frame_sensor);
}

int main(void)
{
    /* Initialize and check LED devices*/
    if (!gpio_is_ready_dt(&led_red)){
        return 0;
    }

    ret_led = gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_INACTIVE);
    if (ret_led < 0){
        return 0;
    }

    bmp5_rslt = bmp5_interface_init(&dev, BMP5_I2C_INTF);
    if (bmp5_rslt == BMP5_OK)
    {
        bmp5_soft_reset(&dev);

        bmp5_rslt = bmp5_init(&dev);

        if (bmp5_rslt == BMP5_OK)
        {
            bmp5_rslt = set_config(&osr_odr_press_cfg, &dev);
        }
    }

    /* Initial sensor values*/
    get_sensor_data(&osr_odr_press_cfg, &dev);

    gpio_pin_set_dt(&led_red, 1);

    /* Start BLE stack and setup/run GATT Server*/
    bluetooth_advertiser_init();

    while (1)
    {
        /*Get sensor data from the BMP581*/
        bmp5_rslt = get_sensor_data(&osr_odr_press_cfg, &dev);

        /* Make a new char array packet*/
        new_packet();
        /* Send the packet to the characteristic*/
        sensor_notify(frame_payload);
        k_msleep(SAMPLING_INTERVAL_MS);
    }

    return 0;
}

int8_t get_sensor_data(const struct bmp5_osr_odr_press_config *osr_odr_press_cfg, struct bmp5_dev *dev)
{
    int8_t rslt = 0;
    uint8_t int_status = 0x1;

    if (int_status & BMP5_INT_ASSERTED_DRDY)
    {
        /* Get Timestamp "*/
        timestamp_ms = k_uptime_get_32();
        /* New value for pressure in sensor_data.pressure and for temerature in sensor_data.temperature */
        rslt = bmp5_get_sensor_data(&sensor_data, osr_odr_press_cfg, dev);
    }

    return rslt;
}
