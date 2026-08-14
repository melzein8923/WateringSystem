#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

#include "../include/system_state.h"
#include "../include/system_init.h"
#include "../include/plant.h"
#include "../include/plant_update.h"
#include "../include/display.h"
#include "../include/irrigation.h"
#include "../include/config.h"
#include "../hardware/bme280.h"
#include "../hardware/ads1115.h"
#include "../hardware/pump.h"

static struct termios origTermios;
static int rawModeEnabled = 0;

static void disableRawMode(void)
{
    if (rawModeEnabled) {
        tcsetattr(STDIN_FILENO, TCSANOW, &origTermios);
        rawModeEnabled = 0;
    }
}

static void enableRawMode(void)
{
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, &origTermios) != 0) {
        return;
    }

    raw = origTermios;
    raw.c_lflag &= (unsigned)~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) {
        rawModeEnabled = 1;
    }
}

// Waits up to LOOP_DELAY_SEC seconds (same cadence as the old sleep()), but
// wakes up early if the user presses a key. Returns 1 if 'q'/'Q' was pressed.
static int waitForQuit(void)
{
    struct timeval tv;
    fd_set readfds;

    tv.tv_sec = LOOP_DELAY_SEC;
    tv.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0 && (c == 'q' || c == 'Q')) {
            return 1;
        }
    }

    return 0;
}

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

    if (!pumpInit()) {
        fprintf(stderr, "Warning: could not verify pump on GPIO %d, assuming it is connected\n", PUMP_GPIO);
    }

    int elapsedTime = 0;
    int running = 1;

    enableRawMode();
    printf("Press 'q' to quit.\n");

    while (running) {
        updateAllPlants(&system, boards);

        if (bme280_is_connected()) {
            bme280_update_environment(&system.environment);
        }

        evaluate_irrigation(&system, &elapsedTime);

        if (system.pumpActive && !pumpIsOn()) {
            pumpOn();
        } else if (!system.pumpActive && pumpIsOn()) {
            pumpOff();
        }

        display_system_status(&system);

        if (waitForQuit()) {
            running = 0;
        }
    }
    pumpOn();
    printf("Pump on");
    sleep(3);

    pumpOff();
    pumpCleanup();
    disableRawMode();
    printf("Shutting down.\n");

    return 0;
}
