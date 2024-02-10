#include <zephyr/types.h>
/* Bluetooth Stack Include*/
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/addr.h>
/* For managing Bluetooth LE Connections */
#include <zephyr/bluetooth/conn.h>

#include "main.h"

static char frame_payload[20];

/* LBS Service UUID. */
#define BT_UUID_LBS_VAL \
	BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)
#define BT_UUID_LBS BT_UUID_DECLARE_128(BT_UUID_LBS_VAL)

/* MYSENSOR characteristic UUID */
#define BT_UUID_LBS_MYSENSOR_VAL \
	BT_UUID_128_ENCODE(0x00001526, 0x1212, 0xefde, 0x1523, 0x785feabcd123)
#define BT_UUID_LBS_MYSENSOR BT_UUID_DECLARE_128(BT_UUID_LBS_MYSENSOR_VAL)

void bluetooth_advertiser_init();

void sensor_notify(char* buf);

