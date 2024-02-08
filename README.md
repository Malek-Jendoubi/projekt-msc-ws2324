# Feasibility Study of Human Activity Recognition with Air Pressure Sensors

> Projekt Msc. Elektrotechnik Malek Jendoubi - Wintersemester 2023/2024

## Problematic
In recent technological advancements, air pressure sensors have emerged as a promising modality for human activity detection when strategically affixed to the human body. This master’s project focuses on harnessing the potential of air pressure sensors, offering a novel perspective on activity monitoring. Similar to accelerometers and magnetometers, air pressure sensors fabricated in Micro-Electro-Mechanical Systems (MEMS) technology feature a low power consumption, making them particularly suitable for wearable applications where energy efficiency is paramount. 

Additionally, the availability of new sensor technology with enhanced accuracy opens avenues for more precise and reliable human activity recognition. Notably, the use of air pressure sensors represents a relatively unexplored territory in research, providing an opportunity for the student to contribute to the growing field of sensor-based human-computer interaction.

In the proposed topic, a given microcontroller board should be programmed to _read sensor data_ via __I2C__ or SPI from an attached state-of-the-art Bosch Sensortec BMP581 MEMS air pressure sensor. The acquired data should be _transmitted via the Bluetooth Low Energy (BLE)_ chip integrated on the µC to a recording device. In order to verify the functionality of the setup, _datasets should be recorded_ of human activities like walking on level ground, up stairs, and downstairs with a few different placements of the sensors on the human body, e.g., foot and wrist. Finally, the quality of acquired datasets and the different sensor placements should be _discussed_ regarding their usefulness for the anticipated activity recognition purpose.

## Tasklist

* Familiarization with the air pressure sensor, the microcontroller board, and its development tools. **_DONE_**
* Implementation of the sensors driver and the reading of sensor data via I2C. **_DONE_**
* Implementation of a Bluetooth Low Energy transmission of the sensor data. **_DONE_**
* Recording of air pressure signals for different walking scenarios when the sensor is attached to the shoe or wrist. **_IN WORK_**
* Discussion of the acquired sensor signals regarding its usefulness for activity recognition.
* Documentation of the acquired results.

## Proposed Solution

The proposed solution is two parts:

* A BLE-Enabled nRF52832-Mikrocontroller:
  * Receives air pressure values from a BMP581 sensor via an I2C connection.
  * It outputs the values and their timestamps via a BLE GATT server.
* A Python client on a Windows PC:
  * Establishes a connection to the GATT Server on the nRF and receives the air pressure and timestamp values.
  * Plots the values of pressure as a function of time
 
## Requirements

### For the server side: nRF52

#### Hardware
The microcontroller is an nrf52832 on an nrf52 bluefruit feather board.

The air pressure sensors are BMP580/581 from Bosch Sensortec on shuttle boards that can be queried via SPI (possibly also I2C).

#### Software

The nrf52832 is programmed with the nrf Connect SDK, preferably in Visual Studio Code. There are very detailed tutorials on the nrf Connect SDK on the Internet, which you are welcome to take a look at.

### For the client side: Recording Device

* A PC with a BLE interface
* Python
* Python libs: TODO

## Get started with the project

### For the server side: Microcontroller

1. Flash the code onto the µC: This code was written for the nRF52832 and was tested on it alone, but should work for similar boards with a few minor adjustments.
2. That's it. You should already see the device withe the name BMP581 if you do a scan for BLE devices.
3. If not, try to reset the board.

### For the client side: Recording device

1. Navigate to the directory `python_client/`.
2. Run the command `python gatt_client.py`.
3. Wait for the sampling period as the nRF52 collects the data and sends it to the subscribed python Client.
4. You can find the generated files under `python_client/pressure_logs`

You can change the duration of the log session in seconds by changing this variable in `gatt_client.py`:
`LOG_DURATION = 10`
