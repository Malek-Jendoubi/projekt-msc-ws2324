import argparse

import bmp581_client
from bmp581_client import *
from nrf_log_parser import parse_app

if __name__ == "__main__":

    valid_input = False
    log_duration = 0

    while not valid_input:
        try:
            log_duration = int(input("Enter the desired log duration in seconds:"))
            valid_input = True
        except ValueError:
            print("Invalid input. Please enter a valid log duration. 0 to take the data from Log.csv\n")
            valid_input = False

    # log_duration = args.duration * (30 if args.duration_unit == "m" else 1)
    print(f"log_duration={log_duration} seconds.")

    if log_duration > 0:
        result = 0
        print(f'bmp581_client({log_duration})')
        result = bmp581_client(log_duration)
    else:
        print("No duration specified.Will plot the pressure values from Log.txt")
        print("Parsing Log.txt ...")
        parse_app()
        print(f"Log.csv saved.")
        plot_values()
        print(f"Plot Values from Log.csv")

