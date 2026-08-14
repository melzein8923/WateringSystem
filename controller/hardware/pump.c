#include "pump.h"
#include "../include/config.h"

#include <gpiod.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

static struct gpiod_chip *chip = NULL;
static struct gpiod_line *line = NULL;
static bool initialized = false;
static bool running = false;


bool pumpInit(void)
{
    if (initialized) return true;

    // Open the GPIO chip
    chip = gpiod_chip_open(GPIO_CHIP);
    if (chip == NULL) {
        fprintf(stderr, "Failed to open GPIO chip\n");
        perror("Failed to open GPIO chip");
        return false;
    }
    // Get the GPIO line
    line = gpiod_chip_get_line(chip, PUMP_GPIO);
    if (line == NULL) {
        fprintf(stderr, "Failed to get GPIO line\n");
        gpiod_chip_close(chip);
        chip = NULL;
        return false;
    }

    // Set the GPIO line as output and set it to low
    if(gpiod_line_request_output(line, "pump", 0) < 0) {
        perror("Failed to request GPIO output");
        gpiod_chip_close(chip);
        line = NULL;
        chip = NULL;
        return false;
    }
    initialized = true;
    running = false;

    return true;
}

void pumpOn(void)
{
    if(!initialized){
        return;
    }
    if (gpiod_line_set_value(line, 1) < 0) {
        perror("Failed to turn pump on");
        return;
    }
    running = true;
}

void pumpOff(void)
{
    if(!initialized){
        return;
    }
    if (gpiod_line_set_value(line, 0) < 0) {
        perror("Failed to turn pump off");
        return;
    }
    running = false;
}


bool pumpIsOn(void)
{
    if(running) {
        return true;
    }
    return false;
}

void pumpCleanup(void)
{
    if(!initialized){
        return;
    }
    gpiod_line_set_value(line, 0);
    gpiod_line_release(line);
    gpiod_chip_close(chip);
    chip = NULL;
    line = NULL;
    initialized = false;
    running = false;
}