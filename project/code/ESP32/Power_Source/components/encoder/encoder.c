
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_heap_caps.h"
#include "driver/pcnt.h"
#include "driver/gpio.h"
#include "encoder.h"

#define ENCODER_PCNT_UNIT      PCNT_UNIT_0

QueueHandle_t pcnt_evt_queue = NULL;   // A queue to handle pulse counter events
pcnt_isr_handle_t user_isr_handle = NULL; //user's ISR service handle

typedef struct {
    int unit;  // the PCNT unit that originated an interrupt
    uint32_t status; // information on the event type that caused the interrupt
} pcnt_evt_t;

static void IRAM_ATTR pcnt_intr_handler(void *arg)
{
    uint32_t intr_status = PCNT.int_st.val;
    int i;
    pcnt_evt_t evt;
    portBASE_TYPE HPTaskAwoken = pdFALSE;

    for (i = 0; i < PCNT_UNIT_MAX; i++) {
        if (intr_status & (BIT(i))) {
            evt.unit = i;
            /* Save the PCNT event type that caused an interrupt
               to pass it to the main program */
            evt.status = PCNT.status_unit[i].val;
            PCNT.int_clr.val = BIT(i);
            xQueueSendFromISR(pcnt_evt_queue, &evt, &HPTaskAwoken);
            if (HPTaskAwoken == pdTRUE) {
                portYIELD_FROM_ISR();
            }
        }
    }
}

bool encoder_get_button_state()
{
    return !gpio_get_level((gpio_num_t)ENCODER_PIN_D);
}
int16_t encoder_get_new_moves()
{
    static int16_t last_count = 0;
    int16_t count = 0;
    int16_t ret = 0;
    pcnt_evt_t evt;
    portBASE_TYPE res;

    pcnt_get_counter_value(ENCODER_PCNT_UNIT, &count);
    ret = count/4 - last_count/4;
    if (abs(ret) > 0) {
        last_count = count;
    }
    while (1) {
        res = xQueueReceive(pcnt_evt_queue, &evt, 0);
        if (res == pdTRUE) {
            if (evt.status & PCNT_STATUS_L_LIM_M) {
                ret += ENCODER_VALUE_MIN;
            }
            if (evt.status & PCNT_STATUS_H_LIM_M) {
                ret += ENCODER_VALUE_MAX;
            }
        } else {
            break;
        }
    }

    return ret;
}

void encoder_init()
{
    /* Initialize PCNT event queue and PCNT functions */
    pcnt_evt_queue = xQueueCreate(10, sizeof(pcnt_evt_t));

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1 << ENCODER_PIN_D;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    /* Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_config = {0};

    pcnt_config.pulse_gpio_num  = ENCODER_PIN_A;
    pcnt_config.ctrl_gpio_num   = ENCODER_PIN_B;
    pcnt_config.channel         = PCNT_CHANNEL_0;
    pcnt_config.unit            = ENCODER_PCNT_UNIT;
    pcnt_config.pos_mode        = PCNT_COUNT_INC; 
    pcnt_config.neg_mode        = PCNT_COUNT_DEC;
    pcnt_config.lctrl_mode      = PCNT_MODE_REVERSE;
    pcnt_config.hctrl_mode      = PCNT_MODE_KEEP;
    pcnt_config.counter_h_lim   = ENCODER_VALUE_MAX * 4;
    pcnt_config.counter_l_lim   = ENCODER_VALUE_MIN * 4;
    pcnt_unit_config(&pcnt_config);

    pcnt_config.pulse_gpio_num  = ENCODER_PIN_B;
    pcnt_config.ctrl_gpio_num   = ENCODER_PIN_A;
    pcnt_config.channel         = PCNT_CHANNEL_1;
    pcnt_config.pos_mode        = PCNT_COUNT_DEC;
    pcnt_config.neg_mode        = PCNT_COUNT_INC;
    pcnt_unit_config(&pcnt_config);

    /* Configure and enable the input filter */
    pcnt_set_filter_value(ENCODER_PCNT_UNIT, 100);
    pcnt_filter_enable(ENCODER_PCNT_UNIT);

    pcnt_event_enable(ENCODER_PCNT_UNIT, PCNT_EVT_H_LIM);
    pcnt_event_enable(ENCODER_PCNT_UNIT, PCNT_EVT_L_LIM);

    /* Initialize PCNT's counter */
    pcnt_counter_pause(ENCODER_PCNT_UNIT);
    pcnt_counter_clear(ENCODER_PCNT_UNIT);

    /* Register ISR handler and enable interrupts for PCNT unit */
    pcnt_isr_register(pcnt_intr_handler, NULL, 0, &user_isr_handle);
    pcnt_intr_enable(ENCODER_PCNT_UNIT);

    /* Everything is set up, now go to counting */
    pcnt_counter_resume(ENCODER_PCNT_UNIT);
}