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
  make menuconfig
  make -j
  ```

* Download firmware

  ```bash
  make flash monitor
  ```
