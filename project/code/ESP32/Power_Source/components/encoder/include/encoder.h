#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ENCODER_VALUE_MAX   (20)
#define ENCODER_VALUE_MIN   (-20)
#define ENCODER_PIN_A   4 
#define ENCODER_PIN_B   5
#define ENCODER_PIN_D   12 // button

bool encoder_get_button_state();

int16_t encoder_get_new_moves();

void encoder_init();

#ifdef __cplusplus
}
#endif