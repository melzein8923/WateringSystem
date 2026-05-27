#ifndef BME280_H
#define BME280_H

#include <stdint.h>

#define BME280_I2C_BUS "/dev/i2c-1"
#define BME280_I2C_ADDRESS 0x76
#define BME280_REG_CHIP_ID 0xD0
#define BME280_CHIP_ID_VALUE 0x60

#define BME280_CTRL_HUM_REG   0xF2
#define BME280_CTRL_MEAS_REG  0xF4
#define BME280_CONFIG_REG     0xF5

#define BME280_TEMP_MSB  0xFA
#define BME280_PRESS_MSB 0xF7
#define BME280_HUM_MSB   0xFD

/* Open I2C bus, select BME280 slave, verify chip ID. Returns 0 on success. */
int bme280_connect(void);

void bme280_disconnect(void);

int bme280_is_connected(void);

int bme280_write_register(unsigned char reg, unsigned char value);

/*
 * Single-byte register read over I2C. Requires an active connection.
 * Returns 0 on success, -1 on error.
 */
int bme280_read_register(uint8_t reg, uint8_t *value);

int bme280_read_bytes(unsigned char reg, unsigned char *buf, int len);

void test_raw_read(void);

#endif
