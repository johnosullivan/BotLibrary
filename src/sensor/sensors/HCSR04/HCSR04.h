#ifndef HCSR04_LOGIC
#define HCSR04_LOGIC

#include <stdio.h>
#include <stdlib.h>
#include "../../sensor.h"
#include "../../../gpio/gpio.h"

#ifdef __APPLE__
#define DEBUG true
#include <unistd.h>
#elif __linux__
#define DEBUG false
#include <unistd.h>
#elif _WIN32
#define DEBUG true
#include <windows.h>
#endif

sensor_t* config_HCSR04(int trigger, int echo);
double      read_HCSRO4(sensor_t *sensor_object);
void     destroy_HCSR04(sensor_t *sensor_object);
char*   tostring_HCSR04(sensor_userdata_t *sensor_data);
char*       info_HCSR04();

#endif
