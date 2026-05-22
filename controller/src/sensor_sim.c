#include <stdlib.h>
#include <time.h>

#include "sensor_sim.h"

void simulate_sensor_readings(SystemState* system)
{
    for (int i = 0; i < system->active_plant_count; i++)
    {
        Plant* plant = &system->plants[i];

        plant->moisture_percent -= 0.3f;

        if (plant->moisture_percent < 0)
        {
            plant->moisture_percent = 0;
        }

        if (system->pump_active)
        {
            plant->moisture_percent += 5.0f;
        }

        if (plant->moisture_percent > 100)
        {
            plant->moisture_percent = 100;
        }
    }
    system->environment.temperature_c =
        18 + rand() % 15;

    system->environment.humidity_percent =
        30 + rand() % 41;

    system->environment.pressure_hpa =
        990 + rand() % 31;
}
