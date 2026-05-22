#include <system_init.h>

void initialize_system(SystemState* system){
	system->active_plant_count = 4;
    system->pump_active = 0;
    system->mode = STATE_MONITORING;
    for (int i = 0; i < system->active_plant_count; i++)
    {
        system->plants[i].id = i + 1;
        system->plants[i].moisture_percent = 50.0f;
        system->plants[i].needs_water = 0;
        system->plants[i].sensor_fault = 0;
        system->plants[i].watering_enabled = 1;
    }
}
