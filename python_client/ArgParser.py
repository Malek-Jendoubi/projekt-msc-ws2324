# Parsing Program Arguments
import argparse

parser = argparse.ArgumentParser(
    prog='BMP581_Client',
    description='Connect to BMP581 Sensor,\r\n '
                'get the sensor pressure value, '
                'parse the received data, '
                'plot the pressure values',
    epilog='bottom text')

parser.add_argument(
    "--duration",
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