//
// Created by life on 20-3-4.
//

#ifndef LVGL_WATERFALL_H
#define LVGL_WATERFALL_H
void waterfall_do_fft(const uint8_t* iq_data);
void lvgl_update_waterfall(lv_obj_t* waterfall_image);
#endif //LVGL_WATERFALL_H
