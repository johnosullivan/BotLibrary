#include <stdio.h>
#include <stdlib.h>
#include "sensor.h"

/* Functions for the servo_t structure data type. */
sensor_t *sensor_create(int pin1,int pin2,int pin3,int pin4,int pin5,int pin6,int pin7,int pin8,int pin9,int pin10)
{
    sensor_t *s;
    s = malloc(sizeof(*s));
    s->pin1 = pin1;
    s->pin2 = pin2;
    s->pin3 = pin3;
    s->pin4 = pin4;
    s->pin5 = pin5;
    s->pin6 = pin6;
    s->pin7 = pin7;
    s->pin8 = pin8;
    s->pin9 = pin9;
    s->pin10 = pin10;
    return s;
}
void sensor_destroy(sensor_t *s)
{
    if (s == NULL) return;
    free(s);
}
int sensor_get_pin(sensor_t *s,int pin)
{
    if (s == NULL) return 0;
    if (pin == 1) { return s->pin1; }
    if (pin == 2) { return s->pin2; }
    if (pin == 3) { return s->pin3; }
    if (pin == 4) { return s->pin4; }
    if (pin == 5) { return s->pin5; }
    if (pin == 6) { return s->pin6; }
    if (pin == 7) { return s->pin7; }
    if (pin == 8) { return s->pin8; }
    if (pin == 9) { return s->pin9; }
    if (pin == 10) { return s->pin10; }
    return 0;
}
