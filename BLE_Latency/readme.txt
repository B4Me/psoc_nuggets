Example that can be used to measure latency between Central and Peripheral

Peripheral.cydsn:
GATT Server that uses GPIO P3.3 to signal received write command
PA_EN is output on P5.0 for TX reference

Central.cydsn:
GATT Client that uses GPIO P3.3 to signal start of GATT Write API call
PA_EN is output on P5.0 for TX reference

Tested using CY8CKIT-042-BLE kits

The *.bmp oscilloscope plots contains some results

DS0006.BMP:
Indeterministic delay from API call to next connection event

DS0007.BMP:
Approximately 350us from connection event start to event received on GATT server

DS0008.BMP:
Ack timing relative to received event

Channel 1 (Yellow) = P3.3 on Peripheral, Write command event received on GATT Server
Channel 2 (Blue) = P5.0 on Central, Radio Tx from GATT Client (connection events)
Channel 3 (Purple) = P3.3 on Central, GATT API write call start
Channel 4 (Green) = P5.0 on Peripheral, Radio Tx (ack) from GATT Server

