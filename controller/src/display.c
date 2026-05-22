#include <stdio.h>

#include "display.h"

void display_system_status(SystemState* system)
{
    printf("\n====================\n");

    printf("System Mode: %d\n", system->mode);

    printf("Pump: %s\n",
           system->pump_active ? "ON" : "OFF");

    printf("\nPlants:\n");

    for (int i = 0; i < system->active_plant_count; i++)
    {
        Plant* plant = &system->plants[i];

        printf(
            "Plant %d: %.1f%% moisture\n",
            plant->id,
            plant->moisture_percent
        );
    }

    printf("\nEnvironment:\n");

    printf("Temperature: %.1f C\n",
           system->environment.temperature_c);

    printf("Humidity: %.1f%%\n",
           system->environment.humidity_percent);

    printf("====================\n");
}
