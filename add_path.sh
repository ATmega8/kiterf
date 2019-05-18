## KiteRF PATH Register
export KITERF_PATH=$PWD

## TOOL PATH Register
export TOOL_PATH=$KITERF_PATH/tools
export K210_TOOLCHAIN_PATH=$TOOL_PATH/K210/kendryte-toolchain/bin
export K210_SDK_PATH=$TOOL_PATH/K210/kendryte-freertos-sdk
export K210_KFLASH_PATH=$TOOL_PATH/K210/kflash 
export LD_LIBRARY_PATH=$K210_TOOLCHAIN_PATH:$LD_LIBRARY_PATH
export PATH=$TOOL_PATH/ESP32/xtensa-esp32-elf/bin:$TOOL_PATH/ESP32/esp-idf/tools:$TOOL_PATH/ESP8266/xtensa-lx106-elf/bin:$PATH
IDF=$1
if [  -n "$IDF" ] ;then
    input=$IDF
else
    echo "Choose ESP32 or ESP8266(0=ESP32, 1=ESP8266)"
    echo "enter (0/1, default 0):"
    read input
fi

if [ $input == 1 ]; then
    export IDF_PATH=$TOOL_PATH/ESP8266/ESP8266_RTOS_SDK
    echo "Register ESP8266 IDF PATH"
else
    export IDF_PATH=$TOOL_PATH/ESP32/esp-idf
    echo "Register ESP32 IDF PATH"
fi
export PATH=$KITERF_PATH/tools/cmake-3.14.2/bin:$PATH