/*
 Copyright 2017 John O'Sullivan

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define lrolibsensor_c
#define VERSIONROLIB (l_mathop(1.0))

/* Linking Frameworks */
#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __APPLE__
#include <time.h>
#elif __linux__
#include <sys/time.h>
#endif

#ifdef _WIN32
#define O_NOCTTY 0
#else
#include <termios.h>
#endif

#include "sensor.h"
#include "../gpio/gpio.h"
/* Defining Vars */
#undef PI
#define PI  (l_mathop(3.141592653589793238462643383279502884))
#if !defined(l_rand)
#if defined(LUA_USE_POSIX)
#define l_rand()    random()
#define l_srand(x)  srandom(x)
#define L_RANDMAX   2147483647
#else
#define l_rand()    rand()
#define l_srand(x)  srand(x)
#define L_RANDMAX   RAND_MAX
#endif
#endif


#define HCSR04  "HC-SR04"

//Gettime in ms
#ifdef _WIN32
double timeout_gettime(void) {
    FILETIME ft;
    double t;
    GetSystemTimeAsFileTime(&ft);
    /* Windows file time (time since January 1, 1601 (UTC)) */
    t  = ft.dwLowDateTime/1.0e7 + ft.dwHighDateTime*(4294967296.0/1.0e7);
    /* convert to Unix Epoch time (time since January 1, 1970 (UTC)) */
    return (t - 11644473600.0);
}
#else
double timeout_gettime(void) {
    struct timeval v;
    gettimeofday(&v, (struct timezone *) NULL);
    /* Unix Epoch time (time since January 1, 1970 (UTC)) */
    return v.tv_sec + v.tv_usec/1.0e6;
}
#endif
// Creates a new servo object
static int lsensor_new(lua_State *L)
{
    sensor_userdata_t    *so;
    const char          *name;
    const char          *type;
    name   = luaL_checkstring(L, 1);
    type   = luaL_checkstring(L, 2);

    if (name == NULL) luaL_error(L, "");
    so       = (sensor_userdata_t *)lua_newuserdata(L, sizeof(*so));
    so->s    = NULL;
    so->name = NULL;
    so->type = NULL;
    luaL_getmetatable(L, "Sensor");
    lua_setmetatable(L, -2);


    if (strcmp(type,HCSR04)==0) {
      int trigger = luaL_checknumber(L, 3);
      int echo = luaL_checknumber(L, 4);

      gpio_t gpio_in, gpio_out;
      if (pin_open(&gpio_in, echo, GPIO_DIRECTION_IN) < 0) {
        fprintf(stderr, "pin_open(): %s\n", pin_errmsg(&gpio_in));
        exit(1);
      }

      if (pin_open(&gpio_out, trigger, GPIO_DIRECTION_OUT) < 0) {
        fprintf(stderr, "pin_open(): %s\n", pin_errmsg(&gpio_out));
        exit(1);
      }

      so->s = sensor_create(gpio_out,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in);
    }


    so->name = strdup(name);
    so->type = strdup(type);

    return 1;
}

static int lsensor_read(lua_State *L)
{
    sensor_userdata_t *so;
    const char *type;
    so = (sensor_userdata_t *)luaL_checkudata(L, 1, "Sensor");
    type = lua_pushstring(L, so->type);

    if (strcmp(so->type,HCSR04)==0) {
      gpio_t gpio_in = sensor_get_pin(so->s,2);
      gpio_t gpio_out = sensor_get_pin(so->s,1);
      double start, stop;

      if (pin_write(&gpio_out, true) < 0) {
          fprintf(stderr, "pin_write(): %s\n", pin_errmsg(&gpio_out));
          exit(1);
      }

      sleep((unsigned int)0.00001);

      if (pin_write(&gpio_out, false) < 0) {
          fprintf(stderr, "pin_write(): %s\n", pin_errmsg(&gpio_out));
          exit(1);
      }


      bool echo = false;
      start = timeout_gettime();
      stop = timeout_gettime();


      while (echo == false) {
        start = timeout_gettime();
        if (pin_read(&gpio_in, &echo) < 0) {
          fprintf(stderr, "pin_read(): %s\n", pin_errmsg(&gpio_in));
          exit(1);
        }
      };

      while (echo == true) {
        stop = timeout_gettime();
        if (pin_read(&gpio_in, &echo) < 0) {
          fprintf(stderr, "pin_read(): %s\n", pin_errmsg(&gpio_in));
          exit(1);
        }
      }

      double timeelapsed = stop - start;

      double distance = (timeelapsed * 34300) / 2;

      lua_pushnumber(L, distance);
    } else {
      lua_pushnumber(L, 0);
    }

    return 1;
}
// Sets the targets of the current servo.
static int lsensor_setup(lua_State *L)
{
    sensor_userdata_t *so;
    const char *type;
    so = (sensor_userdata_t *)luaL_checkudata(L, 1, "Sensor");
    type = lua_pushstring(L, so->type);

    if (strcmp(so->type,HCSR04)==0) {

    }

    return 1;
}
// Destroys the servo object.
static int lsensor_destroy(lua_State *L)
{
    sensor_userdata_t *so;
    so = (sensor_userdata_t *)luaL_checkudata(L, 1, "Sensor");

    if (strcmp(so->type,HCSR04)==0) {
      gpio_t gpio_in = sensor_get_pin(so->s,2);
      gpio_t gpio_out = sensor_get_pin(so->s,1);
      pin_close(&gpio_in);
      pin_close(&gpio_out);
    }

    if (so->s != NULL) sensor_destroy(so->s);
    so->s = NULL;
    if (so->name != NULL) free(so->name);
    so->name = NULL;
    if (so->type != NULL) free(so->type);
    so->type = NULL;
    return 0;
}
// Convert object to string
static int lsensor_tostring(lua_State *L)
{
    sensor_userdata_t *so;
    so = (sensor_userdata_t *)luaL_checkudata(L, 1, "Sensor");



    if (strcmp(so->type,HCSR04)==0) {
      lua_pushfstring(L, "%s[%s Trigger: %d Echo: %d]",so->name, so->type,sensor_get_pin(so->s,1),sensor_get_pin(so->s,2));
    } else {
      lua_pushfstring(L, "Unknown Sensor");
    }

    return 1;
}
// Get info per Servo
static int lsensor_linfo(lua_State *L)
{

  lua_pushfstring(L,"");

  return 1;
}
/* Servo functions */
static const struct luaL_Reg lservo_methods[] = {
    { "read",        lsensor_read      },
    { "setup",       lsensor_setup     },
    { "info",        lsensor_linfo     },
    { "__gc",        lsensor_destroy   },
    { "__tostring",  lsensor_tostring  },
    { NULL,          NULL             },
    { NULL,          NULL             },
};
/* Library functions */
static const struct luaL_Reg lservo_functions[] = {
    { "new",   lsensor_new       },
    { NULL,          NULL                },
    { NULL,          NULL                }
};
/* Init the Lua Robot Library */
LUAMOD_API int luaopen_rolibsensor (lua_State *L) {
    luaL_newmetatable(L, "Sensor");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lservo_methods, 0);
    luaL_newlib(L, lservo_functions);
    lua_pushnumber(L, VERSIONROLIB);
    lua_setfield(L, -2, "version");
    return 1;
}
