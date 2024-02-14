import argparse

import bmp581_client
from bmp581_client import *
from nrf_log_parser import parse_app

if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        prog='BMP581_Client',
        description='Connect to BMP581 Sensor,\r\n '
                    'get the sensor pressure value, '
                    'parse the received data, '
                    'plot the pressure values',
        epilog='bottom text')

    parser.add_argument(
        "duration",
        action="store_true",
        type=int,
        help="Duration of the log session",
    )

    parser.add_argument(
        "-du",
        "--duration_unit",
        action="store_true",
        type=str,
        help="Unit of the Duration of the log session: s for seconds or m for minutes",
    )

    args = parser.parse_args()
    print(args)

    # Pre-calculate the log_duration = log_time * unit
    log_duration = args.duration * (30 if args.duration_unit == "m" else 1)
    print(f"log_duration={log_duration} seconds.")

    if log_duration:
        print(f'bmp581_client({args.duration})')
        bmp581_client(log_duration)
        file_path_csv: str = build_csv()
        print(f"file_path_csv: str = {file_path_csv}")
        plot_values(file_path_csv, log_duration)
        print(f"plot_values({file_path_csv}, {log_duration})")
    else:
        print("Plotting phone_logs/Log.txt ...")
        parse_app()
        print(f"file_path_csv: str = {file_path_csv}")
        plot_values(file_path_csv, log_duration)
        print(f"plot_values({file_path_csv}, {log_duration})")


