#include "plant.h"
#include "config.h"

int isPlantDry(const Plant* plant) {
    if (plant->sensorFault) {
        return 0;
    }
    return plant->moisturePercent < WATERING_THRESHOLD;
}
