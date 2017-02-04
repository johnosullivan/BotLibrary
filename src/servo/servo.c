#include <stdio.h>
#include <stdlib.h>
#include "servo.h"

/*********************************************************
 * Functions for the servo_t structure data type.
 *********************************************************/
servo_t *servo_create(int init, int targ, int sb)
{
    servo_t *s;
    s = malloc(sizeof(*s));
    s->channel = init;
    s->target = targ;
    s->sbconnection = sb;
    return s;
}
void servo_destroy(servo_t *s)
{
    if (s == NULL) return;
    free(s);
}
int servo_getchannel(servo_t *s)
{
    if (s == NULL) return 0;
    return s->channel;
}
int servo_gettarget(servo_t *s)
{
    if (s == NULL) return 0;
    return s->target;
}
int servo_getsbc(servo_t *s)
{
    if (s == NULL) return 0;
    return s->sbconnection;
}
/********************************************************/
