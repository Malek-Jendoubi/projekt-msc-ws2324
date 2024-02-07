from datetime import datetime


NOW = datetime.now().strftime("%Y-%m-%d_%H_%M")  # Timestamp for the file name
# Tags include but are not limited to: UPSTAIRS, DOWNSTAIRS, WALKING, STANDING, CALIBRATE
TAG = "CALIBRATE"
FILENAME = f"./pressure_logs/LOG_{TAG}_{NOW}.csv"
FILE_RAW = "./pressure_logs/RAW_LOG.csv"  # .csv for gitignore sake


def build_csv():  # Build a csv from the file of raw data
    with open(FILE_RAW, "r") as source_file, open(FILENAME, "w") as destination_file:
        # write csv header
        destination_file.writelines("timestamp,pressure_values\n")

        lines = source_file.readlines()

        index = 0
        while index < len(lines):
            destination_file.write(lines[index])
            index += 3

        destination_file.close()
        source_file.close()

    print(f"LOG .csv saved to: {FILENAME}")


if __name__ == "__main__":
    build_csv()