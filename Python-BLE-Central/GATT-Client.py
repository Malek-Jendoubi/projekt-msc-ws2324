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
FILENAME = "pressure_log_data.csv"
i = 0


async def notification_handler(characteristic: BleakGATTCharacteristic, data: bytearray):
    """Simple notification handler which prints the data received."""
    sensor_value: int = int.from_bytes(data, byteorder='little', signed=False)

    file = open(FILENAME, "a")
    file.writelines(str(sensor_value))
    file.close()

    print("*")
    return


async def main():
    file = open(FILENAME, "w")
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
            file = open(FILENAME, "a")
            file.writelines(",\n")
            file.close()
            await asyncio.sleep(SLEEP_DURATION)
            await client.stop_notify(CHAR_PRES_UUID)


if __name__ == "__main__":
    asyncio.run(main())
