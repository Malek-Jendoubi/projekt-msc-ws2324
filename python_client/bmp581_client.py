"""
TODO:   ** Error handling:
                        bt device error     DONE
                        bleak client error  **
        ** Input parameter for log duration, tag

"""
import asyncio
import sys

from bleak import BleakClient, BleakScanner, BLEDevice
from bleak.backends.characteristic import BleakGATTCharacteristic

from plot_values import *
from build_csv import *


def write_to_file(str_value=None, permission="a"):
    file = open("./RAW_LOG.csv", permission)
    file.writelines(str_value)
    file.writelines("\n")
    file.close()


async def notification_handler(characteristic: BleakGATTCharacteristic, data: bytearray):
    """Simple notification handler which prints the data received."""
    try:
        sensor_value: str = data.decode('ascii')
    except UnicodeDecodeError:
        return
    else:
        write_to_file(str(sensor_value), "a")
        return


async def bmp581_client(log_duration=20):
    device_name = "BMP581"
    char_pres_uuid = "00001526-1212-efde-1523-785feabcd123"

    open("./RAW_LOG.csv", "w")
    print("Looking for BMP581...")

    try:
        device: BLEDevice = await BleakScanner.find_device_by_name(device_name)
        print(f"Trying to connect to {device}...")

        if device is None:
            print(f"Could not find device with {device_name}")
            return

        now = datetime.now().strftime("%Y-%m-%d_%H-%M")  # Timestamp for the file name

        async with BleakClient(device, timeout=5) as client:
            print("Connected")
            await client.start_notify(char_pres_uuid, notification_handler)
            print(f"Gathering samples for {log_duration} seconds starting from {now}")
            # TODO: Console Indication about time progress
            await asyncio.sleep(log_duration)
            print("Disconnected")
            await client.stop_notify(char_pres_uuid)

        print(f"LOG_RAW.csv saved.")
        build_csv()
        print(f"Log.csv saved.")
        plot_values()
        print(f"Plot Values from Log.csv")

    except IOError:
        print("No BLE Peripheral detected.\nActivate Bluetooth on your computer or update your driver.")
        return -1


if __name__ == "__main__":
    result: int = asyncio.run(bmp581_client())
