#include <stdlib.h>
#include <time.h>

#include "../include/sensor_sim.h"
#include "../include/system_state.h"

void simulate_sensor_readings(SystemState* system)
{
    for (int i = 0; i < system->activePlants; i++)
    {
        Plant* plant = &system->plants[i];

        plant->moisturePercent -= 0.3f;

        if (plant->moisturePercent < 0)
        {
            plant->moisturePercent = 0;
        }

        if (system->pumpActive)
        {
            plant->moisturePercent += 5.0f;
        }

        if (plant->moisturePercent > 100)
        {
            plant->moisturePercent = 100;
        }
    }
    system->environment.tempC =
        18 + rand() % 15;

    system->environment.humidityPercent =
        30 + rand() % 41;

    system->environment.pressurehpa =
        990 + rand() % 31;
}
