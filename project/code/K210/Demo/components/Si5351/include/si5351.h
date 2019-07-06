//
// Created by life on 19-5-10.
//

#ifndef KITERF_SI5351_H
#define KITERF_SI5351_H
#define SI5351_CHANNEL_A
#define SI5351_CHANNEL_B
#define SI5351_CHANNEL_C

void si5351_init();
void si5351_set_channel(int channel, uint64_t freq);
#endif //KITERF_SI5351_H
