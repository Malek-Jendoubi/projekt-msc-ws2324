# Feasibility Study of Human Activity Recognition with Air Pressure Sensors

> Projekt Msc. Elektrotechnik Malek Jendoubi - Wintersemester 2023/2024

## Tasklist

* familiarization with the air pressure sensor, the microcontroller board, and its development tools. **_DONE_**
* implementation of the sensors driver and the reading of sensor data via I2C. **_DONE_**
* implementation of a Bluetooth Low Energy transmission of the sensor data. **_DONE_**
* recording of air pressure signals for different walking scenarios when the
sensor is attached to the shoe or wrist. **_IN WORK_**
* discussion of the acquired sensor signals regarding its usefulness for activity recognition
* documentation of the acquired results

## Proposed Solution

The proposed solution is two parts:

* A BLE-Enabled nRF52832-Mikrocontroller:
  * Receives air pressure values from a BMP581 sensor via an I2C connection.
  * It outputs the values and their timestamps via a BLE GATT server.
* A Python client on a Windows PC:
  * Establishes a connection to the GATT Server on the nRF and receives the air pressure and timestamp values.
  * Formats the values into a csv
  * Plots the values of pressure as a function of time

## Get started with the project

### For the server side

1. Flash the code onto the uC: This code was written for the nRF52832 and was tested on it alone, but should work for similar boards with a few minor adjustments.
2. That's it. You should already see the device withe the name BMP581 if you do a scan for BLE devices.
3. If not, try to reset the board.

### For the client side

1. Navigate to the directory `python_client/`.
2. Run the command `python gatt_client.py`.
3. Wait for the sampling period as the nRF52 collects the data and sends it to the subscribed python Client.
4. You can find the generated files under `python_client/pressure_logs`

You can change the duration of the log session in seconds by changing this variable in `gatt_client.py`:
`LOG_DURATION = 10`
