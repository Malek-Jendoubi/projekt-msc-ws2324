"""
TODO:   ** Error handling for all the steps in the connection process
        ** Input parameter for the filepath, log duration, tag
        ** Exit value for further processing by the next chain
        ** Logs in different folders for each day.

"""
import asyncio
from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic

from datetime import datetime

import pandas as pd
import matplotlib.pyplot as plt


LOG_DURATION = 1  # Logging time in seconds

DEVICE_NAME = "BMP581"
CHAR_PRES_UUID = "00001526-1212-efde-1523-785feabcd123"

NOW = datetime.now().strftime("%Y-%m-%d_%H_%M")  # Timestamp for the file name
# Tags include but are not limited to: UPSTAIRS, DOWNSTAIRS, WALKING, STANDING, CALIBRATE
TAG = "CALIBRATE"   
FILENAME = f"./pressure_logs/LOG_{TAG}_{NOW}.csv"
FILE_RAW = "./pressure_logs/RAW_LOG.csv"  # .csv for gitignore sake


def plot_values():
    df = pd.read_csv(FILENAME, sep=",")
    df.head()

    X_Data = df['timestamp']
    Y_Data = df['pressure_values']

    # Plot the Data
    # TODO: Make a better plot
    plt.plot(X_Data, Y_Data, 'r-', lw=1)
    # plt.show()

    # Save the plot as PNG
    FILENAME_PNG = FILENAME[:-4] + '.png'
    plt.savefig(FILENAME_PNG)

    print(f"Figure saved to: {FILENAME_PNG}")


def build_csv():  # Build a csv from the file of raw data
    with open(FILE_RAW, "r") as source_file, open(FILENAME, "w") as destination_file:
        # write csv header
        destination_file.writelines("timestamp,pressure_values\n")
        lines = source_file.readlines()

        # Skip to every third line
        index = 0
        while index < len(lines):
            destination_file.write(lines[index])
            index += 3

        destination_file.close()
        source_file.close()

    print(f"LOG .csv saved to: {FILENAME}")


def write_to_file(str_value=None, file=FILENAME, permission="a"):
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

    print("Connecting to device...")

    async with BleakClient(device) as client:
        print("Connected")

        await client.start_notify(CHAR_PRES_UUID, notification_handler)
        print(f"Gathering samples for {LOG_DURATION} seconds...")
        await asyncio.sleep(LOG_DURATION)
        await client.stop_notify(CHAR_PRES_UUID)

    build_csv()

    print(f"Plotting values from {FILENAME} ...")
    plot_values()


if __name__ == "__main__":
    asyncio.run(main())
