#include <stdio.h>

#include "../include/display.h"
#include "../include/system_state.h"

void display_system_status(SystemState* system)
{
    printf("\n====================\n");

    const char* modeLabel = "MONITORING";
    if (system->mode == STATE_WATERING) {
        modeLabel = "WATERING";
    } else if (system->mode == STATE_COOLDOWN) {
        modeLabel = "COOLDOWN";
    } else if (system->mode == STATE_FAULT) {
        modeLabel = "FAULT";
    } else if (system->mode == STATE_INIT) {
        modeLabel = "INIT";
    }
    printf("System Mode: %s\n", modeLabel);

    printf("Pump: %s\n",
           system->pumpActive ? "ON" : "OFF");

    printf("\nPlants:\n");

    for (int i = 0; i < system->activePlants; i++)
    {
        Plant* plant = &system->plants[i];

        if (plant->sensorFault) {
            printf("Plant %d: SENSOR FAULT\n", plant->id);
        } else {
            printf(
                "Plant %d: %.1f%% moisture%s\n",
                plant->id,
                plant->moisturePercent,
                plant->needsWater ? " (dry)" : ""
            );
        }
    }

    printf("\nEnvironment:\n");

    printf("Temperature: %.1f C\n",
           system->environment.tempC);

    printf("Humidity: %.1f%%\n",
           system->environment.humidityPercent);

    printf("====================\n");
}
