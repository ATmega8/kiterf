#include <msi001.h>
#include "lsdr.h"

#include "tca9555.h"
#include "cs5361.h"

static int c_lsdr_set_led(lua_State* L);

static int c_lsdr_set_adc_power_mode(lua_State* L);
static int c_lsdr_set_adc_mode(lua_State* L);
static int c_lsdr_set_adc_speed_mode(lua_State* L);
static int c_lsdr_set_adc_mdiv(lua_State* L);
static int c_lsdr_set_adc_hpf(lua_State* L);
static int c_lsdr_set_adc_sa(lua_State* L);

static int c_lsdr_set_tuner_frequency(lua_State* L);
static int c_lsdr_set_tuner_gain(lua_State* L);
static int c_lsdr_set_tuner_register(lua_State* L);

static int c_lsdr_set_filter_mode(lua_State* L);
static int c_lsdr_set_filter_am_band(lua_State* L);

static const luaL_Reg sdr[] = {
        {"set_led", c_lsdr_set_led},
        {"set_adc_power_mode", c_lsdr_set_adc_power_mode},
        {"set_adc_mode", c_lsdr_set_adc_mode},
        {"set_adc_speed_mode", c_lsdr_set_adc_speed_mode},
        {"set_adc_mdiv", c_lsdr_set_adc_mdiv},
        {"set_adc_hpf_mode", c_lsdr_set_adc_hpf},
        {"set_adc_output_format", c_lsdr_set_adc_sa},
        {"set_tuner_frequency", c_lsdr_set_tuner_frequency},
        {"set_tuner_gain", c_lsdr_set_tuner_gain},
        {"set_tuner_register", c_lsdr_set_tuner_register},
        {"set_filter_mode", c_lsdr_set_filter_mode},
        {"set_filter_am_band", c_lsdr_set_filter_am_band},
        {NULL, NULL}
};

static int c_lsdr_set_led(lua_State* L)
{
    uint32_t status = luaL_checkinteger(L, 1);

    if(status == 0) {
        tca9555_set_io_pin(TCA9555_PORT_0, TCA9555_PORT0_PIN0, TCA9555_IO_SET);
    } else {
        tca9555_set_io_pin(TCA9555_PORT_0, TCA9555_PORT0_PIN0, TCA9555_IO_RESET);
    }

    return 0;
}

static int c_lsdr_set_adc_power_mode(lua_State* L)
{
    uint32_t  status = luaL_checkinteger(L, 1);

    if(status == 0) {
        cs5361_powerdown();
    } else {
        cs5361_powerup();
    }

    return 0;
}

static int c_lsdr_set_adc_mode(lua_State* L)
{
    return 0;
}

static int c_lsdr_set_adc_speed_mode(lua_State* L)
{
    return 0;
}

static int c_lsdr_set_adc_mdiv(lua_State* L)
{
    return 0;
}

static int c_lsdr_set_adc_hpf(lua_State* L)
{
    return 0;
}

static int c_lsdr_set_adc_sa(lua_State* L)
{
    return 0;
}

static int c_lsdr_set_tuner_frequency(lua_State* L)
{
    uint32_t freq = luaL_checkinteger(L, 1);
    msi001_set_tuner_frequency(freq);
    return 0;
}

static int c_lsdr_set_tuner_gain(lua_State* L)
{
    uint32_t gain = luaL_checkinteger(L, 1);

    if(gain > 59 ) {
       luaL_error(L, "invalid gain");
        return 0;
    }

    msi001_set_gain(gain);

    return 0;
}

static int c_lsdr_set_tuner_register(lua_State* L)
{
    uint32_t reg = luaL_checkinteger(L, 1);
    msi001_wreg(reg);
    return 0;
}

static int c_lsdr_set_filter_mode(lua_State* L)
{
    return 0;
}

static int c_lsdr_set_filter_am_band(lua_State* L)
{
    return 0;
}

int luaopen_lsdr(lua_State* L)
{
    luaL_newlib(L, sdr);
    return 1;
}
