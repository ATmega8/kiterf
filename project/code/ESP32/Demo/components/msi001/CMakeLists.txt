set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS "include")
set(COMPONENT_SRCS "msi001.c")

set(COMPONENT_REQUIRES tca9555 rf_switch)

register_component()
