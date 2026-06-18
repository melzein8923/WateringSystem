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

int main(void)
{
    srand(time(NULL));
    SystemState system;
    initialize_system(&system);

    int bme280_ok = (bme280_init() == 0);

    Ads1115 boards[MAX_ADS1115_BOARDS] = {
        {
            .i2cAddress = ADS1115_ADDR,
            .fd = -1,
            .connected = 0,
            .activeSensors = 4,
        },
    };

    ads1115_init(&boards[0]);

    while (1) {
        updateAllPlants(&system, boards);

        if (bme280_ok) {
            bme280_update_environment(&system.environment);
        }

        evaluate_irrigation(&system);
        display_system_status(&system);
        sleep(LOOP_DELAY_SEC);
    }

    return 0;
}
