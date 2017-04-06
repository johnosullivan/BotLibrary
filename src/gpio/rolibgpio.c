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

#define lrolibgpio_c
#define VERSIONROLIB (l_mathop(2.0))

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
//Support gpio
#include "gpio.h"

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

/*
* Background reading on the linux kernal gpio pins.
* https://www.kernel.org/doc/Documentation/gpio/sysfs.txt
*/

static const char *gpio_direction_to_string[] = {
    [GPIO_DIRECTION_IN]         = "in",
    [GPIO_DIRECTION_OUT]        = "out",
    [GPIO_DIRECTION_OUT_LOW]    = "low",
    [GPIO_DIRECTION_OUT_HIGH]   = "high",
};

static const char *gpio_edge_to_string[] = {
    [GPIO_EDGE_NONE]      = "none",
    [GPIO_EDGE_RISING]    = "rising",
    [GPIO_EDGE_FALLING]   = "falling",
    [GPIO_EDGE_BOTH]      = "both",
};

static int gpio_open(lua_State *L) {
  return 0;
}

static int gpio_new(lua_State *L) {
    lua_remove(L, 1);
    lua_newuserdata(L, sizeof(gpio_t));
    luaL_getmetatable(L, "GPIO");
    lua_setmetatable(L, -2);
    lua_insert(L, 1);
    gpio_open(L);
    lua_settop(L, 1);
    return 1;
}

static void gpio_checktype(lua_State *L, int index, int type) {

}
static int gpio_newindex(lua_State *L) {
  return 0;
}
static int gpio_index(lua_State *L) {
  return 0;
}
static int gpio_close(lua_State *L) {
  return 0;
}
static int gpio_poll(lua_State *L) {
  return 0;
}
static int gpio_write(lua_State *L) {
  return 0;
}
static int gpio_read(lua_State *L) {
  return 0;
}

static int gpio_tostring(lua_State *L) {
  gpio_t *gpio;
  lua_pushstring(L, "GPIO Object");
  return 1;
}

static const struct luaL_Reg gpio_method[] = {
    {"close", gpio_close},
    {"read", gpio_read},
    {"write", gpio_write},
    {"poll", gpio_poll},
    {"__gc", gpio_close},
    {"__tostring", gpio_tostring},
    {"__index", gpio_index},
    {"__newindex", gpio_newindex},
    {NULL, NULL}
};

/* Init the Lua Robot Library */
LUAMOD_API int luaopen_rolibgpio (lua_State *L) {
  luaL_newmetatable(L, "GPIO");

  const struct luaL_Reg *funcs = (const struct luaL_Reg *)gpio_method;
  for (; funcs->name != NULL; funcs++) {
      lua_pushcclosure(L, funcs->func, 0);
      lua_setfield(L, -2, funcs->name);
  }
  lua_pushstring(L, "protected");
  lua_setfield(L, -2, "__metatable");

  lua_newtable(L);
  lua_pushcclosure(L, gpio_new, 0);
  lua_setfield(L, -2, "__call");
  lua_pushstring(L, "protected");
  lua_setfield(L, -2, "__metatable");
  lua_setmetatable(L, -2);

  lua_pushstring(L, "1.0");
  lua_setfield(L, -2, "version");

  return 1;
}
