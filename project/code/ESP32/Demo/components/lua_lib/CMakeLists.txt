set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS "include")
set(COMPONENT_SRCS "lsdr.c")

set(COMPONENT_REQUIRES esp_lua tca9555 cs5361 msi001 rf_switch)

register_component()
