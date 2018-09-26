# ESP32-Access-Point
Example code to configure the ESP32 as a WiFi access point

## Notes

### Requirements

Make sure you have first uploaded the `ESP32_flash_loader.ino` under the `extras` folder into the board.

### Start ESP32 in boot mode

1. Remove power from board
2. Connect ESP32 GPIO 0 pin to ground
3. With the previous connection done, power the board

### Arduino settings to program the ESP32

* Upload Speed: 115200
* Flash Frequency: 80 MHz
* Flash Mode: DIO
* Flash Size: 4MB (32 Mb)
* Partition Scheme: Default
* Core Debug Level: None
* PSRAM: Disabled

### Check connection

After connecting your laptop to the ESP32 (you should see a BonaDrone network in the list of detected WiFi networks) you can test the connection via the script `simple_client.py` located under the `extras` folder. To do so:

1. Open Arduino's Serial Monitor
2. Open a terminal session in the `extras` directory
3. Execute `$ python simple_client.py -d DATA_TO_SEND` where `DATA_TO_SEND` should be replaced by the data you want to send.

If communication was successful you should see:
```
New Client
DATA_TO_SEND
Client Disconnected
```
in the Serial monitor.
