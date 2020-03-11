//
// Created by life on 19-11-20.
//

#ifndef MSI001_MSI001_H
#define MSI001_MSI001_H

#include <stdint.h>

typedef enum {
    MSI001IFZero,
    MSI001IF450K,
    MSI001IF1620K,
    MSI001IF2048K
} msi001_if_t;

typedef enum {
    MSI001BandAM,
    MSI001BandVHF,
    MSI001BandB3,
    MSI001BandB45,
    MSI001BL
} msi001_band_t;

typedef enum {
    MSI001Filter200K,
    MSI001Filter300K,
    MSI001Filter600K,
    MSI001Filter1536K,
    MSI001Filter5M,
    MSI001Filter6M,
    MSI001Filter7M,
    MSI001Filter8M
} msi001_filter_t;

typedef struct {
   uint32_t        f_rf;
   msi001_if_t     f_if;
   msi001_band_t   band;
   msi001_filter_t filter;
   uint8_t         if_gain;
   uint8_t         mixer_gain;
   uint8_t         lna_gain;
   uint32_t        f_real_rf;
} msi001_t;

int msi001_set_tuner(msi001_t *dev);
int msi001_standby();
#endif //MSI001_MSI001_H
