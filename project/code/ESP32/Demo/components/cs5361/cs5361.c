//
// Created by life on 20-1-14.
//

#include "cs5361.h"
#include "tca9555.h"

#define CS5361_MS_PORT      TCA9555_PORT_0
#define CS5361_MS_PIN       TCA9555_PORT0_PIN6

#define CS5361_M0_PORT      TCA9555_PORT_0
#define CS5361_M0_PIN       TCA9555_PORT0_PIN2

#define CS5361_M1_PORT      TCA9555_PORT_0
#define CS5361_M1_PIN       TCA9555_PORT0_PIN1

#define CS5361_MDIV_PORT    TCA9555_PORT_0
#define CS5361_MDIV_PIN     TCA9555_PORT0_PIN5

#define CS5361_HPF_PORT     TCA9555_PORT_0
#define CS5361_HPF_PIN      TCA9555_PORT0_PIN4

#define CS5361_SA_PORT      TCA9555_PORT_0
#define CS5361_SA_PIN       TCA9555_PORT0_PIN3

#define CS5361_RST_PORT     TCA9555_PORT_0
#define CS5361_RST_PIN      TCA9555_PORT0_PIN7

void cs5361_set_mode(cs5361_mode_t mode)
{
    switch (mode) {
        case CS5361_MASTER_MODE:
            tca9555_set_io_pin(CS5361_MS_PORT, CS5361_MS_PIN, TCA9555_IO_SET);
            break;

        case CS5361_SLAVE_MODE:
            tca9555_set_io_pin(CS5361_MS_PORT, CS5361_MS_PIN, TCA9555_IO_RESET);
            break;

        default:
            break;
    }
}

void cs5361_set_speed_mode(cs5361_speed_mode_t speed)
{
    switch (speed) {
        case CS5361_SINGLE_SPEED:
            tca9555_set_io_pin(CS5361_M0_PORT, CS5361_M0_PIN, TCA9555_IO_RESET);
            tca9555_set_io_pin(CS5361_M1_PORT, CS5361_M1_PIN, TCA9555_IO_RESET);
            break;

        case CS5361_DOUBLE_SPEED:
            tca9555_set_io_pin(CS5361_M0_PORT, CS5361_M0_PIN, TCA9555_IO_SET);
            tca9555_set_io_pin(CS5361_M1_PORT, CS5361_M1_PIN, TCA9555_IO_RESET);
            break;

        case CS5361_QUAD_SPEED:
            tca9555_set_io_pin(CS5361_M0_PORT, CS5361_M0_PIN, TCA9555_IO_RESET);
            tca9555_set_io_pin(CS5361_M1_PORT, CS5361_M1_PIN, TCA9555_IO_SET);
            break;

        default:
            break;
    }
}

void cs5361_set_mdiv(cs5361_mdiv_mode_t mdiv)
{
    switch (mdiv) {
        case CS5361_MDIV_1:
            tca9555_set_io_pin(CS5361_MDIV_PORT, CS5361_MDIV_PIN, TCA9555_IO_RESET);
            break;

        case CS5361_MDIV_2:
            tca9555_set_io_pin(CS5361_MDIV_PORT, CS5361_MDIV_PIN, TCA9555_IO_SET);
            break;

        default:
            break;
    }
}

void cs5361_set_hpf(cs5361_hpf_mode_t hpf)
{
    switch (hpf) {
        case CS5361_HPF_DISABLE:
            tca9555_set_io_pin(CS5361_HPF_PORT, CS5361_HPF_PIN, TCA9555_IO_SET);
            break;

        case CS5361_HPF_ENABLE:
            tca9555_set_io_pin(CS5361_HPF_PORT, CS5361_HPF_PIN, TCA9555_IO_RESET);
            break;

        default:
            break;
    }
}

void cs5361_set_sa(cs5361_sa_mode_t sa)
{
    switch (sa) {
        case CS5361_I2S_MODE:
            tca9555_set_io_pin(CS5361_SA_PORT, CS5361_SA_PIN, TCA9555_IO_SET);
            break;

        case CS5361_LEFT_JUSTIFIED_MODE:
            tca9555_set_io_pin(CS5361_SA_PORT, CS5361_SA_PIN, TCA9555_IO_RESET);
            break;

        default:
            break;
    }
}

void cs5361_powerdown(void)
{
    tca9555_set_io_pin(CS5361_RST_PORT, CS5361_RST_PIN, TCA9555_IO_RESET);
}

void cs5361_powerup(void)
{
    tca9555_set_io_pin(CS5361_RST_PORT, CS5361_RST_PIN, TCA9555_IO_SET);
}

void cs5361_init(void)
{
    cs5361_set_mode(CS5361_MASTER_MODE);
    cs5361_set_speed_mode(CS5361_SINGLE_SPEED); //48K
    cs5361_set_mdiv(CS5361_MDIV_2);
    cs5361_set_hpf(CS5361_HPF_ENABLE);
    cs5361_set_sa(CS5361_I2S_MODE);
    cs5361_powerup();
}

void cs5361_start(void)
{

}
