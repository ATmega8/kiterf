cmake_minimum_required(VERSION 3.0)

## Required Variable: (if you do not use Kendryte IDE)
# -DTOOLCHAIN=/path/to/rsicv/toolchain/bin
# -DSDK=/path/to/SDK (the folder of this example file)

project(${PROJ})
## Include the SDK library
include("${SDK}/cmake/common.cmake")

## Use SDK library API to add source files
add_source_files("main/*.c")
add_source_files("components/fft_soft/*.c")
add_source_files("components/mmo/*.c")
add_source_files("components/mmo/test/*.c") 
add_source_files("components/unity/*.c")
add_source_files("components/ring_buf/*.c")
add_source_files("components/dsp/basic/*.c" 
                 "components/dsp/complex/*.c" 
                 "components/dsp/controller/*.c" 
                 "components/dsp/fast/*.c" 
                 "components/dsp/filter/*.c" 
                 "components/dsp/matrix/*.c" 
                 "components/dsp/statistics/*.c" 
                 "components/dsp/tables/*.c" 
                 "components/dsp/transform/*.c")
add_source_files("components/fir/*.c")
add_source_files("components/pid/*.c")
add_source_files("components/littlevgl/lvgl/lv_core/*.c" 
                 "components/littlevgl/lvgl/lv_hal/*.c"
                 "components/littlevgl/lvgl/lv_objx/*.c"
                 "components/littlevgl/lvgl/lv_fonts/*.c" 
                 "components/littlevgl/lvgl/lv_misc/*.c"
                 "components/littlevgl/lvgl/lv_themes/*.c"
                 "components/littlevgl/lvgl/lv_draw/*.c")
                 
## I don't know why I need to add every item !!!
header_directories("components/fft_soft/include")
header_directories("components/mmo/include")
header_directories("components/mmo/test")
header_directories("components/unity/include")
header_directories("components/ring_buf/include")
header_directories("components/dsp//include")
header_directories("components/fir/include")
header_directories("components/pid/include")
header_directories("components/littlevgl/.")
header_directories("components/littlevgl/lvgl")
header_directories("components/littlevgl/lvgl/lv_core")
header_directories("components/littlevgl/lvgl/lv_hal")
header_directories("components/littlevgl/lvgl/lv_objx")
header_directories("components/littlevgl/lvgl/lv_fonts")
header_directories("components/littlevgl/lvgl/lv_misc")
header_directories("components/littlevgl/lvgl/lv_themes")
header_directories("components/littlevgl/lvgl/lv_draw") 

# other cmake instructions

## Use SDK builder to build this project
include("${SDK}/cmake/executable.cmake")
add_subdirectory(${SDK_ROOT}/third_party THIRD_PARTY)

