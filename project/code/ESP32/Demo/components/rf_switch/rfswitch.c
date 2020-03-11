//
// Created by life on 20-1-15.
//
#include <stdint.h>
#include "rfswitch.h"
#include "tca9555.h"

#define S1V1_PORT TCA9555_PORT_1
#define S1V1_PIN  TCA9555_PORT1_PIN4

#define S1V2_PORT TCA9555_PORT_1
#define S1V2_PIN  TCA9555_PORT1_PIN5

#define S2V1_PORT TCA9555_PORT_1
#define S2V1_PIN  TCA9555_PORT1_PIN2

#define S2V2_PORT TCA9555_PORT_1
#define S2V2_PIN  TCA9555_PORT1_PIN3

#define S3V1_PORT TCA9555_PORT_1
#define S3V1_PIN  TCA9555_PORT1_PIN1

#define S3V2_PORT TCA9555_PORT_1
#define S3V2_PIN  TCA9555_PORT1_PIN0

typedef enum {
   RF_CHANNEL_1,
   RF_CHANNEL_2,
   RF_CHANNEL_3,
   RF_CHANNEL_4
} rf_channel_t;

typedef struct {
   tca9555_io_port_t port;
   uint8_t           pin;
} rf_port_t;

const rf_port_t rf_port_table[6] = {
        {S1V1_PORT, S1V1_PIN}, {S1V2_PORT, S1V2_PIN}, {S2V1_PORT, S2V1_PIN},
        {S2V2_PORT, S2V2_PIN}, {S3V1_PORT, S3V1_PIN}, {S3V2_PORT, S3V2_PIN}};

static void rf_switch_set_port(int index, rf_channel_t channel)
{
    tca9555_io_port_t port1 = rf_port_table[index*2].port;
    tca9555_io_port_t port2 = rf_port_table[index*2+1].port;
    uint8_t pin1 = rf_port_table[index*2].pin;
    uint8_t pin2 = rf_port_table[index*2+1].pin;

    switch (channel) {
        case RF_CHANNEL_1:
            tca9555_set_io_pin(port1, pin1, TCA9555_IO_RESET);
            tca9555_set_io_pin(port2, pin2, TCA9555_IO_RESET);
            break;

        case RF_CHANNEL_2:
            tca9555_set_io_pin(port1, pin1, TCA9555_IO_SET);
            tca9555_set_io_pin(port2, pin2, TCA9555_IO_RESET);
            break;

        case RF_CHANNEL_3:
            tca9555_set_io_pin(port1, pin1, TCA9555_IO_RESET);
            tca9555_set_io_pin(port2, pin2, TCA9555_IO_SET);
            break;

        case RF_CHANNEL_4:
            tca9555_set_io_pin(port1, pin1, TCA9555_IO_SET);
            tca9555_set_io_pin(port2, pin2, TCA9555_IO_SET);
            break;

        default:
            break;
    }
}

void rf_switch_set_mode(rf_switch_mode_t mode)
{
    switch (mode) {
        case RF_SWITCH_AM_MODE:
            break;

        case RF_SWITCH_VHF1_MODE:
            rf_switch_set_port(0, RF_CHANNEL_3);
            rf_switch_set_port(1, RF_CHANNEL_3);
            rf_switch_set_port(2, RF_CHANNEL_1);
            break;

        case RF_SWITCH_VHF2_MODE:
            rf_switch_set_port(0, RF_CHANNEL_3);
            rf_switch_set_port(1, RF_CHANNEL_3);
            rf_switch_set_port(2, RF_CHANNEL_2);
            break;

        case RF_SWITCH_B45_MODE:
            rf_switch_set_port(0, RF_CHANNEL_3);
            rf_switch_set_port(1, RF_CHANNEL_3);
            rf_switch_set_port(2, RF_CHANNEL_3);
            break;

        case RF_SWITCH_LBAND_MODE:
            rf_switch_set_port(0, RF_CHANNEL_3);
            rf_switch_set_port(1, RF_CHANNEL_3);
            rf_switch_set_port(2, RF_CHANNEL_4);
            break;

        default:
            break;
    }
}

void rf_switch_set_band(rf_switch_band_t band)
{
    switch (band) {
        case RF_BAND_0_12_MHz:
            rf_switch_set_port(0, RF_CHANNEL_2);
            rf_switch_set_port(1, RF_CHANNEL_2);
            break;

        case RF_BAND_12_30MHz:
            rf_switch_set_port(0, RF_CHANNEL_4);
            rf_switch_set_port(1, RF_CHANNEL_1);
            break;

        case RF_BAND_30_60_MHz:
            rf_switch_set_port(0, RF_CHANNEL_1);
            rf_switch_set_port(1, RF_CHANNEL_4);
            break;

        default:
            break;
    }
}