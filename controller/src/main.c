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

int main(){
    srand(time(NULL));
    SystemState system;
    initialize_system(&system);

    if (bme280_connect() == 0) {
        test_raw_read();
    }

    while (1)
    {
        simulate_sensor_readings(&system);
        evaluate_irrigation(&system);
        display_system_status(&system);
        sleep(LOOP_DELAY_SEC);
    }
    return 0;
}
