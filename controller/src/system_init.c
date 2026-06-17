#include "../include/system_init.h"

void initialize_system(SystemState* system){
	system->activePlants = 4;
    system->pumpActive = 0;
    system->pumpSecondsRemaining = 0;
    system->cooldownSecondsRemaining = 0;
    system->mode = STATE_MONITORING;
    for (int i = 0; i < system->activePlants; i++)
    {
        system->plants[i].id = i + 1;
        system->plants[i].moistureSensor.adsIndex = 0;
        system->plants[i].moistureSensor.channel = (uint8_t)i;
        system->plants[i].moisturePercent = 50.0f;
        system->plants[i].needsWater = 0;
        system->plants[i].sensorFault = 0;
        system->plants[i].lastReading = 50.0f;
        system->plants[i].stuckStreak = 0;
        system->plants[i].goodStreak = 0;
    }
}
