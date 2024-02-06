# Projekt Msc. Elektrotechnik Malek Jendoubi - Wintersemester 2022/2023
---
Projekt besteht aus zwei teile:

* Ein BLE-Server (nRF52832) empfängt Luftdruckwerte von einem BMP581-Sensor von Bosch Sensortec über eine I2C-Verbindung. Er gibt sie über einen BLE-GATT-Server aus. 

* Ein Python-Client auf einem Windows-PC stellt eine Verbindung zum NFR her und empfängt die Luftdruck- und Timestampwerte.

