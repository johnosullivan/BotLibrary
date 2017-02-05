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

#define lrolibservo_c
#define VERSIONROLIB (l_mathop(4.0))

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
#include "servo.h"

#ifdef _WIN32
#define O_NOCTTY 0
#else
#include <termios.h>
#endif

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

// Creates a new servo object
static int lservo_new(lua_State *L)
{
    servo_userdata_t    *so;
    const char          *name;
    const char          *type;
    int                  channel;
    int                  target;
    int                  sbc;
    channel  = luaL_checknumber(L, 1);
    name   = luaL_checkstring(L, 2);
    type   = luaL_checkstring(L, 3);
    target = luaL_checknumber(L, 4);
    sbc    = luaL_checknumber(L, 5);
    if (name == NULL) luaL_error(L, "");
    so       = (servo_userdata_t *)lua_newuserdata(L, sizeof(*so));
    so->s    = NULL;
    so->name = NULL;
    so->type = NULL;
    luaL_getmetatable(L, "Servo");
    lua_setmetatable(L, -2);
    so->s    = servo_create(channel,target,sbc);
    so->name = strdup(name);
    so->type = strdup(type);
    return 1;
}
// Gets the servo channel on the board
static int lservo_getchannel(lua_State *L)
{
    servo_userdata_t *so;
    so = (servo_userdata_t *)luaL_checkudata(L, 1, "Servo");
    lua_pushinteger(L, servo_getchannel(so->s));
    return 1;
}
// Gets the servo target on the board
static int lservo_gettarget(lua_State *L)
{
    servo_userdata_t *so;
    so = (servo_userdata_t *)luaL_checkudata(L, 1, "Servo");
    lua_pushinteger(L, servo_gettarget(so->s));
    return 1;
}
// Gets the servo name set.
static int lservo_getname(lua_State *L)
{
    servo_userdata_t *so;
    so = (servo_userdata_t *)luaL_checkudata(L, 1, "Servo");
    lua_pushstring(L, so->name);
    return 1;
}
// Sets the targets of the current servo.
static int lservo_setTarget(lua_State *L)
{
    servo_userdata_t *so;
    int targetval;
    int channel;
    int getconnect;
    const char *type;
    so = (servo_userdata_t *)luaL_checkudata(L, 1, "Servo");
    //Gets parameters
    targetval = luaL_checkinteger(L, 2);
    type = lua_pushstring(L, so->type);
    channel = servo_getchannel(so->s);
    getconnect = servo_getsbc(so->s);
    // Excute servo command via usb serial port.
    return 1;
}
// Destroys the servo object.
static int lservo_destroy(lua_State *L)
{
    servo_userdata_t *so;
    so = (servo_userdata_t *)luaL_checkudata(L, 1, "Servo");
    if (so->s != NULL) servo_destroy(so->s);
    so->s = NULL;
    if (so->name != NULL) free(so->name);
    so->name = NULL;
    if (so->type != NULL) free(so->type);
    so->type = NULL;
    return 0;
}
// Convert object to string
static int lservo_tostring(lua_State *L)
{
    servo_userdata_t *so;
    so = (servo_userdata_t *)luaL_checkudata(L, 1, "Servo");
    lua_pushfstring(L, "%s[%d]-%s(%d)",so->type, servo_getchannel(so->s),so->name,servo_gettarget(so->s));
    return 1;
}
// Connects and get USB Serial connection ID
static int lservoboradconnection(lua_State *L) {
    const char * device = luaL_checkstring(L, 1);
    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        lua_pushinteger(L, -1);
    }
    lua_pushinteger(L, fd);
    return 1;
}
/* Servo functions */
static const struct luaL_Reg lservo_methods[] = {
    { "getchannel",  lservo_getchannel},
    { "gettarget",   lservo_gettarget },
    { "getname",     lservo_getname   },
    { "setTarget",   lservo_setTarget },
    { "__gc",        lservo_destroy   },
    { "__tostring",  lservo_tostring  },
    { NULL,          NULL             },
};
/* Library functions */
static const struct luaL_Reg lservo_functions[] = {
    { "create_servo",   lservo_new       },
    { "sbc",     lservoboradconnection   },
    { NULL,          NULL                }
};
/* Init the Lua Robot Library */
LUAMOD_API int luaopen_rolibservo (lua_State *L) {
    luaL_newmetatable(L, "Servo");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lservo_methods, 0);
    luaL_newlib(L, lservo_functions);
    lua_pushnumber(L, VERSIONROLIB);
    lua_setfield(L, -2, "version");
    return 1;
}
