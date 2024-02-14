from datetime import datetime


def build_csv():  # Build a csv from the file of raw data
    now = datetime.now().strftime("%Y-%m-%d_%H-%M")  # Timestamp for the file name
    # Tags include but are not limited to: UPSTAIRS, DOWNSTAIRS, WALKING, STANDING, CALIBRATE
    tag = "CALIBRATE"
    filename = f"./pressure_logs/LOG.csv"
    file_raw = "./pressure_logs/RAW_LOG.csv"  # .csv for gitignore sake

    with open(file_raw, "r") as source_file, open(filename, "w") as destination_file:

        # write csv header
        destination_file.writelines("timestamp,pressure_values\n")

        lines = source_file.readlines()

        # Write 1 line, skip 2 (garbage values in )
        index = 0
        while index < len(lines):
            destination_file.write(lines[index])
            index += 2

        destination_file.close()
        source_file.close()

    print(f"LOG .csv saved to: {filename}")
    return filename


if __name__ == "__main__":
    build_csv()
