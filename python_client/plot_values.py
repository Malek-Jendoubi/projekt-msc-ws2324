import math
from typing import List

import pandas as pd
import matplotlib.pyplot as plt

from build_csv import *


def plot_values():
    now = datetime.now().strftime("%Y-%m-%d_%H-%M")  # Timestamp for the file name
    # Tags include but are not limited to: UPSTAIRS, DOWNSTAIRS, WALKING, STANDING, CALIBRATE
    tag = "CALIBRATE"
    filename_png = f"./LOG_{tag}_{now}.png"

    df = pd.read_csv("./LOG.csv", sep=",")
    df.head()

    # Todo: Preprocessing:
    # Extract the data from csv
    # Time Dataframe
    # print(df)
    df.sort_values(['timestamp'])
    x_data = df['timestamp']
    # print(x_data)
    # Post-processing of data to get a measure of elevation
    # Inverse the df: Pressure is
    # df['pressure_values'] = df['pressure_values'].iloc[::-1]
    # Subtract from Air Pressure Value at ground level
    elevation_values = 101325 - df['pressure_values']

    # TODO: Make a better plot
    # Label the plot
    plt.title(f'Measurements of Elevation\n')

    # Plot the pressure data
    plt.subplots(1)
    plt.ylabel("Pressure in Pa")
    plt.plot(x_data, elevation_values, 'r-')

    # Show the figure
    plt.show()

    # Save the figure as PNG
    plt.savefig(filename_png)

    print(f"Figure saved to: {filename_png}")


if __name__ == "__main__":
    plot_values()
