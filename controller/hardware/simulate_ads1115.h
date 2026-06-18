#ifndef SIMULATE_ADS1115_H
#define SIMULATE_ADS1115_H

#include <stdint.h>

int simulate_ads1115_read_channel(uint8_t channel, int16_t *raw);

#endif
