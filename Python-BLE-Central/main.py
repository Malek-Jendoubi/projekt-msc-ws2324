"""
TODO:   ** Error handling for all the steps in the connection process
        ** Input parameter for the filepath
        ** Input parameter for the log duration
        ** Exit value for further processing by the next chain
"""
import asyncio

from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic

LOG_DURATION = 5  # Logging time in seconds

DEVICE_NAME = "BMP581"
CHAR_PRES_UUID = "00001526-1212-efde-1523-785feabcd123"
FILENAME = "pressure_log_data.csv"


def write_to_file(value=None, file=FILENAME, permission="a"):
    file = open(file, permission)
    file.writelines(value)
    file.writelines(",\n")
    file.close()


async def notification_handler(characteristic: BleakGATTCharacteristic, data: bytearray):
    """Simple notification handler which prints the data received."""
    sensor_value: int = int.from_bytes(data, byteorder='little', signed=False)

    write_to_file(str(sensor_value))

    print("*")
    return


async def main():
    write_to_file("sensor_value,\ntemperature,\ntimestamp", FILENAME, "w")

    print("Looking for BMP581...")
    device = await BleakScanner.find_device_by_name(DEVICE_NAME)

    if device is None:
        print(f"Could not find device with {DEVICE_NAME}")
        return

    print("Connecting to device...")

    async with BleakClient(device) as client:
        print("Connected")

        await client.start_notify(CHAR_PRES_UUID, notification_handler)
        await asyncio.sleep(LOG_DURATION)
        await client.stop_notify(CHAR_PRES_UUID)

    print("LOG FINISHED")


if __name__ == "__main__":
    asyncio.run(main())
