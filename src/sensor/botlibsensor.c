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

#define lbotlibsensor_c
#define VERSIONbotlib (l_mathop(1.0))

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

/* Defining pthread structure
#include <pthread.h>
#include "../thpool.h"
#define NUM_THREADS	5 */

/* Including sensor objects */
#include "sensor.h"
#include "../gpio/gpio.h"
#include "sensors/HCSR04/HCSR04.h"
/* Defining sensor types */
#define INVALIDSENSOR "Error invalid sensor, please check your sensor's configuration."
#define HCSR04  "HC-SR04"

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
    so->callback = NULL;
    luaL_getmetatable(L, "Sensor");

    lua_setmetatable(L, -2);
    if (strcmp(type,HCSR04)==0) {
      so->s = config_HCSR04(luaL_checknumber(L, 3), luaL_checknumber(L, 4));
    }

    so->name = strdup(name);
    so->type = strdup(type);
    so->callback = NULL;

    return 1;
}
static int lsensor_read(lua_State *L)
{
    sensor_userdata_t *so;
    const char *type;
    so = (sensor_userdata_t *)luaL_checkudata(L, 1, "Sensor");
    type = lua_pushstring(L, so->type);

    if (strcmp(so->type,HCSR04)==0) {
      lua_pushnumber(L, read_HCSRO4(so->s));
    } else {
      lua_pushnumber(L, 0.0);
    }

    return 1;
}
static int lsensor_destroy(lua_State *L)
{
    sensor_userdata_t *so;
    so = (sensor_userdata_t *)luaL_checkudata(L, 1, "Sensor");

    if (strcmp(so->type,HCSR04)==0) {
      destroy_HCSR04(so->s);
    }

    if (so->s != NULL) sensor_destroy(so->s);
    so->s = NULL;
    if (so->name != NULL) free(so->name);
    so->name = NULL;
    if (so->type != NULL) free(so->type);
    so->type = NULL;

    return 0;
}
static int lsensor_tostring(lua_State *L)
{
    sensor_userdata_t *so;
    so = (sensor_userdata_t *)luaL_checkudata(L, 1, "Sensor");

    if (strcmp(so->type,HCSR04)==0) {
      lua_pushfstring(L, tostring_HCSR04(so));
    } else {
      lua_pushfstring(L, INVALIDSENSOR);
    }
    return 1;
}
static int lsensor_linfo(lua_State *L)
{
  sensor_userdata_t *so;
  so = (sensor_userdata_t *)luaL_checkudata(L, 1, "Sensor");

  if (strcmp(so->type,HCSR04)==0) {
    lua_pushfstring(L, info_HCSR04());
  } else {
    lua_pushfstring(L, INVALIDSENSOR);
  }

  return 1;
}
static int lsensor_lcallback(lua_State *L)
{
  sensor_userdata_t *so;
  so = (sensor_userdata_t *)luaL_checkudata(L, 1, "Sensor");
  const char *callbackName;
  callbackName = luaL_checkstring(L, 2);
  so->callback = callbackName;
  return 1;
}



static int lsensor_setup(lua_State *L)
{
    sensor_userdata_t *so;
    const char *type;
    so = (sensor_userdata_t *)luaL_checkudata(L, 1, "Sensor");
    type = lua_pushstring(L, so->type);
    if (strcmp(so->type,HCSR04)==0) { }
    return 1;
}
/*
threadpool thpool;
int signal_thread = 1;
int needQuit() { return signal_thread; }
void signal_off() { signal_thread = 0; }
void signal_on()  { signal_thread = 1; }
struct thread_args {
    sensor_userdata_t *data;
    lua_State *L;
};
void doThread(void *param){
  struct thread_args *temp = (struct thread_args *)param;
  while (needQuit()) {
    lua_State *L = temp->L;
    lua_getglobal(L, temp->data->callback);
    if(!lua_isfunction(L,-1))
    {
      lua_pop(L,1);
    } else {
      lua_createtable(L, 0, 2);
      lua_pushstring(L, "sensor");
      lua_pushstring(L, temp->data->name);
      lua_settable(L, -3);
      lua_pushstring(L, "value");
      lua_pushnumber(L, 100.0);
      lua_settable(L, -3);
      if (lua_pcall(L, 1, 1, 0) != 0) {
          printf("error running function `f': %s\n",lua_tostring(L, -1));
      }
    }
  }
}
static int lsensor_kill(lua_State *L)
{
  signal_off();
	thpool_destroy(thpool);
  return 1;
}
*/


/* Servo functions */
static const struct luaL_Reg lservo_methods[] = {
    { "read",        lsensor_read      },
    { "setCallBack", lsensor_lcallback },
    { "setup",       lsensor_setup     },
    { "info",        lsensor_linfo     },
    { "__gc",        lsensor_destroy   },
    { "__tostring",  lsensor_tostring  },
    { NULL,          NULL              },
    { NULL,          NULL              },
};
/* Library functions */
static const struct luaL_Reg lservo_functions[] = {
    { "new",         lsensor_new         },
    { NULL,          NULL                },
    { NULL,          NULL                }
};
/* Init the Lua Robot Library */
LUAMOD_API int luaopen_botlibsensor (lua_State *L) {
    //thpool = thpool_init(4);
    luaL_newmetatable(L, "Sensor");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lservo_methods, 0);
    luaL_newlib(L, lservo_functions);
    lua_pushnumber(L, VERSIONbotlib);
    lua_setfield(L, -2, "version");
    return 1;
}
