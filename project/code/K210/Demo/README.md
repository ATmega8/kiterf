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
  ./build.sh 
  ```
  * note

    If you modify `CMakeLists.txt`, delete the `build` folder first

* Download firmware

  ```bash
  ./flash.sh
  ```
