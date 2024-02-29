from datetime import datetime

import numpy
import pandas as pd
import matplotlib.pyplot as plt


def plot_values():
    df = pd.read_csv("./LOG.csv", sep=",")
    df.head()

    # Todo: Preprocessing:
    # Extract the data from csv
    # Time Dataframe
    # print(df)
    df.sort_values(['timestamp'])
    x_data = df['timestamp'] - df['timestamp'][0]

    # Post-processing of data to get a measure of elevation
    # Inverse the df: Pressure is
    df['pressure_values'] = df['pressure_values'].iloc[::-1]

    df_running_mean = running_mean(df['pressure_values'], 10)
    # Subtract from Air Pressure Value at ground level
    elevation_values = 101325 - df['pressure_values']

    # TODO: Make a better plot
    # Label the plot
    plt.title(f'Measurements of Elevation\n')

    # Plot the pressure data
    plt.plot()
    plt.ylabel("Elevation Measure / Pressure at sea level (101325 Pa)")
    plt.xlabel("Time in ms")
    plt.style.use('fast')
    plt.plot(x_data, elevation_values)

    now = datetime.now().strftime("%Y-%m-%d_%H-%M")  # Timestamp for the file name

    # Save the figure as PNG
    plt.savefig(f"./plots/FIGURE_{now}.PNG", dpi=1000)
    
    # Save .csv LOG
    with open("./LOG.csv", 'r') as f:
        f_csv = open(f"./plots/LOG_{now}.csv", 'w')
        lines_csv = f.readlines()
        f_csv.writelines(lines_csv)
        f_csv.close()

    # Show the figure
    plt.show()

    print(f"Figure saved to ./FIGURE_{now}.PNG")
    print(f"Values saved to ./LOG_{now}.PNG")


def running_mean(data, window_width):
    cumsum = numpy.cumsum(numpy.insert(data, 0, 0))
    return (cumsum[window_width:] - cumsum[:-window_width]) / float(window_width)


if __name__ == "__main__":
    plot_values()
