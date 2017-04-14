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

#define lbotlibgpio_c
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

//Wrapper from lua 5.2
#define lua_pushunsigned(L,n)   lua_pushinteger(L, (lua_Integer)(n))
#define lua_tounsignedx(L,i,is) ((lua_Unsigned)lua_tointegerx(L,i,is))
#define lua_tounsigned(L,i)     lua_tounsignedx(L,(i),NULL)

/*
* Background reading on the linux kernal gpio pins.
* https://www.kernel.org/doc/Documentation/gpio/sysfs.txt
*/

static const char *gpio_error_strings[] = {
    [-GPIO_ERROR_ARG]           = "GPIO_ERROR_ARG",
    [-GPIO_ERROR_EXPORT]        = "GPIO_ERROR_EXPORT",
    [-GPIO_ERROR_OPEN]          = "GPIO_ERROR_OPEN",
    [-GPIO_ERROR_IO]            = "GPIO_ERROR_IO",
    [-GPIO_ERROR_CLOSE]         = "GPIO_ERROR_CLOSE",
    [-GPIO_ERROR_SET_DIRECTION] = "GPIO_ERROR_SET_DIRECTION",
    [-GPIO_ERROR_GET_DIRECTION] = "GPIO_ERROR_GET_DIRECTION",
    [-GPIO_ERROR_SET_EDGE]      = "GPIO_ERROR_SET_EDGE",
    [-GPIO_ERROR_GET_EDGE]      = "GPIO_ERROR_GET_EDGE",
};

static int pin_error(lua_State *L, enum gpio_error_code code, int c_errno, const char *fmt, ...) {
    char message[128];
    va_list ap;
    va_start(ap, fmt);
    lua_newtable(L);
    lua_pushstring(L, gpio_error_strings[-code]);
    lua_setfield(L, -2, "code");
    lua_pushinteger(L, c_errno);
    lua_setfield(L, -2, "c_errno");
    vsnprintf(message, sizeof(message), fmt, ap);
    lua_pushstring(L, message);
    lua_setfield(L, -2, "message");
    va_end(ap);
    luaL_getmetatable(L, "botlib.error");
    lua_setmetatable(L, -2);
    return lua_error(L);
}

static void pin_checktype(lua_State *L, int index, int type) {
    if (lua_type(L, index) != type) {
        pin_error(L, GPIO_ERROR_ARG, 0, "Error: invalid argument #%d (%s expected, got %s)", index, lua_typename(L, type), lua_typename(L, lua_type(L, index)));
    }
}

