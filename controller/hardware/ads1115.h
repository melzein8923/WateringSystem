#ifndef ADS1115_H
#define ADS1115_H

#include <stdint.h>

#include "moisture_sensor.h"

#define ADS1115_I2C_BUS       "/dev/i2c-1"
#define ADS1115_ADDR           0x48
#define ADS1115_CONVERSION_REG 0x00
#define ADS1115_CONFIG_REG     0x01
#define ADS1115_MAX_CHANNELS   4

typedef struct {
	uint8_t i2cAddress;
	int connected;
	int activeSensors; /* 1 to ADS1115_MAX_CHANNELS */
	MoistureSensor sensors[ADS1115_MAX_CHANNELS];
} Ads1115;

int ads1115_init(void);
int ads1115_read_channel(uint8_t channel);
int ads1115_write_register(uint8_t reg, uint16_t value);
int ads1115_read_register(uint8_t reg, uint16_t *value);

#endif