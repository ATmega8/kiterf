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
