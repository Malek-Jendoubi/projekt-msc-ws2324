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

FILE_RAW = "./pressure_logs/RAW_LOG.csv"


def write_to_file(str_value=None, file=FILE_RAW, permission="a"):
    file = open(file, permission)
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
        write_to_file(str(sensor_value), FILE_RAW, "a")
        return


async def bmp581_client(log_duration=30):
    device_name = "BMP581"
    char_pres_uuid = "00001526-1212-efde-1523-785feabcd123"

    open(FILE_RAW, "w")
    print("Looking for BMP581...")

    try:
        device: BLEDevice = await BleakScanner.find_device_by_name(device_name)

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

        print(f"Trying to connect to {device}...")
    except IOError:
        print("No BLE Peripheral detected.\nActivate Bluetooth on your computer or update your driver.")
        return -1


if __name__ == "__main__":
    result: int = asyncio.run(bmp581_client())
