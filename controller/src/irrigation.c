#include "../include/irrigation.h"
#include "../include/system_state.h"
#include "../include/config.h"
#include "../include/plant.h"

// One pump serves every plant, so the total time it needs to run for a full
// watering session scales with plant count (mx+b), then gets split evenly
// across SOAK_COUNT pulses.
static int pumpPulseDurationMs(const SystemState* system)
{
    int totalMs = PUMP_DURATION_MS_PER_PLANT * system->activePlants + PUMP_DURATION_MS_BASE;
    return totalMs / SOAK_COUNT;
}

void evaluate_irrigation(SystemState* system, int* elapsedTime)
{
    int anyDry = 0;
    int allFaulted = system->activePlants > 0;

    for (int i = 0; i < system->activePlants; i++) {
        Plant* plant = &system->plants[i];

        if (isPlantDry(plant)) {
            plant->needsWater = 1;
            anyDry = 1;
        } else {
            plant->needsWater = 0;
        }

        if (!plant->sensorFault) {
            allFaulted = 0;
        }
    }

    *elapsedTime += LOOP_DELAY_SEC * 1000;

    // Dispatch is driven purely by pumpActive/soaksRemaining/cooldownActive
    // (never by `mode`), so the STATE_FAULT display override below can never
    // corrupt the actual state machine.
    if (system->pumpActive) {
        system->mode = allFaulted ? STATE_FAULT : STATE_WATERING;
        if (*elapsedTime >= pumpPulseDurationMs(system)) {
            *elapsedTime = 0;
            system->pumpActive = 0;
            system->soaksRemaining--;
            if (system->soaksRemaining > 0) {
                system->mode = allFaulted ? STATE_FAULT : STATE_SOAKING;
            } else {
                system->cooldownActive = 1;
                system->mode = allFaulted ? STATE_FAULT : STATE_COOLDOWN;
            }
        }
        return;
    }

    if (system->soaksRemaining > 0) {
        system->mode = allFaulted ? STATE_FAULT : STATE_SOAKING;
        if (*elapsedTime >= SOAK_DURATION_SEC * 1000) {
            *elapsedTime = 0;
            system->pumpActive = 1;
            system->mode = allFaulted ? STATE_FAULT : STATE_WATERING;
        }
        return;
    }

    if (system->cooldownActive) {
        system->mode = allFaulted ? STATE_FAULT : STATE_COOLDOWN;
        if (*elapsedTime >= PUMP_COOLDOWN_SEC * 1000) {
            *elapsedTime = 0;
            system->cooldownActive = 0;
            system->mode = allFaulted ? STATE_FAULT : STATE_MONITORING;
        }
        return;
    }

    system->mode = allFaulted ? STATE_FAULT : STATE_MONITORING;

    if (anyDry) {
        *elapsedTime = 0;
        system->soaksRemaining = SOAK_COUNT;
        system->pumpActive = 1;
        system->mode = STATE_WATERING;
    }
}
