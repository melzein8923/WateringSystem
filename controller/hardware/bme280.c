#include "bme280.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../include/config.h"

#if defined(__linux__)

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

typedef struct {
    int fd;
    int connected;
    uint8_t i2c_addr;
} Bme280Connection;

static Bme280Connection bme = {
    .fd = -1,
    .connected = 0,
    .i2c_addr = 0,
};

static int select_slave(uint8_t addr)
{
    if (ioctl(bme.fd, I2C_SLAVE, addr) < 0) {
        fprintf(stderr, "bme280: I2C_SLAVE 0x%02X failed: %s\n", addr, strerror(errno));
        return -1;
    }
    bme.i2c_addr = addr;
    return 0;
}

static int bus_read_register(uint8_t reg, uint8_t *value)
{
    if (bme.fd < 0 || value == NULL) {
        return -1;
    }

    if (write(bme.fd, &reg, 1) != 1) {
        fprintf(stderr, "bme280: write reg 0x%02X failed: %s\n", reg, strerror(errno));
        return -1;
    }

    if (read(bme.fd, value, 1) != 1) {
        fprintf(stderr, "bme280: read reg 0x%02X failed: %s\n", reg, strerror(errno));
        return -1;
    }

    return 0;
}

static int read_chip_id(uint8_t *chip_id)
{
    return bus_read_register(BME280_REG_CHIP_ID, chip_id);
}

static int try_connect_at_address(uint8_t addr)
{
    if (select_slave(addr) != 0) {
        return -1;
    }

    uint8_t chip_id = 0;
    if (read_chip_id(&chip_id) != 0) {
        return -1;
    }

    if (chip_id != BME280_CHIP_ID_VALUE) {
        fprintf(
            stderr,
            "bme280: device at 0x%02X returned chip ID 0x%02X (expected 0x%02X)\n",
            addr,
            chip_id,
            BME280_CHIP_ID_VALUE
        );
        return -1;
    }

    printf("bme280: connected at I2C 0x%02X (chip ID 0x%02X)\n", addr, chip_id);
    return 0;
}

int bme280_connect(void)
{
    if (bme.connected) {
        return 0;
    }

    bme.fd = open(BME280_I2C_BUS, O_RDWR);
    if (bme.fd < 0) {
        fprintf(
            stderr,
            "bme280: failed to open %s: %s\n",
            BME280_I2C_BUS,
            strerror(errno)
        );
        return -1;
    }

    if (try_connect_at_address(BME280_I2C_ADDRESS) == 0) {
        bme.connected = 1;
        return 0;
    }

    fprintf(
        stderr,
        "bme280: no BME280 found at 0x%02X on %s\n",
        BME280_I2C_ADDRESS,
        BME280_I2C_BUS
    );
    close(bme.fd);
    bme.fd = -1;
    bme.i2c_addr = 0;
    return -1;
}

void bme280_disconnect(void)
{
    if (bme.fd >= 0) {
        close(bme.fd);
    }
    bme.fd = -1;
    bme.connected = 0;
    bme.i2c_addr = 0;
}

int bme280_is_connected(void)
{
    return bme.connected;
}

int bme280_read_register(uint8_t reg, uint8_t *value)
{
    if (!bme.connected) {
        return -1;
    }
    return bus_read_register(reg, value);
}

int bme280_read_bytes(unsigned char reg, unsigned char *buf, int len)
{
    if (!bme.connected || bme.fd < 0 || buf == NULL || len <= 0) {
        return -1;
    }

    if (write(bme.fd, &reg, 1) != 1) {
        fprintf(stderr, "bme280: write reg 0x%02X failed: %s\n", reg, strerror(errno));
        return -1;
    }

    if (read(bme.fd, buf, (size_t)len) != len) {
        fprintf(stderr, "bme280: read %d bytes from 0x%02X failed: %s\n", len, reg, strerror(errno));
        return -1;
    }

    return 0;
}

void test_raw_read(void)
{
    unsigned char data[8];

    if (bme280_read_bytes(0xF7, data, 8) != 0) {
        return;
    }

    int raw_press =
        (data[0] << 12) |
        (data[1] << 4)  |
        (data[2] >> 4);

    int raw_temp =
        (data[3] << 12) |
        (data[4] << 4)  |
        (data[5] >> 4);

    int raw_hum =
        (data[6] << 8) |
        data[7];

    printf("Raw Temp: %d\n", raw_temp);
    printf("Raw Pressure: %d\n", raw_press);
    printf("Raw Humidity: %d\n", raw_hum);
}

#else /* !__linux__ */

int bme280_connect(void)
{
    fprintf(stderr, "bme280: I2C is only supported on Linux (e.g. Raspberry Pi)\n");
    return -1;
}

void bme280_disconnect(void)
{
}

int bme280_is_connected(void)
{
    return 0;
}

int bme280_read_register(uint8_t reg, uint8_t *value)
{
    (void)reg;
    (void)value;
    return -1;
}

int bme280_read_bytes(unsigned char reg, unsigned char *buf, int len)
{
    (void)reg;
    (void)buf;
    (void)len;
    return -1;
}

void test_raw_read(void)
{
}

#endif
