## KiteRF PATH Register
export KITERF_PATH=$PWD

## TOOL PATH Register
export TOOL_PATH=$KITERF_PATH/tools
export K210_TOOLCHAIN_PATH=$TOOL_PATH/K210/kendryte-toolchain/bin
export K210_SDK_PATH=$TOOL_PATH/K210/kendryte-freertos-sdk
export K210_KFLASH_PATH=$TOOL_PATH/K210/kflash 
export LD_LIBRARY_PATH=$K210_TOOLCHAIN_PATH:$LD_LIBRARY_PATH
export PATH=$TOOL_PATH/ESP32/xtensa-esp32-elf/bin:$TOOL_PATH/ESP32/esp-idf/tools:$PATH
export IDF_PATH=$TOOL_PATH/ESP32/esp-idf
export PATH=$KITERF_PATH/tools/cmake-3.14.2/bin:$PATH