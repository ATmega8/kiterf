## How to use

* Add environment variables

  First, go back to the KiteRF root directory, and then execute the environment variable script

  ```bash
  cd ../../../
  . add_path.sh
  ```
  * note

    Don't forget `"."`

* Compiling project
 
  ```bash
  idf.py menuconfig
  idf.py build
  ```

* Download firmware

  ```bash
  idf.py flash monitor
  ```

  ```bash
  python spiffsgen.py 0x1f0000 spiffs spiffs.bin
  esptool.py -p /dev/ttyUSB0 -b 460800 --after hard_reset write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x210000 spiffs.bin
  ```
