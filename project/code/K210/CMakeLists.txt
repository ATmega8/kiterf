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
                 "components/mmo/matrix_add.c" 
                 "components/mmo/matrix_inv.c" 
                 "components/mmo/matrix_mul.c" 
                 "components/mmo/matrix_sub.c" 
                 "components/mmo/matrix.c" 
                 "components/mmo/test/matrix_add_test.c" 
                 "components/mmo/test/matrix_inv_test.c" 
                 "components/mmo/test/matrix_mul_test.c" 
                 "components/mmo/test/matrix_sub_test.c" 
                 "components/mmo/test/matrix_test.c" 
                 "components/unity/unity.c"
                 "components/ring_buf/ring_buf.c")
                 
## I don't know why I need to add every item !!!
header_directories("components/fft_soft/include")
header_directories("components/mmo/include")
header_directories("components/mmo/test")
header_directories("components/unity/include")
header_directories("components/ring_buf/include")
# other cmake instructions

## Use SDK builder to build this project
include("${SDK}/cmake/executable.cmake")
add_subdirectory(${SDK_ROOT}/third_party THIRD_PARTY)

