from typing import List

import pandas as pd
import matplotlib.pyplot as plt

from build_csv import *

NOW = datetime.now().strftime("%Y-%m-%d_%H-%M")  # Timestamp for the file name
# Tags include but are not limited to: UPSTAIRS, DOWNSTAIRS, WALKING, STANDING, CALIBRATE
TAG = "CALIBRATE"
FILENAME_PNG = f"./pressure_logs/LOG_{TAG}_{NOW}.png"
FILENAME_TEST = f"./pressure_logs/LOG_CALIBRATE_2024-02-10_16-23.csv"


def plot_values():
    df = pd.read_csv(FILENAME, sep=",")
    df.head()

    # Todo: Preprocessing:
    # Extract the data from csv
    x_data: pd.DataFrame = df['timestamp'] - df['timestamp'].iloc[0]
    # x_data.to_datetime(df['timestamp'], unit='ms')
    y_data = df['pressure_values']
    f, ax = plt.subplots(1)
    # TODO: Make a better plot
    # Label the plot
    plt.xlabel("Time in ms")
    plt.ylabel("Pressure in Pa")
    plt.title(f'Measurements @{NOW}\n Activity: {TAG} Duration:{x_data.iloc[-1]}')

    # Plot the Data
    ax.plot(x_data, y_data, 'r-')
    # ax.set_ylim(ymin=98000, ymax=103000)
    plt.show()

    # Save the plot as PNG
    plt.savefig(FILENAME_PNG)

    print(f"Figure saved to: {FILENAME_PNG}")


if __name__ == "__main__":
    plot_values()
