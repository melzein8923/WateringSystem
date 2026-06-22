#include <stddef.h>

#include "../include/system_init.h"
#include "../include/config.h"
#include "../hardware/ads1115.h"

void initialize_system(SystemState* system, int numPlants)
{
    system->activePlants = numPlants;
    system->pumpActive = 0;
    system->pumpSecondsRemaining = 0;
    system->cooldownSecondsRemaining = 0;
    system->mode = STATE_MONITORING;
    for (int i = 0; i < system->activePlants; i++)
    {
        system->plants[i].id = i + 1;
        system->plants[i].moistureSensor.adsIndex = i / ADS1115_MAX_CHANNELS;
        system->plants[i].moistureSensor.channel = (uint8_t)(i % ADS1115_MAX_CHANNELS);
        system->plants[i].moistureSensor.dryValue = 28000;
        system->plants[i].moistureSensor.wetValue = 12000;
        system->plants[i].moistureSensor.rawValue = 0;
        system->plants[i].moistureSensor.sensorFault = 0;
        system->plants[i].moisturePercent = 50.0f;
        system->plants[i].needsWater = 0;
        system->plants[i].sensorFault = 0;
        system->plants[i].lastReading = 50.0f;
        system->plants[i].stuckStreak = 0;
        system->plants[i].goodStreak = 0;
    }
}

int initialize_ads1115_boards(Ads1115* boards, int numPlants)
{
    int boardsNeeded = (numPlants + ADS1115_MAX_CHANNELS - 1) / ADS1115_MAX_CHANNELS;

    if (boards == NULL || numPlants < 1 || boardsNeeded > MAX_ADS1115_BOARDS) {
        return -1;
    }

    for (int b = 0; b < boardsNeeded; b++) {
        int remaining = numPlants - b * ADS1115_MAX_CHANNELS;

        boards[b].i2cAddress = ADS1115_ADDR + (uint8_t)b;
        boards[b].fd = -1;
        boards[b].connected = 0;
        boards[b].activeSensors = remaining < ADS1115_MAX_CHANNELS
            ? remaining
            : ADS1115_MAX_CHANNELS;

        if (ads1115_init(&boards[b]) != 0) {
            return -1;
        }
    }

    return boardsNeeded;
}
