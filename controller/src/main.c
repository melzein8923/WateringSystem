#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../include/system_state.h"
#include "../include/system_init.h"
#include "../include/plant.h"
#include "../include/plant_update.h"
#include "../include/display.h"
#include "../include/irrigation.h"
#include "../include/config.h"
#include "../hardware/bme280.h"
#include "../hardware/ads1115.h"

int main(int argc, char *argv[])
{
    int numPlants = 4;

    if (argc >= 2) {
        numPlants = atoi(argv[1]);
    }

    if (numPlants < 1 || numPlants > MAX_PLANTS) {
        fprintf(stderr, "Usage: %s [num_plants (1-%d)]\n", argv[0], MAX_PLANTS);
        return 1;
    }

    srand(time(NULL));
    SystemState system;
    initialize_system(&system, numPlants);

    bme280_init();

    Ads1115 boards[MAX_ADS1115_BOARDS];
    if (initialize_ads1115_boards(boards, numPlants) < 0) {
        fprintf(stderr, "Failed to initialize ADS1115 boards for %d plants\n", numPlants);
        return 1;
    }

    while (1) {
        updateAllPlants(&system, boards);

        if (bme280_is_connected()) {
            bme280_update_environment(&system.environment);
        }

        evaluate_irrigation(&system);
        display_system_status(&system);
        sleep(LOOP_DELAY_SEC);
    }

    return 0;
}
