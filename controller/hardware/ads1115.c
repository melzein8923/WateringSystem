#include "ads1115.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#if defined(__linux__)

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

static int ads_fd = -1;

int ads1115_init(void)
{
    ads_fd = open(ADS1115_I2C_BUS, O_RDWR);
    if (ads_fd < 0) {
        perror("ads1115: open failed");
        return -1;
    }
    if (ioctl(ads_fd, I2C_SLAVE, ADS1115_ADDR) < 0) {
        perror("ads1115: connection failed");
        close(ads_fd);
        ads_fd = -1;
        return -1;
    }
    printf("ads1115: connected at I2C 0x%02X\n", ADS1115_ADDR);
    return 0;
}

int ads1115_write_register(uint8_t reg, uint16_t value)
{
    uint8_t buffer[3];

    if (ads_fd < 0) {
        return -1;
    }

    buffer[0] = reg;
    buffer[1] = (value >> 8) & 0xFF;
    buffer[2] = value & 0xFF;
    if (write(ads_fd, buffer, 3) != 3) {
        perror("ads1115: write failed");
        return -1;
    }
    return 0;
}

int ads1115_read_register(uint8_t reg, uint16_t *value)
{
    uint8_t buffer[2];

    if (ads_fd < 0 || value == NULL) {
        return -1;
    }

    if (write(ads_fd, &reg, 1) != 1) {
        perror("ads1115: read register write failed");
        return -1;
    }

    if (read(ads_fd, buffer, 2) != 2) {
        perror("ads1115: read register failed");
        return -1;
    }

    *value = ((uint16_t)buffer[0] << 8) | buffer[1];
    return 0;
}

int ads1115_read_channel(uint8_t channel)
{
    (void)channel;
    return -1;
}

#else /* !__linux__ */

int ads1115_init(void)
{
    fprintf(stderr, "ads1115: I2C is only supported on Linux (e.g. Raspberry Pi)\n");
    return -1;
}

int ads1115_read_channel(uint8_t channel)
{
    (void)channel;
    return -1;
}

int ads1115_write_register(uint8_t reg, uint16_t value)
{
    (void)reg;
    (void)value;
    return -1;
}

int ads1115_read_register(uint8_t reg, uint16_t *value)
{
    (void)reg;
    if (value != NULL) {
        *value = 0;
    }
    return -1;
}

#endif
