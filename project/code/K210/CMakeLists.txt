cmake_minimum_required(VERSION 3.0)

## Required Variable: (if you do not use Kendryte IDE)
# -DTOOLCHAIN=/path/to/rsicv/toolchain/bin
# -DSDK=/path/to/SDK (the folder of this example file)

project(${PROJ})
## Include the SDK library
include("${SDK}/cmake/common.cmake")

## Use SDK library API to add source files
add_source_files("main/main.c"
		 "components/fft_soft/fft_soft.c"
		 )
header_directories("components/fft_soft/include")
# other cmake instructions

## Use SDK builder to build this project
include("${SDK}/cmake/executable.cmake")
add_subdirectory(${SDK_ROOT}/third_party THIRD_PARTY)

