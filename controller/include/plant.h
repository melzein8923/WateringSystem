#ifndef PLANT_H
#define PLANT_H

typedef struct{
	int id;
	float moisturePercent;
	int sensorFault;
} Plant;

int isPlantDry(const Plant* plant);

#endif
