#include "ads1115.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#if defined(__linux__)

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

static uint16_t channel_mux(uint8_t channel)
{
    static const uint16_t mux[] = { 0x4000, 0x5000, 0x6000, 0x7000 };
    return mux[channel];
}

int ads1115_read_register(Ads1115 *board, uint8_t reg, uint16_t *value)
{
    uint8_t buffer[2];

    if (board == NULL || board->fd < 0 || value == NULL) {
        return -1;
    }

    if (write(board->fd, &reg, 1) != 1) {
        fprintf(stderr, "ads1115: pointer write 0x%02X failed: %s\n", reg, strerror(errno));
        return -1;
    }

    if (read(board->fd, buffer, 2) != 2) {
        fprintf(stderr, "ads1115: read reg 0x%02X failed: %s\n", reg, strerror(errno));
        return -1;
    }

    *value = ((uint16_t)buffer[0] << 8) | buffer[1];
    return 0;
}

int ads1115_init(Ads1115 *board)
{
    uint16_t config = 0;
    uint8_t addr;

    if (board == NULL) {
        return -1;
    }

    board->connected = 0;
    board->fd = -1;

    if (board->i2cAddress == 0) {
        board->i2cAddress = ADS1115_ADDR;
    }
    addr = board->i2cAddress;

    board->fd = open(ADS1115_I2C_BUS, O_RDWR);
    if (board->fd < 0) {
        fprintf(stderr, "ads1115: failed to open %s: %s\n", ADS1115_I2C_BUS, strerror(errno));
        return -1;
    }

    if (ioctl(board->fd, I2C_SLAVE, addr) < 0) {
        fprintf(stderr, "ads1115: I2C_SLAVE 0x%02X failed: %s\n", addr, strerror(errno));
        close(board->fd);
        board->fd = -1;
        return -1;
    }

    if (ads1115_read_register(board, ADS1115_CONFIG_REG, &config) != 0) {
        fprintf(
            stderr,
            "ads1115: no response at 0x%02X on %s (is it wired and powered?)\n",
            addr,
            ADS1115_I2C_BUS
        );
        close(board->fd);
        board->fd = -1;
        return -1;
    }

    board->connected = 1;
    printf("ads1115: connected at I2C 0x%02X (config 0x%04X)\n", addr, config);
    return 0;
}

int ads1115_write_register(Ads1115 *board, uint8_t reg, uint16_t value)
{
    uint8_t buffer[3];

    if (board == NULL || !board->connected || board->fd < 0) {
        return -1;
    }

    buffer[0] = reg;
    buffer[1] = (value >> 8) & 0xFF;
    buffer[2] = value & 0xFF;
    if (write(board->fd, buffer, 3) != 3) {
        fprintf(stderr, "ads1115: write reg 0x%02X failed: %s\n", reg, strerror(errno));
        return -1;
    }
    return 0;
}

int ads1115_read_channel(Ads1115 *board, uint8_t channel, int16_t *raw)
{
    if (board == NULL || raw == NULL || channel >= ADS1115_MAX_CHANNELS) {
        return -1;
    }

    if (!board->connected) {
        return -1;
    }

    config = 0x8000 | channel_mux(channel) | 0x0200 | 0x0100 | 0x0080;
    if (ads1115_write_register(board, ADS1115_CONFIG_REG, config) != 0) {
        return -1;
    }

    for (int i = 0; i < 100; i++) {
        if (ads1115_read_register(board, ADS1115_CONFIG_REG, &config) != 0) {
            return -1;
        }
        if (config & 0x8000) {
            break;
        }
        usleep(1000);
    }

    if ((config & 0x8000) == 0) {
        fprintf(stderr, "ads1115: conversion timed out on channel %u\n", channel);
        return -1;
    }

    if (ads1115_read_register(board, ADS1115_CONVERSION_REG, &reading) != 0) {
        return -1;
    }

    *raw = (int16_t)reading;
    return 0;
}

#else /* !__linux__ */

int ads1115_init(Ads1115 *board)
{
    if (board == NULL) {
        return -1;
    }

    board->connected = 0;
    board->fd = -1;
    fprintf(stderr, "ads1115: I2C is only supported on Linux (e.g. Raspberry Pi)\n");
    return -1;
}

int ads1115_read_channel(Ads1115 *board, uint8_t channel, int16_t *raw)
{
    (void)board;
    (void)channel;
    if (raw != NULL) {
        *raw = 0;
    }
    return -1;
}

int ads1115_write_register(Ads1115 *board, uint8_t reg, uint16_t value)
{
    (void)board;
    (void)reg;
    (void)value;
    return -1;
}

int ads1115_read_register(Ads1115 *board, uint8_t reg, uint16_t *value)
{
    (void)board;
    (void)reg;
    if (value != NULL) {
        *value = 0;
    }
    return -1;
}

#endif
