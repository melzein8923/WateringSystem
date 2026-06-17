#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../include/system_state.h"
#include "../include/system_init.h"
#include "../include/sensor_sim.h"
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

    while (1) {
        simulate_sensor_readings(&system);

        if (bme280_ok) {
            bme280_update_environment(&system.environment);
        }

        evaluate_irrigation(&system);
        display_system_status(&system);
        sleep(LOOP_DELAY_SEC);
    }

    return 0;
}
