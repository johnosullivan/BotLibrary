

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include "i2c.h"

#define lua_tounsignedx(L,i,is) ((lua_Unsigned)lua_tointegerx(L,i,is))
#define lua_tounsigned(L,i)     lua_tounsignedx(L,(i),NULL)

/* Define a new error for malloc() required in read/write */
#define I2C_ERROR_ALLOC    (I2C_ERROR_CLOSE-1)

static const char *i2c_error_code_strings[] = {
    [-I2C_ERROR_ARG]            = "I2C_ERROR_ARG",
    [-I2C_ERROR_OPEN]           = "I2C_ERROR_OPEN",
    [-I2C_ERROR_QUERY_SUPPORT]  = "I2C_ERROR_QUERY_SUPPORT",
    [-I2C_ERROR_NOT_SUPPORTED]  = "I2C_ERROR_NOT_SUPPORTED",
    [-I2C_ERROR_TRANSFER]       = "I2C_ERROR_TRANSFER",
    [-I2C_ERROR_CLOSE]          = "I2C_ERROR_CLOSE",
    [-I2C_ERROR_ALLOC]          = "I2C_ERROR_ALLOC",
};

static int lua_i2c_error(lua_State *L, enum i2c_error_code code, int c_errno, const char *fmt, ...) {
    char message[128];
    va_list ap;

    va_start(ap, fmt);

    /* Create error table */
    lua_newtable(L);
    /* .code string */
    lua_pushstring(L, i2c_error_code_strings[-code]);
    lua_setfield(L, -2, "code");
    /* .c_errno number */
    lua_pushinteger(L, c_errno);
    lua_setfield(L, -2, "c_errno");
    /* .message string */
    vsnprintf(message, sizeof(message), fmt, ap);
    lua_pushstring(L, message);
    lua_setfield(L, -2, "message");

    va_end(ap);

    /* Set error metatable on it */
    luaL_getmetatable(L, "botlib.error");
    lua_setmetatable(L, -2);

    return lua_error(L);
}
#ifdef __linux__
static void lua_i2c_checktype(lua_State *L, int index, int type) {
    if (lua_type(L, index) != type)
        lua_i2c_error(L, I2C_ERROR_ARG, 0, "Error: invalid argument #%d (%s expected, got %s)", index, lua_typename(L, type), lua_typename(L, lua_type(L, index)));
}

static int lua_i2c_open(lua_State *L) {
    #ifdef __linux__
    i2c_t *i2c;
    const char *device;
    int ret;

    i2c = luaL_checkudata(L, 1, "BotLib.I2C");

    /* Initialize file descriptor to an invalid value, in case an error occurs
     * below and gc later close()'s this object. */
    i2c->fd = -1;

    /* Arguments passed in table form */
    if (lua_istable(L, 2)) {
        lua_getfield(L, 2, "device");
        if (!lua_isstring(L, -1))
            return lua_i2c_error(L, I2C_ERROR_ARG, 0, "Error: invalid type of table argument 'device', should be string");

        device = lua_tostring(L, -1);

    /* Arguments passed normally */
    } else {
        lua_i2c_checktype(L, 2, LUA_TSTRING);

        device = lua_tostring(L, 2);
    }

    if ((ret = i2c_open(i2c, device)) < 0)
        return lua_i2c_error(L, ret, i2c_errno(i2c), i2c_errmsg(i2c));

    #endif
    return 0;
}
#endif

