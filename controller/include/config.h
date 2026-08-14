#ifndef CONFIG_H
#define CONFIG_H

#define WATERING_THRESHOLD 35.0f

//(watering time) = (PUMP_DURATION_MS_PER_PLANT)numPlants + (PUMP_DURATION_MS_BASE)
#define PUMP_DURATION_MS_PER_PLANT 100
#define PUMP_DURATION_MS_BASE 4000


#define SOAK_COUNT 3
#define SOAK_DURATION_SEC 45

#define PUMP_COOLDOWN_SEC 60
#define LOOP_DELAY_SEC 2
#define SENSOR_MAX_STUCK_COUNT 5   // Number of consecutive stuck readings before declaring a fault
#define SENSOR_FAULT_CLEAR_COUNT 3 // Number of valid readings required to clear a fault

// ADS1115 settings
#define MAX_ADS1115_BOARDS 4
#define MAX_PLANTS 16

// GPIO pins for pump
#define GPIO_CHIP "/dev/gpiochip0"
#define PUMP_GPIO 17

#endif
