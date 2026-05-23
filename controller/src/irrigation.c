#include "irrigation.h"
#include "config.h"

void evaluate_irrigation(SystemState* system)
{
    int needs_watering = 0;

    for (int i = 0; i < system->activePlants; i++) {
        Plant* plant = &system->plants[i];

        if (plant->moisturePercent < WATERING_THRESHOLD) {
            plant->needs_water = 1;
            needs_watering = 1;
        }
        else{
            plant->needs_water = 0;
        }
    }
    if (needs_watering) {
        system->pump_active = 1;
        system->mode = STATE_WATERING;
    }
    else {
        system->pump_active = 0;
        system->mode = STATE_MONITORING;
    }
}
