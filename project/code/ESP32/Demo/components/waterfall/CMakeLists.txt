set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS "include")
set(COMPONENT_SRCS "waterfall_image.c waterfall.c")

set(COMPONENT_REQUIRES littlevgl esp-dsp)

register_component()
