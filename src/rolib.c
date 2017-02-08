#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <time.h>
#include <unistd.h>

LUAMOD_API int luaopen_rolibcore (lua_State *L);
LUAMOD_API int luaopen_rolibgpio (lua_State *L);
LUAMOD_API int luaopen_rolibsensor (lua_State *L);
LUAMOD_API int luaopen_rolibservo (lua_State *L);


static int rolib_error_tostring(lua_State *L) {
    lua_getfield(L, -1, "message");
    return 1;
}

LUALIB_API int luaopen_rolib(lua_State *L) {

    luaL_newmetatable(L, "rolib.error");
    lua_pushcclosure(L, rolib_error_tostring, 0);
    lua_setfield(L, -2, "__tostring");
    lua_pop(L, 1);
    lua_newtable(L);

    luaopen_rolibcore(L);
    lua_setfield(L, -2, "Core");
    luaopen_rolibgpio(L);
    lua_setfield(L, -2, "GPIO");
    luaopen_rolibsensor(L);
    lua_setfield(L, -2, "Sensor");

    /*
    luaopen_rolibservo(L);
    lua_setfield(L, -2, "Servo");
    */

    return 1;
}
