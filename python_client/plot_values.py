from typing import List

import pandas as pd
import matplotlib.pyplot as plt

from build_csv import *

NOW = datetime.now().strftime("%Y-%m-%d_%H-%M")  # Timestamp for the file name
# Tags include but are not limited to: UPSTAIRS, DOWNSTAIRS, WALKING, STANDING, CALIBRATE
TAG = "CALIBRATE"
FILENAME_PNG = f"./pressure_logs/LOG_{TAG}_{NOW}.png"
FILENAME_TEST = f"./pressure_logs/LOG_CALIBRATE_2024-02-10_16-23.csv"


def plot_values(log_duration=0):
    df = pd.read_csv(FILENAME_TEST, sep=",")
    df.head()

    # Todo: Preprocessing:
    # Extract the data from csv
    # Time Dataframe
    x_data: pd.DataFrame = df['timestamp'] - df['timestamp'].iloc[0]
    # x_data.to_datetime(df['timestamp'], unit='ms')
    pressure_values = df['pressure_values']

    # Post-processing of data to get a measure of elevation
    elevation_data = df['pressure_values'].iloc[::-1]
    # Smooth the curves with a rolling average
    elevation_data_rm = df.rolling(window=5)['pressure_values'].mean()
    elevation_data_rm = elevation_data_rm
    print(elevation_data)
    print(elevation_data_rm)

    # TODO: Make a better plot
    # Label the plot
    plt.xlabel("Time in ms")
    plt.title(f'Measurements @{NOW}\n Activity: {TAG}\n Duration:{log_duration}s\n')

    # Plot the pressure data
    plt.subplot(3, 1, 1)
    plt.ylabel("Pressure in Pa")
    plt.plot(x_data, pressure_values, 'r-')

    # Plot the elevation data
    plt.subplot(3, 1, 2)
    plt.ylabel("Elevation level")
    plt.plot(x_data, elevation_data, 'r')

    # Plot the elevation data
    plt.subplot(3, 1, 3)
    plt.ylabel("Elevation level")
    plt.plot(x_data, elevation_data_rm, 'r')

    # Show the figure
    plt.show()

    # Save the figure as PNG
    # fig = plt.get_figure()
    plt.savefig(FILENAME_PNG)

    res = elevation_data_rm.plot().get_figure()
    # Save figure
    res.savefig(f'./pressure_logs/LOG_{NOW}.png')

    print(f"Figure saved to: {FILENAME_PNG}")


if __name__ == "__main__":
    plot_values()