static int gpio_open(lua_State *L) {
    gpio_t *gpio;
    unsigned int pin;
    gpio_direction_t direction;
    const char *str_direction;
    int ret;
    gpio = luaL_checkudata(L, 1, "GPIO");
    gpio->fd = -1;
    if (lua_istable(L, 2)) {
        lua_getfield(L, 2, "pin");
        if (!lua_isnumber(L, -1))
            return pin_error(L, GPIO_ERROR_ARG, 0, "Error: invalid type of table argument 'pin', should be number");
        pin = lua_tounsigned(L, -1);

        lua_getfield(L, 2, "direction");
        if (lua_isnil(L, -1))
            str_direction = "preserve";
        else if (lua_isstring(L, -1))
            str_direction = lua_tostring(L, -1);
        else
            return pin_error(L, GPIO_ERROR_ARG, 0, "Error: invalid type of table argument 'direction', should be string");

    } else {
        pin_checktype(L, 2, LUA_TNUMBER);
        pin = lua_tounsigned(L, 2);

        if (lua_gettop(L) > 2) {
            pin_checktype(L, 3, LUA_TSTRING);
            str_direction = lua_tostring(L, 3);
        } else {
            str_direction = "preserve";
        }
    }

    if (strcmp(str_direction, "in") == 0)
        direction = GPIO_DIRECTION_IN;
    else if (strcmp(str_direction, "out") == 0)
        direction = GPIO_DIRECTION_OUT;
    else if (strcmp(str_direction, "low") == 0)
        direction = GPIO_DIRECTION_OUT_LOW;
    else if (strcmp(str_direction, "high") == 0)
        direction = GPIO_DIRECTION_OUT_HIGH;
    else if (strcmp(str_direction, "preserve") == 0)
        direction = GPIO_DIRECTION_PRESERVE;
    else
        return pin_error(L, GPIO_ERROR_ARG, 0, "Error: Invalid Direction");

    if ((ret = pin_open(gpio, pin, direction)) < 0)
        return pin_error(L, ret, pin_errno(gpio), pin_errmsg(gpio));

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

static int gpio_read(lua_State *L) {
    gpio_t *gpio;
    bool value;
    int ret;
    gpio = luaL_checkudata(L, 1, "GPIO");
    if ((ret = pin_read(gpio, &value)) < 0) {
        return pin_error(L, ret, pin_errno(gpio), "Error: %s", pin_errmsg(gpio));
    }
    lua_pushboolean(L, value);
    return 1;
}

static int gpio_write(lua_State *L) {
    gpio_t *gpio;
    bool value;
    int ret;
    gpio = luaL_checkudata(L, 1, "GPIO");
    if (lua_isnumber(L, 2))
        value = (lua_tointeger(L, 2)) ? true : false;
    else if (lua_isboolean(L, 2))
        value = (lua_toboolean(L, 2)) ? true : false;
    else
        return pin_error(L, GPIO_ERROR_ARG, 0, "Error: invalid value type (number or boolean expected, got %s)", lua_typename(L, lua_type(L, 2)));

    if ((ret = pin_write(gpio, value)) < 0)
        return pin_error(L, ret, pin_errno(gpio), "Error: %s", pin_errmsg(gpio));

    return 0;
}

static int gpio_poll(lua_State *L) {
    gpio_t *gpio;
    int timeout_ms;
    int ret;
    gpio = luaL_checkudata(L, 1, "GPIO");
    pin_checktype(L, 2, LUA_TNUMBER);
    timeout_ms = lua_tointeger(L, 2);
    if ((ret = pin_poll(gpio, timeout_ms)) < 0) {
        return pin_error(L, ret, pin_errno(gpio), "Error: %s", pin_errmsg(gpio));
    }
    lua_pushboolean(L, ret > 0);
    return 1;
}

static int gpio_close(lua_State *L) {
    gpio_t *gpio;
    int ret;
    gpio = luaL_checkudata(L, 1, "GPIO");
    if ((ret = pin_close(gpio)) < 0) {
        return pin_error(L, ret, pin_errno(gpio), "Error: %s", pin_errmsg(gpio));
    }
    gpio->fd = -1;
    return 0;
}

static int gpio_tostring(lua_State *L) {
    gpio_t *gpio;
    char gpio_str[128];
    gpio = luaL_checkudata(L, 1, "GPIO");
    pin_tostring(gpio, gpio_str, sizeof(gpio_str));
    lua_pushstring(L, gpio_str);
    return 1;
}

static int gpio_index(lua_State *L) {
    gpio_t *gpio;
    const char *field;

    if (!lua_isstring(L, 2))
        return pin_error(L, GPIO_ERROR_ARG, 0, "Error: unknown method or property");

    field = lua_tostring(L, 2);

    lua_getmetatable(L, 1);
    lua_getfield(L, -1, field);
    if (!lua_isnil(L, -1))
        return 1;

    gpio = luaL_checkudata(L, 1, "GPIO");

    if (strcmp(field, "fd") == 0) {
        lua_pushinteger(L, pin_fd(gpio));
        return 1;
    } else if (strcmp(field, "pin") == 0) {
        lua_pushunsigned(L, pin(gpio));
        return 1;
    } else if (strcmp(field, "supports_interrupts") == 0) {
        bool supported;
        int ret;

        if ((ret = pin_interrupts(gpio, &supported)) < 0)
            return pin_error(L, ret, pin_errno(gpio), "Error: %s", pin_errmsg(gpio));

        lua_pushboolean(L, supported);
        return 1;
    } else if (strcmp(field, "direction") == 0) {
        gpio_direction_t direction;
        int ret;

        if ((ret = pin_get_direction(gpio, &direction)) < 0)
            return pin_error(L, ret, pin_errno(gpio), "Error: %s", pin_errmsg(gpio));

        switch (direction) {
            case GPIO_DIRECTION_IN: lua_pushstring(L, "in"); break;
            case GPIO_DIRECTION_OUT: lua_pushstring(L, "out"); break;
            default: lua_pushstring(L, "unknown"); break;
        }
        return 1;
    } else if (strcmp(field, "edge") == 0) {
        gpio_edge_t edge;
        int ret;

        if ((ret = pin_get_edge(gpio, &edge)) < 0)
            return pin_error(L, ret, pin_errno(gpio), "Error: %s", pin_errmsg(gpio));

        switch (edge) {
            case GPIO_EDGE_NONE: lua_pushstring(L, "none"); break;
            case GPIO_EDGE_RISING: lua_pushstring(L, "rising"); break;
            case GPIO_EDGE_FALLING: lua_pushstring(L, "falling"); break;
            case GPIO_EDGE_BOTH: lua_pushstring(L, "both"); break;
            default: lua_pushstring(L, "unknown"); break;
        }
        return 1;
    }

    return pin_error(L, GPIO_ERROR_ARG, 0, "Error: unknown property");
}

static int gpio_newindex(lua_State *L) {
    gpio_t *gpio;
    const char *field;

    gpio = luaL_checkudata(L, 1, "GPIO");

    if (!lua_isstring(L, 2))
        return pin_error(L, GPIO_ERROR_ARG, 0, "Error: unknown property");

    field = lua_tostring(L, 2);

    if (strcmp(field, "fd") == 0)
        return pin_error(L, GPIO_ERROR_ARG, 0, "Error: immutable property");
    else if (strcmp(field, "pin") == 0)
        return pin_error(L, GPIO_ERROR_ARG, 0, "Error: immutable property");
    else if (strcmp(field, "supports_interrupts") == 0)
        return pin_error(L, GPIO_ERROR_ARG, 0, "Error: immutable property");
    else if (strcmp(field, "direction") == 0) {
        gpio_direction_t direction;
        int ret;

        const char *value;
        pin_checktype(L, 3, LUA_TSTRING);
        value = lua_tostring(L, 3);

        if (strcmp(value, "in") == 0)
            direction = GPIO_DIRECTION_IN;
        else if (strcmp(value, "out") == 0)
            direction = GPIO_DIRECTION_OUT;
        else if (strcmp(value, "low") == 0)
            direction = GPIO_DIRECTION_OUT_LOW;
        else if (strcmp(value, "high") == 0)
            direction = GPIO_DIRECTION_OUT_HIGH;
        else
            return pin_error(L, GPIO_ERROR_ARG, 0, "Error: invalid direction, should be 'in', 'out', 'low', or 'high'");

        if ((ret = pin_set_direction(gpio, direction)) < 0)
            return pin_error(L, ret, pin_errno(gpio), "Error: %s", pin_errmsg(gpio));

        return 0;
    } else if (strcmp(field, "edge") == 0) {
        gpio_edge_t edge;
        int ret;

        const char *value;
        pin_checktype(L, 3, LUA_TSTRING);
        value = lua_tostring(L, 3);

        if (strcmp(value, "none") == 0)
            edge = GPIO_EDGE_NONE;
        else if (strcmp(value, "rising") == 0)
            edge = GPIO_EDGE_RISING;
        else if (strcmp(value, "falling") == 0)
            edge = GPIO_EDGE_FALLING;
        else if (strcmp(value, "both") == 0)
            edge = GPIO_EDGE_BOTH;
        else
            return pin_error(L, GPIO_ERROR_ARG, 0, "Error: invalid edge, should be 'none', 'rising', 'falling', or 'both'");

        if ((ret = pin_set_edge(gpio, edge)) < 0)
            return pin_error(L, ret, pin_errno(gpio), "Error: %s", pin_errmsg(gpio));

        return 0;
    }

    return pin_error(L, GPIO_ERROR_ARG, 0, "Error: unknown property");
}

static const struct luaL_Reg gpio_method[] = {
    {"close",   gpio_close},
    {"read",    gpio_read},
    {"write",   gpio_write},
    {"poll",    gpio_poll},
    {"__gc",        gpio_close},
    {"__tostring", gpio_tostring},
    {"__index", gpio_index},
    {"__newindex", gpio_newindex},
    {NULL, NULL}
};

/* Init the Lua Robot Library */
LUAMOD_API int luaopen_botlibgpio (lua_State *L) {
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
  lua_pushnumber(L, VERSIONbotlib);
  lua_setfield(L, -2, "version");
  return 1;
}
