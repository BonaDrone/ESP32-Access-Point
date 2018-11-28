The command to flash the ESP32 is:

```
$ python esptool.py --chip esp32 --port /dev/ttyACM0 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 boot_app0.bin 0x1000 bootloader_dio_80m.bin 0x10000 ./bin/ESP32-Receiver.ino.bin 0x8000 ./bin/ESP32-Receiver.ino.partitions.bin
```

Where `ESP32-Receiver.ino.bin` and `ESP32-Receiver.ino.partitions.bin` should be replaced by the binaries of the code to be loaded. To find this binaries, activate the compilation and upload verbose options of your Arduino IDE (File > Preferences > Show verbose output during). After activating them, when a sketch is compiled or uploaded, the full trace of the commands will be listed in the console-like part of the IDE. There you can check from where the IDE is getting the files and grab them from there.

On Linux this folder is under the `/tmp/` dir and called `arduino_build_[...]`. So, the full path would be something like `/tmp/arduino_build_[...]`
