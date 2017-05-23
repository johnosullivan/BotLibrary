#ifndef LFIRS_LOGIC
#define LFIRS_LOGIC

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

sensor_t* config_LFIRS(int output);
bool      read_LFIRS(sensor_t *sensor_object);
void     destroy_LFIRS(sensor_t *sensor_object);
char*   tostring_LFIRS(sensor_userdata_t *sensor_data);
char*       info_LFIRS();

#endif
