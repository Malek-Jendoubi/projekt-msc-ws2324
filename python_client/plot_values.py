from datetime import datetime

import pandas as pd
import matplotlib.pyplot as plt

NOW = datetime.now().strftime("%Y-%m-%d_%H_%M")  # Timestamp for the file name
# Tags include but are not limited to: UPSTAIRS, DOWNSTAIRS, WALKING, STANDING, CALIBRATE
TAG = "CALIBRATE"
FILENAME = f"./pressure_logs/LOG_{TAG}_{NOW}.csv"


def plot_values():
    df = pd.read_csv(FILENAME, sep=",")
    df.head()

    X_Data = df['timestamp']
    Y_Data = df['pressure_values']

    # Plot the Data
    # TODO: Make a better plot
    plt.plot(X_Data, Y_Data, 'r-', lw=1)
    plt.show()

    # Save the plot as PNG
    FILENAME_PNG = FILENAME[:-4] + '.png'
    plt.savefig(FILENAME_PNG)

    print(f"Figure saved to: {FILENAME_PNG}")


if __name__ == "__main__":
    build_csv()
