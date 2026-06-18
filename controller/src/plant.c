#include <stddef.h>
#include <math.h>

#include "../include/plant.h"
#include "../include/plant_update.h"
#include "../include/config.h"
#include "../include/system_state.h"
#include "../hardware/ads1115.h"

static float rawToMoisturePercent(int raw, const MoistureSensor* sensor)
{
    int range = sensor->dryValue - sensor->wetValue;

    if (range == 0) {
        return 0.0f;
    }

    float percent = 100.0f * (float)(sensor->dryValue - raw) / (float)range;

    if (percent < 0.0f) {
        return 0.0f;
    }
    if (percent > 100.0f) {
        return 100.0f;
    }
    return percent;
}

void updateMoistureSensor(Plant* plant, Ads1115* boards)
{
    int16_t raw = 0;
    int adsIndex;
    Ads1115* board;

    if (plant == NULL || boards == NULL) {
        return;
    }

    adsIndex = plant->moistureSensor.adsIndex;
    if (adsIndex < 0 || adsIndex >= MAX_ADS1115_BOARDS) {
        return;
    }

    board = &boards[adsIndex];
    if (ads1115_read_channel(board, plant->moistureSensor.channel, &raw) != 0) {
        return;
    }

    plant->moistureSensor.rawValue = raw;
    plantUpdateMoisture(plant, rawToMoisturePercent(raw, &plant->moistureSensor));
}

void updatePlant(Plant* plant, Ads1115* boards)
{
    if (plant == NULL || boards == NULL) {
        return;
    }

    updateMoistureSensor(plant, boards);
}

void updateAllPlants(SystemState* system, Ads1115* boards)
{
    if (system == NULL || boards == NULL) {
        return;
    }

    for (int i = 0; i < system->activePlants; i++) {
        updatePlant(&system->plants[i], boards);
    }
}

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
