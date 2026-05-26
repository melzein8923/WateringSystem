#ifndef PLANT_H
#define PLANT_H

typedef struct {
	int id;
	float moisturePercent;
	int sensorFault;
	int needsWater;
	float lastReading;
	int stuckStreak;
	int goodStreak;
} Plant;

void plantUpdateMoisture(Plant* plant, float reading);
int isPlantDry(const Plant* plant);

#endif
