#include <stdio.h>
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

    Ads1115 ads = {
        .i2cAddress = ADS1115_ADDR,
        .fd = -1,
        .connected = 0,
        .activeSensors = 1,
    };

    int ads_ok = (ads1115_init(&ads) == 0);
    if (!ads_ok) {
        fprintf(stderr, "ADS1115 init failed\n");
        return 1;
    }

    while (1) {
        int16_t raw = 0;

        if (ads1115_read_channel(&ads, 0, &raw) == 0) {
            printf("A0 responding, raw=%d\n", raw);
        } else {
            printf("A0 read failed\n");
        }

        sleep(1);
    }

    /*
    while (1) {
        simulate_sensor_readings(&system);

        if (bme280_ok) {
            bme280_update_environment(&system.environment);
        }

        evaluate_irrigation(&system);
        display_system_status(&system);
        sleep(LOOP_DELAY_SEC);
    }
    */

    (void)bme280_ok;
    (void)system;
    return 0;
}
