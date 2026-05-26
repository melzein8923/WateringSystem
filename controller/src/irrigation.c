#include "../include/irrigation.h"
#include "../include/system_state.h"
#include "../include/config.h"
#include "../include/plant.h"

void evaluate_irrigation(SystemState* system)
{
    int anyDry = 0;

    for (int i = 0; i < system->activePlants; i++) {
        if (isPlantDry(&system->plants[i])) {
            system->plants[i].needsWater = 1;
            anyDry = 1;
        } else {
            system->plants[i].needsWater = 0;
        }
    }

    if (system->pumpSecondsRemaining > 0) {
        system->pumpActive = 1;
        system->mode = STATE_WATERING;
        system->pumpSecondsRemaining -= LOOP_DELAY_SEC;
        if (system->pumpSecondsRemaining < 0) {
            system->pumpSecondsRemaining = 0;
        }
        if (system->pumpSecondsRemaining == 0) {
            system->pumpActive = 0;
            system->cooldownSecondsRemaining = PUMP_COOLDOWN_SEC;
            system->mode = STATE_COOLDOWN;
        }
        return;
    }

    if (system->cooldownSecondsRemaining > 0) {
        system->pumpActive = 0;
        system->mode = STATE_COOLDOWN;
        system->cooldownSecondsRemaining -= LOOP_DELAY_SEC;
        if (system->cooldownSecondsRemaining < 0) {
            system->cooldownSecondsRemaining = 0;
        }
        return;
    }

    system->pumpActive = 0;
    system->mode = STATE_MONITORING;

    if (anyDry) {
        system->pumpActive = 1;
        system->pumpSecondsRemaining = PUMP_DURATION_SEC;
        system->mode = STATE_WATERING;
    }
}
