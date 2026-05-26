#include <stdlib.h>
#include <time.h>

#include "../include/sensor_sim.h"
#include "../include/system_state.h"
#include "../include/plant.h"

void simulate_sensor_readings(SystemState* system)
{
    for (int i = 0; i < system->activePlants; i++)
    {
        Plant* plant = &system->plants[i];
        float reading = plant->moisturePercent - 1.4f;

        if (system->pumpActive) {
            reading += 5.0f;
        }

        plantUpdateMoisture(plant, reading);
    }
    system->environment.tempC = 18 + rand() % 15;
    system->environment.humidityPercent = 30 + rand() % 41;
    system->environment.pressurehpa = 990 + rand() % 31;
}
