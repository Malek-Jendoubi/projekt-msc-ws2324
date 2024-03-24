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

/* MY_SERVICE Service UUID. */
/* 75c276c3-8f97-20bc-a143-b354244886d4 */
#define BT_UUID_MY_SERVICE_VAL 0xd4, 0x86, 0x48, 0x24, 0x54, 0xB3, \
				0x43, 0xA1, \
			 	0xBC, 0x20, \
				0x97, 0x8F, \
				0xC3, 0x76, 0xC2, 0x75
#define BT_UUID_MY_SERVICE BT_UUID_DECLARE_128(BT_UUID_MY_SERVICE_VAL)

/* MYSENSOR characteristic UUID */
/* Should correspond to the char uuid in the py client*/
/* 75c276c4-8f97-20bc-a143-b354244886d4 */
#define BT_UUID_MY_SERVICE_SENSOR_VAL 0xd4, 0x86, 0x48, 0x24, 0x54, 0xB3, \
				0x43, 0xA1, \
			 	0xBC, 0x20, \
				0x97, 0x8F, \
				0xC4, 0x76, 0xC2, 0x75
#define BT_UUID_MY_SERVICE_SENSOR BT_UUID_DECLARE_128(BT_UUID_MY_SERVICE_SENSOR_VAL)

void bluetooth_advertiser_init();

void sensor_notify(char* buf);

