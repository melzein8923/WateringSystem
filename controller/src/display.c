#include <stdio.h>

#include "../include/display.h"
#include "../include/system_state.h"

void display_system_status(SystemState* system)
{
    printf("\n====================\n");

    printf("System Mode: %d\n", system->mode);

    printf("Pump: %s\n",
           system->pumpActive ? "ON" : "OFF");

    printf("\nPlants:\n");

    for (int i = 0; i < system->activePlants; i++)
    {
        Plant* plant = &system->plants[i];

        printf(
            "Plant %d: %.1f%% moisture\n",
            plant->id,
            plant->moisturePercent
        );
    }

    printf("\nEnvironment:\n");

    printf("Temperature: %.1f C\n",
           system->environment.tempC);

    printf("Humidity: %.1f%%\n",
           system->environment.humidityPercent);

    printf("====================\n");
}
