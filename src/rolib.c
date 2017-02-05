#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <time.h>
#include <unistd.h>

LUAMOD_API int luaopen_rolibcore (lua_State *L);
LUAMOD_API int luaopen_rolibgpio (lua_State *L);
LUAMOD_API int luaopen_rolibsensor (lua_State *L);
LUAMOD_API int luaopen_rolibservo (lua_State *L);

static int s_sleep(lua_State *L) {

    return 0;
}

static int sleep_ms(lua_State *L) {

    return 0;
}

static int sleep_us(lua_State *L) {

    return 0;
}

LUALIB_API int luaopen_rolib(lua_State *L) {
    lua_newtable(L);
    luaopen_rolibcore(L);
    lua_setfield(L, -2, "Core");
    luaopen_rolibgpio(L);
    lua_setfield(L, -2, "GPIO");
    luaopen_rolibsensor(L);
    lua_setfield(L, -2, "Sensor");
    luaopen_rolibservo(L);
    lua_setfield(L, -2, "Servo");

    lua_pushcclosure(L, s_sleep, 0);
    lua_setfield(L, -2, "sleep");
    lua_pushcclosure(L, sleep_ms, 0);
    lua_setfield(L, -2, "sleep_ms");
    lua_pushcclosure(L, sleep_us, 0);
    lua_setfield(L, -2, "sleep_us");
    return 1;
}
