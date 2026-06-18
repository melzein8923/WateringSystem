#ifndef PLANT_UPDATE_H
#define PLANT_UPDATE_H

#include "system_state.h"
#include "../hardware/ads1115.h"

void updateMoistureSensor(Plant* plant, Ads1115* boards);
void updatePlant(Plant* plant, Ads1115* boards);
void updateAllPlants(SystemState* system, Ads1115* boards);

#endif
