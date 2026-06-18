#include "simulate_ads1115.h"

#include <stdlib.h>

#include "ads1115.h"

int simulate_ads1115_read_channel(uint8_t channel, int16_t *raw)
{
    if (raw == NULL || channel >= ADS1115_MAX_CHANNELS) {
        return -1;
    }

    *raw = (int16_t)(rand() % 30001);
    return 0;
}
