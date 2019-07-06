set(COMPONENT_SRCS "main.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

set(COMPONENT_REQUIRES fft_soft mmo dsp pid ring_buf spi_flash littlevgl)

register_component()