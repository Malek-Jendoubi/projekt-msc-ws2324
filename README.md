# Projekt Msc. Elektrotechnik Malek Jendoubi - Wintersemester 2022/2023

---

## Tasklist:

* familiarization with the air pressure sensor, the microcontroller board, and its development tools 
* implementation of the sensors driver and the reading of sensor data via I2C
* implementation of a Bluetooth Low Energy transmission of the sensor data
* recording of air pressure signals for different walking scenarios when the
sensor is attached to the shoe or wrist
* discussion of the acquired sensor signals regarding its usefulness for activity
recognition
* documentation of the acquired results

---

## Proposed Solution

The proposed solution is two parts:

* A BLE-Enabled nRF52832-Mikrocontroller:
  * Receives air pressure values from a BMP581 sensor via an I2C connection.
  * It outputs the values and their timestamps via a BLE GATT server.
* A Python client on a Windows PC:
  * Establishes a connection to the GATT Server on the nRF and receives the air pressure and timestamp values.
  * Formats the values into a csv
  * Plots the values of pressure as a function of time

