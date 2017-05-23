#ifndef WAPIRS_LOGIC
#define WAPIRS_LOGIC

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

sensor_t* config_WAPIRS(int output);
bool      read_WAPIRS(sensor_t *sensor_object);
void     destroy_WAPIRS(sensor_t *sensor_object);
char*   tostring_WAPIRS(sensor_userdata_t *sensor_data);
char*       info_WAPIRS();

#endif
