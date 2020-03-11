//
// Created by life on 20-1-15.
//

#ifndef MSI001_RFSWITCH_H
#define MSI001_RFSWITCH_H
typedef enum {
    RF_SWITCH_AM_MODE,
    RF_SWITCH_VHF1_MODE,
    RF_SWITCH_VHF2_MODE,
    RF_SWITCH_B45_MODE,
    RF_SWITCH_LBAND_MODE
} rf_switch_mode_t;

typedef enum {
    RF_BAND_0_12_MHz,
    RF_BAND_12_30MHz,
    RF_BAND_30_60_MHz
} rf_switch_band_t;

void rf_switch_set_mode(rf_switch_mode_t mode);
void rf_switch_set_band(rf_switch_band_t band);
#endif //MSI001_RFSWITCH_H
