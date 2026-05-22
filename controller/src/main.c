#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "systemState.h"
#include "system_init.h"
#include "sensor_sim.h"
#include "display.h"
#include "config.h"

int main(){
    srand(time(NULL));
    SystemState system;
    initialize_system(&system);
    while (1)
    {
        simulate_sensor_readings(&system);
        display_system_status(&system);
        sleep(LOOP_DELAY_SEC);
    }
    return 0;
}
