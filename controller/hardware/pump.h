#ifndef PUMP_H
#define PUMP_H

#include <stdbool.h>

bool pumpInit(void);
void pumpOn(void);
void pumpOff(void);
bool pumpIsOn(void);
void pumpCleanup(void);

#endif