Example that measures throughput when using the Low Energy Data Length Extension feature introduced in Bluetooth 4.2

Peripheral.cydsn:
GATT Server that receives data over custom characteristic. Verifies data integrity by using a sequence counter and disconnects on error.

Central.cydsn:
GATT Client that sends data by using the ATT Write Command. It also measures time, calculates throughput and outputs result on UART.

Tested using CY8CKIT-042-BLE kits with BT 4.2 radio modules.

Measured throughput results are above 99.7% of theoretical maximum BLE throughput when boards are placed in close proximity. Measured throughput when using the full 251 byte packet length is 789893 bits/second. The theoretical maximum payload throughput when using ATT Write Command and an attribute MTU size of 247 would be 244 bytes in 2468 us, 790924 bits/second.
  

Example output:

Starting GAP Central
Connected to GAP Peripheral
ATT_MTU,Bytes Sent,Time(ms),Throughput(kbps)
23,500000,16937,236169
23,500000,16936,236183
23,500000,16936,236183
247,500200,5066,789893
247,500200,5066,789893
247,500200,5066,789893