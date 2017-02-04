#include <stdio.h>
#include <stdlib.h>

#ifndef SERVO_OBJECT
#define SERVO_OBJECT

/* Defining Servo Structures */
struct servo;
typedef struct servo servo_t;
struct servo {
    int channel;
    int target;
    int sbconnection;
};
typedef struct {
    servo_t *s;
    char      *name;
    char      *type;
} servo_userdata_t;
servo_t *servo_create(int init, int targ, int sb);
void servo_destroy(servo_t *s);
int servo_getchannel(servo_t *s);
int servo_gettarget(servo_t *s);
int servo_getsbc(servo_t *s);

#endif
