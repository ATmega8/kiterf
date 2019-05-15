set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS "include")
set(COMPONENT_SRCS "flash_buf.c")

set(COMPONENT_REQUIRES spi_flash)

register_component()
