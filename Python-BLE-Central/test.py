# -*- coding: utf-8 -*-
"""
Notifications
-------------

Example showing how to add notifications to a characteristic and handle the responses.

Updated on 2019-07-03 by hbldh <henrik.blidh@gmail.com>

"""
import asyncio
import os

from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic

SLEEP_DURATION = 100e-3  # 100 ms sleep

DEVICE_NAME = "BMP581"
CHAR_PRES_UUID = "00001526-1212-efde-1523-785feabcd123"


async def notification_handler(characteristic: BleakGATTCharacteristic, data: bytearray, sensor_value_old=0):
    """Simple notification handler which prints the data received."""
    sensor_value: int = int.from_bytes(data, byteorder='little', signed=False)

    print(f"{characteristic.description}: {sensor_value}")

    return

async def main():
    print("Looking for BMP581...")
    device = await BleakScanner.find_device_by_name(DEVICE_NAME)

    if device is None:
        print(f"Could not find device with {DEVICE_NAME}")
        return

    print("Connecting to device...")

    async with BleakClient(device) as client:
        print("Connected")

        while True:
            await client.start_notify(CHAR_PRES_UUID, notification_handler)
            await asyncio.sleep(SLEEP_DURATION)
            await client.stop_notify(CHAR_PRES_UUID)


if __name__ == "__main__":
    asyncio.run(main())