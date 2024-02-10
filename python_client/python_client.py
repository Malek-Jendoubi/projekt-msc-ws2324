"""
TODO:   ** Error handling
        ** Input parameter for log duration, tag

"""
import asyncio
from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic

from plot_values import *
from build_csv import *

SECONDS = 1
MINUTES = 60

LOG_DURATION = 10 * SECONDS  # Logging time

DEVICE_NAME = "BMP581"
CHAR_PRES_UUID = "00001526-1212-efde-1523-785feabcd123"

FILE_RAW = "./pressure_logs/RAW_LOG.csv"

NOW = datetime.now().strftime("%Y-%m-%d_%H-%M")  # Timestamp for the file name
# Tags include but are not limited to: UPSTAIRS, DOWNSTAIRS, WALKING, STANDING, CALIBRATE
TAG = "CALIBRATE_"
FILENAME_CSV = f"./pressure_logs/LOG_{TAG}{NOW}.csv"


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


async def main():
    open(FILE_RAW, "w")
    print("Looking for BMP581...")
    device = await BleakScanner.find_device_by_name(DEVICE_NAME)

    if device is None:
        print(f"Could not find device with {DEVICE_NAME}")
        return

    print(f"Trying to connect to {device}...")

    async with BleakClient(device, timeout=1) as client:
        print("Connected")
        await client.start_notify(CHAR_PRES_UUID, notification_handler)
        print(f"Gathering samples for {LOG_DURATION} seconds starting from {NOW}")
        await asyncio.sleep(LOG_DURATION)
        print("Disconnected")
        await client.stop_notify(CHAR_PRES_UUID)

    build_csv()

    plot_values()


if __name__ == "__main__":
    asyncio.run(main())
