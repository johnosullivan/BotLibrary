#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <time.h>
#include <unistd.h>

LUAMOD_API int luaopen_botlibcore (lua_State *L);
LUAMOD_API int luaopen_botlibgpio (lua_State *L);
LUAMOD_API int luaopen_botlibsensor (lua_State *L);
LUAMOD_API int luaopen_botlibservo (lua_State *L);


static int botlib_error_tostring(lua_State *L) {
    lua_getfield(L, -1, "message");
    return 1;
}

LUALIB_API int luaopen_botlib(lua_State *L) {

    luaL_newmetatable(L, "botlib.error");
    lua_pushcclosure(L, botlib_error_tostring, 0);
    lua_setfield(L, -2, "__tostring");
    lua_pop(L, 1);

    lua_newtable(L);

    luaopen_botlibcore(L);
    lua_setfield(L, -2, "Core");
    luaopen_botlibgpio(L);
    lua_setfield(L, -2, "GPIO");
    /*luaopen_botlibsensor(L);
    lua_setfield(L, -2, "Sensor");
    luaopen_botlibservo(L);
    lua_setfield(L, -2, "Servo");*/


    return 1;
}
