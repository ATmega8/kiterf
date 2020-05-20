//
// Created by life on 20-3-22.
//

#ifndef KITERF_IQ_RECEIVE_H
#define KITERF_IQ_RECEIVE_H

#include <stdint.h>

void iq_receive_spi_init(void);
void iq_receive_run(uint8_t* pdata, size_t len);

#endif //KITERF_IQ_RECEIVE_H