static int lua_i2c_new(lua_State *L) {
    #ifdef __linux__
    /* Remove self table object */
    lua_remove(L, 1);

    /* Create handle userdata */
    lua_newuserdata(L, sizeof(i2c_t));
    /* Set I2C metatable on it */
    luaL_getmetatable(L, "BotLib.I2C");
    lua_setmetatable(L, -2);
    /* Move userdata to the beginning of the stack */
    lua_insert(L, 1);

    /* Call open */
    lua_i2c_open(L);

    /* Leave only userdata on the stack */
    lua_settop(L, 1);

    #endif

    return 1;
}
#ifdef __linux__
static void _free_i2c_msgs(struct i2c_msg *i2c_msgs, unsigned int num_msgs) {
    unsigned int i;

    for (i = 0; i < num_msgs; i++) {
        if (i2c_msgs[i].buf != NULL)
            free(i2c_msgs[i].buf);
    }
    free(i2c_msgs);
}
#endif
static int lua_i2c_transfer(lua_State *L) {
    #ifdef __linux__
    i2c_t *i2c;
    struct i2c_msg *i2c_msgs;
    unsigned int num_msgs;
    uint16_t i2c_addr;
    unsigned int i, j;
    int ret;

    i2c = luaL_checkudata(L, 1, "BotLib.I2C");
    lua_i2c_checktype(L, 2, LUA_TNUMBER);
    lua_i2c_checktype(L, 3, LUA_TTABLE);

    i2c_addr = lua_tounsigned(L, 2);
    num_msgs = luaL_len(L, 3);

    if ((i2c_msgs = malloc(num_msgs * sizeof(struct i2c_msg))) == NULL)
        return lua_i2c_error(L, I2C_ERROR_ALLOC, errno, "Error: allocating memory for i2c messages");
    memset(i2c_msgs, 0, num_msgs * sizeof(struct i2c_msg));

    /* Convert transfer table to struct i2c_msg array */
    /* e.g. { {0xf0, 0xaa}, {0x00, 0x00, .flags = i2c.I2C_M_READ} } */
    for (i = 0; i < num_msgs; i++) {
        unsigned int msg_len, msg_flags;

        lua_pushinteger(L, i+1);
        lua_gettable(L, -2);
        /* Check message table is a table with length > 0 */
        if (!lua_istable(L, -1) || luaL_len(L, -1) == 0) {
            _free_i2c_msgs(i2c_msgs, num_msgs);
            return lua_i2c_error(L, I2C_ERROR_ARG, 0, "Error: invalid message index %d of transfer table.", i+1);
        }

        /* Get message length */
        msg_len = luaL_len(L, -1);

        /* Get message flags */
        lua_getfield(L, -1, "flags");
        if (lua_isnil(L, -1)) {
            msg_flags = 0;
        } else if (!lua_isnumber(L, -1)) {
            _free_i2c_msgs(i2c_msgs, num_msgs);
            return lua_i2c_error(L, I2C_ERROR_ARG, 0, "Error: invalid message flags in message index %d of transfer table.", i+1);
        } else {
            msg_flags = lua_tounsigned(L, -1);
        }
        /* Pop message flags */
        lua_pop(L, 1);

        /* Prepare i2c message */
        i2c_msgs[i].addr = i2c_addr;
        i2c_msgs[i].flags = msg_flags;
        i2c_msgs[i].len = msg_len;
        if (msg_len > 0) {
            /* Allocate memory for message data */
            if ((i2c_msgs[i].buf = malloc(msg_len)) == NULL) {
                _free_i2c_msgs(i2c_msgs, num_msgs);
                return lua_i2c_error(L, I2C_ERROR_ALLOC, errno, "Error: allocating memory for message data");
            }

            /* Extract message data from table */
            for (j = 0; j < msg_len; j++) {
                lua_pushinteger(L, j+1);
                lua_gettable(L, -2);
                /* Check message data is an integer */
                if (!lua_isnumber(L, -1)) {
                    _free_i2c_msgs(i2c_msgs, num_msgs);
                    return lua_i2c_error(L, I2C_ERROR_ARG, 0, "Error: invalid message data %d in message index %d of transfer table");
                }

                i2c_msgs[i].buf[j] = lua_tounsigned(L, -1);

                /* Pop message data */
                lua_pop(L, 1);
            }
        }

        /* Pop message table */
        lua_pop(L, 1);
    }

    /* Make I2C transfer */
    if ((ret = i2c_transfer(i2c, i2c_msgs, num_msgs)) < 0) {
        _free_i2c_msgs(i2c_msgs, num_msgs);
        return lua_i2c_error(L, ret, i2c_errno(i2c), "Error: %s", i2c_errmsg(i2c));
    }

    /* Update message tables in transfer table with read data */
    for (i = 0; i < num_msgs; i++) {
        if (i2c_msgs[i].flags & I2C_M_RD) {
            /* Get message table at this index */
            lua_pushinteger(L, i+1);
            lua_gettable(L, -2);

            /* For each byte of the read message, update the message table */
            for (j = 0; j < i2c_msgs[i].len; j++) {
                lua_pushinteger(L, j+1);
                lua_pushinteger(L, i2c_msgs[i].buf[j]);
                lua_settable(L, -3);
            }

            /* Pop message table */
            lua_pop(L, 1);
        }
    }

    _free_i2c_msgs(i2c_msgs, num_msgs);
    #endif
    return 1;
}

static int lua_i2c_close(lua_State *L) {
    #ifdef __linux__
    i2c_t *i2c;
    int ret;

    i2c = luaL_checkudata(L, 1, "BotLib.I2C");

    if ((ret = i2c_close(i2c)) < 0)
        return lua_i2c_error(L, ret, i2c_errno(i2c), "Error: %s", i2c_errmsg(i2c));


    #endif
    return 0;
}

