# _KiteRF_

![Version](https://img.shields.io/badge/version-1.0.0-yellow.svg)
[![Release](https://img.shields.io/github/release/ATmega8/kiterf.svg "release badge")](https://github.com/ATmega8/kiterf/releases/latest/ "release")
[![Release](https://img.shields.io/github/issues/ATmega8/kiterf.svg "issues badge")](https://github.com/ATmega8/kiterf/issues "issues")

_KieRF is an SDR system with k210 as its core controller, which makes SDR more intelligent, portable and cheap._

* KiteRF structure

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

      * Wi-Fi Data Transfer

      * SDCARD control

      * Touch Button

      * RGB LED Control

  * RF (Radio Freqency) unit

    * ADC (Analog-to-Digital Converter)
    
      * Sampling Analog Baseband IQ Data

    * DAC (Digital-to-Analog Converter)

      * Output Analog Baseband IQ Data 

    * RF Transceiver

      * Mixing IF signal to baseband and outputting IQ analog signal

    * VCO (Voltage-controlled oscillator)

      * Provide local IF frequency

    * Mixer (Frequency mixer)

      * Mixing HF signal to IF signal

    * LNA (Low-noise Amplifier)

      * Suppression of Mirror Noise Signal

    * RF Switch

      * Switching signal paths at different frequencies

* KiteRF coreborad v1.0.0

  ![KiteRF](data/kiterf_1.0.0.jpg)

## Contents

![Contents](https://img.shields.io/github/repo-size/ATmega8/kiterf.svg)

* Directory tree

    ```
    ├── data
    │   ├── kiterf_1.0.0.jpg
    │   ├── kiterf_structure.pos
    │   └── kiterf_structure.svg
    ├── docs
    ├── LICENSE
    ├── project
    │   └── pcb
    │       ├── ESP32_K210.bak
    │       ├── ESP32_K210-cache.lib
    │       ├── ESP32_K210.csv
    │       ├── ESP32_K210.kicad_pcb
    │       ├── ESP32_K210.kicad_pcb-bak
    │       ├── ESP32_K210.net
    │       ├── ESP32_K210.png
    │       ├── ESP32_K210.pro
    │       ├── ESP32_K210-rescue.dcm
    │       ├── ESP32_K210-rescue.lib
    │       ├── ESP32_K210.sch
    │       ├── ESP32_K210.xlsx
    │       ├── ESP32_K210.xml
    │       ├── FPC.pretty
    │       ├── fp-info-cache
    │       ├── K210.bak
    │       ├── K210_Gerber
    │       ├── k210.pretty
    │       ├── K210.sch
    │       ├── lib
    │       ├── POWER.bak
    │       ├── POWER.sch
    │       ├── sym-lib-table
    │       ├── USB2UART.bak
    │       └── USB2UART.sch
    ├── README.md
    ├── release
    └── tools
    ```

    * project

      * PCB project (KiCAD)

      * FPGA project (ISE)

      * esp-32 project (esp-idf)

      * K210 project (Kendryte FreeRTOS)

    * release

      * pcb gerber

      * FPGA bin

      * ESP32 bin

      * K210 bin

    * tools

      * script

      * sdk

      * toolchain

## License

[![License](https://img.shields.io/github/license/ATmega8/kiterf.svg)](LICENSE)

* MIT License
 
  A short and simple permissive license with conditions only requiring preservation of copyright and license notices. Licensed works, modifications, and larger works may be distributed under different terms and without source code.

## Contributing

![Stars](https://img.shields.io/github/stars/ATmega8/kiterf.svg?style=social)

If you want to support this project, we hope you can contribute your code or circuit.
