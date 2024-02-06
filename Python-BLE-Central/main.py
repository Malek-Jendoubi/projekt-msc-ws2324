"""
TODO:   ** Error handling for all the steps in the connection process
        ** Input parameter for the filepath
        ** Input parameter for the log duration
        ** Exit value for further processing by the next chain
"""
import asyncio
import csv
from datetime import datetime
import pandas as pd

from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic

DEVICE_NAME = "BMP581"
CHAR_PRES_UUID = "00001526-1212-efde-1523-785feabcd123"

LOG_DURATION = 5  # Logging time in seconds
NOW = datetime.now().strftime("%Y-%m-%d@%H_%M_%S")
TAG = "_CALIBRATE"
FILENAME = f"./pressure_logs/pressure_log{TAG}_{NOW}"
FILE = FILENAME + ".csv"
FILE_PREPROCESSED = FILENAME + "_prepr.csv"
# FILE = "pressure_log.csv"


def write_to_file(value=None, file=FILE, permission="a"):
    file = open(file, permission)
    file.writelines(value)
    file.writelines(",\n")
    file.close()


async def notification_handler(characteristic: BleakGATTCharacteristic, data: bytearray):
    """Simple notification handler which prints the data received."""
    sensor_value: int = int.from_bytes(data, byteorder='little', signed=False)

    write_to_file(str(sensor_value),FILE_PREPROCESSED,"a")
    print("...")
    return


def correct_csv():
    # Open the source file in read mode and the destination file in write mode
    with open(FILE_PREPROCESSED, "r") as source_file, open(f"{FILE}_temp", "w") as destination_file:
        # Iterate over each line in the source file, enumerate to keep track of the line number
        for line_number, line in enumerate(source_file, 1):  # Starting index at 1 for easier modulo operation
            if line_number % 3 == 0:  # Check if it's every third line (line numbers 3, 6, 9, etc.)
                destination_file.write(line[:-1])  # Write the current line to the destination file
                destination_file.write("\n")  # Write the current line to the destination file
            else:
                destination_file.write(line[:-1])  # Write the current line to the destination file
                destination_file.write(" ")  # Write the current line to the destination file
    # Open the source file in read mode and the destination file in write mode
    with open(f"{FILE}_temp", "r") as source_file, open(FILE, "w") as destination_file:
        # Read the content of the source file
        content = source_file.read()
        # Write the content to the destination file
        destination_file.write(content)
    print(f"Corrected csv file.")


async def main():
    open(FILE_PREPROCESSED, "w")
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
    correct_csv()

    print(f"LOG saved to file: {FILE}")


if __name__ == "__main__":
    asyncio.run(main())