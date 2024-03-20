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

from nrf_log_parser import parse_app
from plot_values import *
from build_csv import *


def write_to_file(str_value=None):
    file = open("./RAW_LOG.csv", "a")
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
        write_to_file(str(sensor_value))
        return


async def bmp581_client(log_duration=20):
    valid_input = False
    log_duration = 0

    while not valid_input:
        try:
            log_duration = int(input("Enter the desired log duration in seconds:"))
            valid_input = True
        except ValueError:
            print("Invalid input. Please enter a valid log duration. 0 to take the data from Log.csv\n")
            valid_input = False

    if log_duration == 0:
        return -1

    # log_duration = args.duration * (30 if args.duration_unit == "m" else 1)
    print(f"log_duration={log_duration} seconds.")

    device_name = "BMP581"
    # cf. bt-periph.h mysensor char uuid
    char_pres_uuid = "75c276c4-8f97-20bc-a143-b354244886d4"

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

        return 0

    except IOError:
        print("No BLE Peripheral detected.\nActivate Bluetooth on your computer or update your driver.")
        return -1


if __name__ == "__main__":
    result: int = asyncio.run(bmp581_client())

    if result != 0:
        print("Will plot the pressure values from Log.txt")
        print("Parsing Log.txt ...")
        parse_app()
        print(f"Log.csv saved.")
        plot_values()
        print(f"Plot Values from Log.csv")

