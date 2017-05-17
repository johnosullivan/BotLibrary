#ifndef SENSOR_OBJECT
#define SENSOR_OBJECT

#include <stdio.h>
#include <stdlib.h>
#include "../gpio/gpio.h"
/* Defining Sensor Structures */
struct sensor;
typedef struct sensor sensor_t;
struct sensor {
    gpio_t pin1;
    gpio_t pin2;
    gpio_t pin3;
    gpio_t pin4;
    gpio_t pin5;
    gpio_t pin6;
    gpio_t pin7;
    gpio_t pin8;
    gpio_t pin9;
    gpio_t pin10;
};
typedef struct {
    sensor_t *s;
    char      *name;
    char      *type;
    const char      *callback;
} sensor_userdata_t;

gpio_t sensor_get_pin(sensor_t *s,int pin);
void sensor_destroy(sensor_t *s);
sensor_t *sensor_create(gpio_t pin1,gpio_t pin2,gpio_t pin3,gpio_t pin4,gpio_t pin5,gpio_t pin6,gpio_t pin7,gpio_t pin8,gpio_t pin9,gpio_t pin10);

#endif
