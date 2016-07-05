Example that extends the CapSense Tuner over BLE

CapSense_Target.cydsn:
CapSense project with two touch buttons
CapSense Tuner works both via normal EZI2C interface and Bluetooth LE
Implements BLE GATT server with custom CapSense Tuner service 
Runs on CY8CKIT-042-BLE kit with CY8CKIT-143A PSoC 4 BLE module

Dongle.cydsn:
Connects to CapSense Tuner application via I2C
Implements GATT client for custom CapSense Tuner service
Runs on CY5677 dongle


The custom CapSense Tuner service contains two custom characteristics:
1. Data Buffer is used for sending CapSense data via notifications to GATT client
2. EZI2C Write is used for sending I2C writes from Tuner to GATT server 