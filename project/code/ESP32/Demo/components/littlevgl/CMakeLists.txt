set(COMPONENT_SRCDIRS "lvgl/lv_core" 
                      "lvgl/lv_hal" 
                      "lvgl/lv_objx" 
                      "lvgl/lv_fonts" 
                      "lvgl/lv_misc" 
                      "lvgl/lv_themes" 
                      "lvgl/lv_draw")
set(COMPONENT_ADD_INCLUDEDIRS "." 
                              "lvgl"
                              "lvgl/lv_core"
                              "lvgl/lv_hal"
                              "lvgl/lv_objx"
                              "lvgl/lv_fonts"
                              "lvgl/lv_misc"
                              "lvgl/lv_themes"
                              "lvgl/lv_draw")

register_component()

