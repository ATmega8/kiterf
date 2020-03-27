# _KiteRF_

[![Build](https://travis-ci.org/ATmega8/kiterf.svg?branch=master "build badge")](https://travis-ci.org/ATmega8/kiterf "build")
[![Release](https://img.shields.io/github/release/ATmega8/kiterf.svg "release badge")](https://github.com/ATmega8/kiterf/releases/latest/ "release")
[![Release](https://img.shields.io/github/issues/ATmega8/kiterf.svg "issues badge")](https://github.com/ATmega8/kiterf/issues "issues")

_KiteRF is a SDR system with K210 & FPGA & ESP32 as its core controller, which make SDR more intelligent, portable and cheap._

* KiteRF Structure

  ![KiteRF](data/kiterf_structure.svg)

  * DS (Digital Signal) unit

    * FPGA (Field-Programmable Gate Array)

      * Radio Frequency Data Processing

      * DS Control

      * Communication with PC

    * K210 (Kendryte K210)

      * LCD control

      * Camera control

      * Audio

      * FFT

    * ESP32 (Espressif ESP32)

      * Wi-Fi Data Transmission

      * SDCARD control

      * Touch Button

      * RGB LED Control

  * RF (Radio Frequency) unit

    * ADC (Analog-to-Digital Converter)
    
      * Sampling Analog Baseband IQ Data

    * DAC (Digital-to-Analog Converter)

      * Output Analog Baseband IQ Data 

    * RF Transceiver

      * Mixing IF signal to baseband and outputting IQ analog signal

    * VCO (Voltage-Controlled Oscillator)

      * Provide local IF frequency

    * Mixer (Frequency mixer)

      * Mixing HF signal to IF signal

    * LNA (Low-Noise Amplifier)

      * Suppression of Mirror Noise Signal

    * RF Switch

      * Switching signal paths in different frequencies

* KiteRF core-board v1.0.0

  ![KiteRF](data/kiterf_1.0.0.jpg)

## Contents

![Contents](https://img.shields.io/github/repo-size/ATmega8/kiterf.svg)

* Directory tree

    ```
    ├── add_path.sh
    ├── data
    │   ├── kendryte_datasheet.pdf
    │   ├── kendryte_freertos_programming_guide.pdf
    │   ├── kiterf_1.0.0.jpg
    │   ├── kiterf_structure.pos
    │   └── kiterf_structure.svg
    ├── docs
    │   └── Doxyfile
    ├── LICENSE
    ├── project
    │   ├── code
    │   │   ├── ESP32
    │   │   ├── ESP8266
    │   │   └── K210
    │   └── pcb
    │       ├── K210
    │       └── RF
    ├── README.md
    └── tools
        ├── cmake-3.14.2
        ├── ESP32
        │   └── esp-idf
        ├── ESP8266
        │   ├── ESP8266_RTOS_SDK
        │   └── xtensa-lx106-elf
        └── K210
            ├── kendryte-freertos-sdk
            ├── kendryte-toolchain
            └── kflash
    ```

    * project

      * PCB project (KiCAD)

      * FPGA project (ISE)

      * ESP32 project (esp-idf)

      * K210 project (Kendryte FreeRTOS)

    * release

      * PCB Gerber

      * FPGA bin

      * ESP32 bin

      * K210 bin

    * tools

      * script

      * SDK

      * toolchain

## How to use

You can follow the steps below to set up the development environment, or directly download the release version of the full environment.

* clone

  ```bash
  git clone --recursive https://github.com/ATmega8/kiterf
  cd kiterf
  ```

  * note

    Don't omit `--recursive`, because we use submodule.

* update

  ```bash
  git pull
  git submodule update --init --recursive
  ```

* Add environment variables

  ```bash
  . add_path.sh
  ```
  * note

    Don't forget `"."`

* Install toolchain

  * K210

    * Ubuntu/Debian/Kali

      ```bash
      wget https://www.kiterf.xyz/download/kendryte-toolchain-ubuntu-amd64-8.2.0-20190213.tar.gz
      tar zxvf kendryte-toolchain-ubuntu-amd64-8.2.0-20190213.tar.gz -C tools/K210/
      rm kendryte-toolchain-ubuntu-amd64-8.2.0-20190213.tar.gz
      ```
    
    * Arch

      ```bash
      wget https://www.kiterf.xyz/download/kendryte-toolchain-arch-amd64-8.2.0-20190415.tar.gz
      tar zxvf kendryte-toolchain-arch-amd64-8.2.0-20190415.tar.gz -C tools/K210/
      rm kendryte-toolchain-arch-amd64-8.2.0-20190415.tar.gz
      ```

  * ESP32

    * Ubuntu/Debian/Kali/Arch

      ```bash
      cd tools/ESP32/esp-idf
      ./install.sh
      cd $KITERF_PATH
      ```

  * ESP8266

    KiteRF also integrates the ESP8266 development environment, which is not necessary, and you can choose to ignore it.

    * Ubuntu/Debian/Kali/Arch

      ```bash
      wget https://www.kiterf.xyz/download/xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz
      tar zxvf xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz -C tools/ESP8266/
      rm xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz
      ```

  * CMake

    * Ubuntu/Debian/Kali/Arch

      ```bash
      wget https://www.kiterf.xyz/download/cmake-3.14.2.tar.gz
      tar xf cmake-3.14.2.tar.gz -C tools/
      cd tools/cmake-3.14.2
      ./configure
      # Please do not use all CPU cores to compile, otherwise your computer may crash.
      make -j2
      cd $KITERF_PATH
      rm cmake-3.14.2.tar.gz
      ```

## Release

* download

  ```bash
  wget https://www.kiterf.xyz/download/kiterf-debain-amd64-1.2.0-20190517.tar.gz
  tar zxvf kiterf-debain-amd64-1.2.0-20190517.tar.gz
  rm kiterf-debain-amd64-1.2.0-20190517.tar.gz
  cd kiterf
  ```
* Add environment variables

  ```bash
  . add_path.sh
  ```
  * note

    Don't forget `"."`

## License

[![License](https://img.shields.io/github/license/ATmega8/kiterf.svg)](LICENSE)

* MIT License
 
  A short and simple permissive license with conditions only requiring preservation of copyright and license notices. Licensed works, modifications, and larger works may be distributed under different terms and without source code.

## Contributing

![Stars](https://img.shields.io/github/stars/ATmega8/kiterf.svg?style=social)

If you want to support this project, we hope you can contribute your code or circuit.
