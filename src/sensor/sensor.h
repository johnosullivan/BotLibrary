#ifndef SENSOR_OBJECT
#define SENSOR_OBJECT

#include <stdio.h>
#include <stdlib.h>

/* Defining Sensor Structures */
struct sensor;
typedef struct sensor sensor_t;
struct sensor {
    int channel;
    int target;
    int sbconnection;
};
typedef struct {
    sensoro_t *s;
    char      *name;
    char      *type;
} sensor_userdata_t;



#endif
