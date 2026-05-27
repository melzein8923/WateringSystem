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

static BME280_CalibData calib;
static int t_fine;

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

int bme280_is_connected(void)
{
    return bme.connected;
}

int bme280_write_register(unsigned char reg, unsigned char value)
{
    unsigned char buf[2];

    if (!bme.connected || bme.fd < 0) {
        return -1;
    }

    buf[0] = reg;
    buf[1] = value;
    if (write(bme.fd, buf, 2) != 2) {
        fprintf(stderr, "bme280: write reg 0x%02X failed: %s\n", reg, strerror(errno));
        return -1;
    }
    return 0;
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

int bme280_read_calibration(void)
{
    unsigned char data[24];
    unsigned char hum_data[7];
    uint8_t h1 = 0;

    if (bme280_read_bytes(0x88, data, 24) != 0) {
        fprintf(stderr, "bme280: failed to read T/P calibration\n");
        return -1;
    }

    calib.dig_T1 = (data[1] << 8) | data[0];
    calib.dig_T2 = (data[3] << 8) | data[2];
    calib.dig_T3 = (data[5] << 8) | data[4];

    calib.dig_P1 = (data[7] << 8) | data[6];
    calib.dig_P2 = (data[9] << 8) | data[8];
    calib.dig_P3 = (data[11] << 8) | data[10];
    calib.dig_P4 = (data[13] << 8) | data[12];
    calib.dig_P5 = (data[15] << 8) | data[14];
    calib.dig_P6 = (data[17] << 8) | data[16];
    calib.dig_P7 = (data[19] << 8) | data[18];
    calib.dig_P8 = (data[21] << 8) | data[20];
    calib.dig_P9 = (data[23] << 8) | data[22];

    if (bme280_read_register(0xA1, &h1) != 0) {
        fprintf(stderr, "bme280: failed to read humidity calibration (H1)\n");
        return -1;
    }
    calib.dig_H1 = h1;

    if (bme280_read_bytes(0xE1, hum_data, 7) != 0) {
        fprintf(stderr, "bme280: failed to read humidity calibration (H2-H6)\n");
        return -1;
    }

    calib.dig_H2 = (hum_data[1] << 8) | hum_data[0];
    calib.dig_H3 = hum_data[2];
    calib.dig_H4 = (hum_data[3] << 4) | (hum_data[4] & 0x0F);
    calib.dig_H5 = (hum_data[5] << 4) | (hum_data[4] >> 4);
    calib.dig_H6 = (signed char)hum_data[6];

    printf("bme280: calibration loaded\n");
    return 0;
}

int bme280_init(void)
{
    if (bme280_connect() != 0) {
        return -1;
    }

    if (bme280_read_calibration() != 0) {
        return -1;
    }

    if (bme280_write_register(BME280_CTRL_HUM_REG, 0x01) != 0) {
        fprintf(stderr, "bme280: failed to configure humidity oversampling\n");
        return -1;
    }

    if (bme280_write_register(BME280_CONFIG_REG, 0x00) != 0) {
        fprintf(stderr, "bme280: failed to configure standby/filter\n");
        return -1;
    }

    if (bme280_write_register(BME280_CTRL_MEAS_REG, 0x00) != 0) {
        fprintf(stderr, "bme280: failed to set sleep mode\n");
        return -1;
    }

    return 0;
}

static int bme280_trigger_measurement(void)
{
    uint8_t status = 0;

    if (bme280_write_register(BME280_CTRL_MEAS_REG, BME280_FORCED_MEASURE) != 0) {
        return -1;
    }

    for (int i = 0; i < 100; i++) {
        if (bme280_read_register(BME280_REG_STATUS, &status) != 0) {
            return -1;
        }
        if ((status & 0x08) == 0) {
            return 0;
        }
        usleep(1000);
    }

    fprintf(stderr, "bme280: measurement timed out\n");
    return -1;
}

float bme280_compensate_temperature(int adc_T)
{
    float var1, var2, T;

    var1 =
        ((((adc_T >> 3) -
        ((int)calib.dig_T1 << 1))) *
        ((int)calib.dig_T2)) >> 11;

    var2 =
        (((((adc_T >> 4) -
        ((int)calib.dig_T1)) *
        ((adc_T >> 4) -
        ((int)calib.dig_T1))) >> 12) *
        ((int)calib.dig_T3)) >> 14;

    t_fine = var1 + var2;

    T = (t_fine * 5 + 128) >> 8;

    return T / 100.0f;
}

float bme280_compensate_humidity(int adc_H)
{
    float h;

    h = t_fine - 76800.0f;

    h = (adc_H -
        (calib.dig_H4 * 64.0f +
        calib.dig_H5 / 16384.0f * h)) *
        (calib.dig_H2 / 65536.0f *
        (1.0f +
        calib.dig_H6 / 67108864.0f * h *
        (1.0f +
        calib.dig_H3 / 67108864.0f * h)));

    h = h * (1.0f -
        calib.dig_H1 * h / 524288.0f);

    if (h > 100.0f)
        h = 100.0f;

    if (h < 0.0f)
        h = 0.0f;

    return h;
}

float bme280_compensate_pressure(int adc_P)
{
    double var1, var2, p;

    var1 = ((double)t_fine / 2.0) - 64000.0;

    var2 = var1 * var1 *
           ((double)calib.dig_P6) / 32768.0;

    var2 = var2 +
           var1 * ((double)calib.dig_P5) * 2.0;

    var2 = (var2 / 4.0) +
           (((double)calib.dig_P4) * 65536.0);

    var1 =
        (((double)calib.dig_P3) *
        var1 * var1 / 524288.0 +
        ((double)calib.dig_P2) *
        var1) / 524288.0;

    var1 =
        (1.0 + var1 / 32768.0) *
        ((double)calib.dig_P1);

    if (var1 == 0.0)
    {
        return 0;
    }

    p = 1048576.0 - (double)adc_P;

    p = (p - (var2 / 4096.0)) *
        6250.0 / var1;

    var1 =
        ((double)calib.dig_P9) *
        p * p / 2147483648.0;

    var2 =
        p * ((double)calib.dig_P8) /
        32768.0;

    p = p +
        (var1 + var2 +
        ((double)calib.dig_P7)) / 16.0;

    return p / 100.0f;
}

void bme280_read_environment(float *temperature, float *humidity, float *pressure_hpa)
{
    unsigned char data[8];

    if (temperature == NULL || humidity == NULL || pressure_hpa == NULL) {
        return;
    }

    if (bme280_read_bytes(BME280_PRESS_MSB, data, 8) != 0) {
        return;
    }

    int raw_press =
        (data[0] << 12) |
        (data[1] << 4) |
        (data[2] >> 4);

    int raw_temp =
        (data[3] << 12) |
        (data[4] << 4) |
        (data[5] >> 4);

    int raw_hum =
        (data[6] << 8) |
        data[7];

    *temperature = bme280_compensate_temperature(raw_temp);
    *pressure_hpa = bme280_compensate_pressure(raw_press);
    *humidity = bme280_compensate_humidity(raw_hum);
}

int bme280_update_environment(EnvironmentData *env)
{
    float temperature = 0.0f;
    float humidity = 0.0f;
    float pressure_hpa = 0.0f;

    if (env == NULL || !bme.connected) {
        return -1;
    }

    if (bme280_trigger_measurement() != 0) {
        return -1;
    }

    bme280_read_environment(&temperature, &humidity, &pressure_hpa);
    env->tempC = temperature;
    env->humidityPercent = humidity;
    env->pressurehpa = pressure_hpa;
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

int bme280_init(void)
{
    fprintf(stderr, "bme280: I2C is only supported on Linux (e.g. Raspberry Pi)\n");
    return -1;
}

int bme280_connect(void)
{
    fprintf(stderr, "bme280: I2C is only supported on Linux (e.g. Raspberry Pi)\n");
    return -1;
}

int bme280_is_connected(void)
{
    return 0;
}

int bme280_write_register(unsigned char reg, unsigned char value)
{
    (void)reg;
    (void)value;
    return -1;
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

int bme280_read_calibration(void)
{
    return -1;
}

float bme280_compensate_temperature(int adc_T)
{
    (void)adc_T;
    return 0.0f;
}

float bme280_compensate_humidity(int adc_H)
{
    (void)adc_H;
    return 0.0f;
}

float bme280_compensate_pressure(int adc_P)
{
    (void)adc_P;
    return 0.0f;
}

void bme280_read_environment(float *temperature, float *humidity, float *pressure_hpa)
{
    if (temperature != NULL) {
        *temperature = 0.0f;
    }
    if (humidity != NULL) {
        *humidity = 0.0f;
    }
    if (pressure_hpa != NULL) {
        *pressure_hpa = 0.0f;
    }
}

int bme280_update_environment(EnvironmentData *env)
{
    if (env != NULL) {
        env->tempC = 0.0f;
        env->humidityPercent = 0.0f;
        env->pressurehpa = 0.0f;
    }
    return -1;
}

void test_raw_read(void)
{
}

#endif