static int lua_i2c_tostring(lua_State *L) {
    #ifdef __linux__
    i2c_t *i2c;
    char i2c_str[128];

    i2c = luaL_checkudata(L, 1, "BotLib.I2C");

    i2c_tostring(i2c, i2c_str, sizeof(i2c_str));

    lua_pushstring(L, i2c_str);


    #endif
    return 1;
}

static int lua_i2c_index(lua_State *L) {
    #ifdef __linux__
    i2c_t *i2c;
    const char *field;

    if (!lua_isstring(L, 2))
        return lua_i2c_error(L, I2C_ERROR_ARG, 0, "Error: unknown method or property");

    field = lua_tostring(L, 2);

    /* Look up method in metatable */
    lua_getmetatable(L, 1);
    lua_getfield(L, -1, field);
    if (!lua_isnil(L, -1))
        return 1;

    i2c = luaL_checkudata(L, 1, "BotLib.I2C");

    if (strcmp(field, "fd") == 0) {
        lua_pushinteger(L, i2c_fd(i2c));
        return 1;
    }
    #endif

    return lua_i2c_error(L, I2C_ERROR_ARG, 0, "Error: unknown property");
}

static int lua_i2c_newindex(lua_State *L) {
    #ifdef __linux__
    const char *field;

    if (!lua_isstring(L, 2))
        return lua_i2c_error(L, I2C_ERROR_ARG, 0, "Error: unknown property");

    field = lua_tostring(L, 2);

    if (strcmp(field, "fd") == 0)
        return lua_i2c_error(L, I2C_ERROR_ARG, 0, "Error: immutable property");


    #endif

    return lua_i2c_error(L, I2C_ERROR_ARG, 0, "Error: unknown property");
}

static const struct luaL_Reg periphery_i2c_m[] = {
    {"close", lua_i2c_close},
    {"transfer", lua_i2c_transfer},
    {"__gc", lua_i2c_close},
    {"__tostring", lua_i2c_tostring},
    {"__index", lua_i2c_index},
    {"__newindex", lua_i2c_newindex},
    {NULL, NULL}
};

LUALIB_API int luaopen_botlibi2c(lua_State *L) {
    /* Create BotLib.I2C metatable */
    luaL_newmetatable(L, "BotLib.I2C");
    /* Set metatable functions */
    const struct luaL_Reg *funcs = (const struct luaL_Reg *)periphery_i2c_m;
    for (; funcs->name != NULL; funcs++) {
        lua_pushcclosure(L, funcs->func, 0);
        lua_setfield(L, -2, funcs->name);
    }
    /* Set metatable properties */
    lua_pushstring(L, "protected metatable");
    lua_setfield(L, -2, "__metatable");

    /* Create {__call = lua_i2c_new, __metatable = "protected metatable", version = ...} table */
    lua_newtable(L);
    lua_pushcclosure(L, lua_i2c_new, 0);
    lua_setfield(L, -2, "__call");
    lua_pushstring(L, "protected metatable");
    lua_setfield(L, -2, "__metatable");
    /* Set it as the metatable for the BotLib.I2C metatable */
    lua_setmetatable(L, -2);

    lua_pushstring(L, "1.0");
    lua_setfield(L, -2, "version");

    /* Copy useful message flags from <linux/i2c.h> */
    /* The constants below are available from kernel version 3.2 onwards */
    #ifdef __linux__
    lua_pushinteger(L, I2C_M_TEN);
    lua_setfield(L, -2, "I2C_M_TEN");
    lua_pushinteger(L, I2C_M_RD);
    lua_setfield(L, -2, "I2C_M_RD");
    lua_pushinteger(L, I2C_M_NOSTART);
    lua_setfield(L, -2, "I2C_M_NOSTART");
    lua_pushinteger(L, I2C_M_REV_DIR_ADDR);
    lua_setfield(L, -2, "I2C_M_REV_DIR_ADDR");
    lua_pushinteger(L, I2C_M_IGNORE_NAK);
    lua_setfield(L, -2, "I2C_M_IGNORE_NAK");
    lua_pushinteger(L, I2C_M_NO_RD_ACK);
    lua_setfield(L, -2, "I2C_M_NO_RD_ACK");
    #endif
    /* I2C_M_STOP flag was added in kernel version 3.6 */
    #ifdef I2C_M_STOP
    lua_pushinteger(L, I2C_M_STOP);
    lua_setfield(L, -2, "I2C_M_STOP");
    #endif

    return 1;
}
