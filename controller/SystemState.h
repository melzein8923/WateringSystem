#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H
#include "plant.h"
#define MAX_PLANTS 8

typedef enum{
	STATE_INIT,
	STATE_MONITORING,
	STATE_WATERING,
	STATE_FAULT
} SystemMode;

typedef struct{
	Plant plants[MAX_PLANTS];
	int activePlants;
	SystemMode mode;
	int pumpActive;
} SystemState;

#endif
	
