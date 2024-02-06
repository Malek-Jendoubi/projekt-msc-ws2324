"""
TODO:   ** Error handling for all the steps in the connection process
        ** Input parameter for the filepath
        ** Input parameter for the log duration
        ** Exit value for further processing by the next chain
"""
import asyncio
from datetime import datetime
import pandas as pd
import matplotlib.pyplot as plt

from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic

DEVICE_NAME = "BMP581"
CHAR_PRES_UUID = "00001526-1212-efde-1523-785feabcd123"

LOG_DURATION = 10  # Logging time in seconds
NOW = datetime.now().strftime("%Y-%m-%d_%H_%M_%S")
TAG = "_CALIBRATE"
FILENAME = f"./pressure_logs/pressure_log{TAG}_{NOW}.csv"
FILE_PREPROCESSED = "pressure_log_prepr.csv"


def plot_values():
    df = pd.read_csv(FILENAME, sep=" ")
    df.head()

    X_Data = df['timestamp']
    Y_Data = df['pressure_values']

    # Plot the Data
    plt.plot(X_Data, Y_Data, 'r-', lw=1)
    plt.show()

    # Save the plot as PNG
    FILENAME_PNG = FILENAME[:-2] + '.png'
    plt.savefig(FILENAME_PNG)

def write_to_file(str_value=None, file=FILENAME, permission="a"):
    file = open(file, permission)
    file.writelines(str_value)
    file.writelines("\n")
    file.close()


async def notification_handler(characteristic: BleakGATTCharacteristic, data: bytearray):
    """Simple notification handler which prints the data received."""
    sensor_value: int = int.from_bytes(data, byteorder='little', signed=False)

    write_to_file(str(sensor_value), FILE_PREPROCESSED, "a")
    return


def correct_csv():
    # Open the source file in read mode and the destination file in write mode
    with open(FILE_PREPROCESSED, "r") as source_file, open(f"{FILENAME}_temp", "w") as destination_file:
        # Iterate over each line in the source file, enumerate to keep track of the line number
        for line_number, line in enumerate(source_file, 1):  # Starting index at 1 for easier modulo operation
            if line_number % 2 == 0:  # Check if it's skip line (line numbers 2, 4, 5, etc.)
                destination_file.write(line.strip())  # Write the current line to the destination file
                destination_file.write("\n")  # Write the current line to the destination file
            else:
                destination_file.write(line.strip())  # Write the current line to the destination file
                destination_file.write(" ")  # Write the current line to the destination file
    # Open the source file in read mode and the destination file in write mode

    with open(f"{FILENAME}_temp", "r") as source_file, open(FILENAME, "w") as destination_file:
        destination_file.writelines("timestamp pressure_values\n")
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
        print(f"Gathering samples for {LOG_DURATION} seconds...")
        await asyncio.sleep(LOG_DURATION)
        await client.stop_notify(CHAR_PRES_UUID)

    correct_csv()
    print(f"LOG saved to file: {FILENAME}")

    print(f"Plotting values of {FILENAME} ...")
    plot_values()


if __name__ == "__main__":
    asyncio.run(main())
