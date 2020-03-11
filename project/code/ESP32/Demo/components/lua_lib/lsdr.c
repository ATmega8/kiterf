#include "lsdr.h"

static int c_lsdr_set_led(lua_State* L);

static int c_lsdr_set_adc_power_mode(lua_State* L);
static int c_lsdr_set_adc_mode(lua_State* L);
static int c_lsdr_set_adc_speed_mode(lua_State* L);
static int c_lsdr_set_adc_mdiv(lua_State* L);
static int c_lsdr_set_adc_hpf(lua_State* L);
static int c_lsdr_set_adc_sa(lua_State* L);

static int c_lsdr_set_tuner_frequency(lua_State* L);
static int c_lsdr_set_tuner_gain(lua_State* L);

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
        {"set_filter_mode", c_lsdr_set_filter_mode},
        {"set_filter_am_band", c_lsdr_set_filter_am_band},
        {NULL, NULL}
};

static int c_lsdr_set_led(lua_State* L)
{
    return 0;
}

static int c_lsdr_set_adc_power_mode(lua_State* L)
{
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
    return 0;
}

static int c_lsdr_set_tuner_gain(lua_State* L)
{
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
