#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include "system_state.h"
#include "../hardware/ads1115.h"

void initialize_system(SystemState* system, int numPlants);
int initialize_ads1115_boards(Ads1115* boards, int numPlants);

#endif
