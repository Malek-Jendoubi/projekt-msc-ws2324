import re

new_file_lines: list = []


def parse_app():
    with open("phone_logs/Log.txt", 'r') as f:
        log_file_lines = f.readlines()

    # Regex to test against
    regex = (
        r"\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{2}-\w{"
        r"2}-\w{2}-\w{2}")

    lines_decoded = []

    # Extract the string from each line and store
    for line in log_file_lines:
        # Check for Application log lines
        if (line[0]) == "A":
            matches = re.search(regex, line)
            if matches:
                payload = matches.group()
                payload_chars = payload.split('-')
                line_decoded = ''

                for char in payload_chars:
                    # print(letter)
                    line_decoded = line_decoded + chr(int(char, 16))

                lines_decoded.append(line_decoded[:15])

    # print(lines_decoded)

    with open("./LOG.csv", "w") as destination_file:
        # write csv header
        destination_file.writelines("timestamp,pressure_values\n")

        # Write 1 line, skip 2 (garbage values in )
        for line in lines_decoded:
            print(line[:15])
            destination_file.write(line[:15] + "\n")

        destination_file.close()


if __name__ == "__main__":
    parse_app()
    print(f"Phone LOG converted and saved to: ./LOG.csv")
