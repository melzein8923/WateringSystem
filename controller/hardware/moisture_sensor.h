#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H

#include <stdint.h>

/*
 * One capacitive moisture probe on an ADS1115 channel.
 * adsIndex: which ADS1115 board (0, 1, ...)
 * channel:  AIN0-AIN3 on that board (0-3)
 */
typedef struct {
	int adsIndex;
	uint8_t channel;

	int dryValue;
    int wetValue;

	int rawValue;
	float moisturePercent;

    int sensorFault;
} MoistureSensor;

#endif
