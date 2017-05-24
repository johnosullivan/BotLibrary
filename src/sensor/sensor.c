#include <stdio.h>
#include <stdlib.h>
#include "sensor.h"

#ifdef _WIN32
double timeout_gettime(void) {
    FILETIME ft;
    double t;
    GetSystemTimeAsFileTime(&ft);
    t  = ft.dwLowDateTime/1.0e7 + ft.dwHighDateTime*(4294967296.0/1.0e7);
    return (t - 11644473600.0);
}
#else
#include <sys/time.h>
double timeout_gettime(void) {
    struct timeval v;
    gettimeofday(&v, (struct timezone *) NULL);
    return v.tv_sec + v.tv_usec/1.0e6;
}
#endif
/* Functions for the servo_t structure data type. */
sensor_t *sensor_create(gpio_t pin1,gpio_t pin2,gpio_t pin3,gpio_t pin4,gpio_t pin5,gpio_t pin6,gpio_t pin7,gpio_t pin8,gpio_t pin9,gpio_t pin10)
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
    s->address = 0;
    s->bus = 0;
    s->fd = 0;
    return s;
}
void sensor_destroy(sensor_t *s)
{
    if (s == NULL) return;
    free(s);
}
gpio_t sensor_get_pin(sensor_t *s,int pin)
{
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
    return s->pin1;
}
