#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SWAP(a, b) do {complex t = (a); (a) = (b); (b) = t;} while(0)

typedef struct {
    double real, imag;
} complex;

void fft_soft(complex *data, int n);
void ifft_soft(complex *data, int n);
void show(complex *data, int n);

#ifdef __cplusplus
}
#endif