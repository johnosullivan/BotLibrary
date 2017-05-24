#ifndef GY521_LOGIC
#define GY521_LOGIC

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

/*#define GRAVITIY_MS2 9.80665
#define ACCEL_SCALE_MODIFIER_2G 16384.0
#define ACCEL_SCALE_MODIFIER_4G 8192.0
#define ACCEL_SCALE_MODIFIER_8G 4096.0
#define ACCEL_SCALE_MODIFIER_16G 2048.0
#define GYRO_SCALE_MODIFIER_250DEG 131.0
#define GYRO_SCALE_MODIFIER_500DEG 65.5
#define GYRO_SCALE_MODIFIER_1000DEG 32.8
#define GYRO_SCALE_MODIFIER_2000DEG 16.4
#define ACCEL_RANGE_2G 0x00
#define ACCEL_RANGE_4G 0x08
#define ACCEL_RANGE_8G 0x10
#define ACCEL_RANGE_16G 0x18
#define GYRO_RANGE_250DEG 0x00
#define GYRO_RANGE_500DEG 0x08
#define GYRO_RANGE_1000DEG 0x10
#define GYRO_RANGE_2000DEG 0x18
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
#define ACCEL_XOUT0 0x3B
#define ACCEL_YOUT0 0x3D
#define ACCEL_ZOUT0 0x3F
#define TEMP_OUT0 0x41
#define GYRO_XOUT0 0x43
#define GYRO_YOUT0 0x45
#define GYRO_ZOUT0 0x47
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG 0x1B*/

sensor_t* config_GY521(int smbus);
double      read_GY521(sensor_t *sensor_object);
void     destroy_GY521(sensor_t *sensor_object);
char*   tostring_GY521(sensor_userdata_t *sensor_data);
char*       info_GY521();

#endif
