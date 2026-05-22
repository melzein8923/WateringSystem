#ifndef PLANT_H
#define PLANT_H

typedef struct{
	int id;
	float moisturePercent;
	int needsWater;
	int sensorFault;
	int lastWatered;
} Plant;

#endif
