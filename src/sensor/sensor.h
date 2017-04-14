#ifndef SENSOR_OBJECT
#define SENSOR_OBJECT

#include <stdio.h>
#include <stdlib.h>

/* Defining Sensor Structures */
struct sensor;
typedef struct sensor sensor_t;
struct sensor {
    int pin1;
    int pin2;
    int pin3;
    int pin4;
    int pin5;
    int pin6;
    int pin7;
    int pin8;
    int pin9;
    int pin10;
};
typedef struct {
    sensor_t *s;
    char      *name;
    char      *type;
} sensor_userdata_t;

int sensor_get_pin(sensor_t *s,int pin);
void sensor_destroy(sensor_t *s);
sensor_t *sensor_create(int pin1,int pin2,int pin3,int pin4,int pin5,int pin6,int pin7,int pin8,int pin9,int pin10);

#endif
