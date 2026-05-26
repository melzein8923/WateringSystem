#include <math.h>

#include "../include/plant.h"
#include "../include/config.h"

static int readingOutOfRange(float reading) {
    return reading < 0.0f || reading > 100.0f;
}

static int readingUnchanged(Plant* plant, float reading) {
    return fabsf(reading - plant->lastReading) < SENSOR_STUCK_EPSILON;
}

static float clampMoisture(float reading) {
    if (reading < 0.0f) {
        return 0.0f;
    }
    if (reading > 100.0f) {
        return 100.0f;
    }
    return reading;
}

void plantUpdateMoisture(Plant* plant, float reading) {
    if (readingOutOfRange(reading)) {
        plant->stuckStreak = 0;
        plant->goodStreak = 0;
        plant->sensorFault = 1;
        plant->lastReading = reading;
        return;
    }

    if (readingUnchanged(plant, reading)) {
        plant->stuckStreak++;
    } else {
        plant->stuckStreak = 0;
    }
    plant->lastReading = reading;

    if (plant->stuckStreak >= SENSOR_STUCK_COUNT) {
        plant->goodStreak = 0;
        plant->sensorFault = 1;
        return;
    }

    if (plant->sensorFault) {
        plant->goodStreak++;
        if (plant->goodStreak >= SENSOR_FAULT_CLEAR_COUNT) {
            plant->sensorFault = 0;
            plant->goodStreak = 0;
            plant->moisturePercent = clampMoisture(reading);
        }
        return;
    }

    plant->goodStreak = 0;
    plant->moisturePercent = clampMoisture(reading);
}

int isPlantDry(const Plant* plant) {
    if (plant->sensorFault) {
        return 0;
    }
    return plant->moisturePercent < WATERING_THRESHOLD;
}
