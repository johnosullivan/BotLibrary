#ifndef _BOTLIB_I2C_H
#define _BOTLIB_I2C_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#ifdef __APPLE__
#include <time.h>
#elif __linux__
#include <sys/time.h>
#include <linux/i2c-dev.h>
#endif
//#include <linux/i2c.h>

enum i2c_error_code {
    I2C_ERROR_ARG               = -1, /* Invalid arguments */
    I2C_ERROR_OPEN              = -2, /* Opening I2C device */
    I2C_ERROR_QUERY_SUPPORT     = -3, /* Querying I2C support on I2C device */
    I2C_ERROR_NOT_SUPPORTED     = -4, /* I2C not suppoted on this device */
    I2C_ERROR_TRANSFER          = -5, /* I2C transfer */
    I2C_ERROR_CLOSE             = -6, /* Closing I2C device */
};

typedef struct i2c_handle {
    int fd;

    struct {
        int c_errno;
        char errmsg[96];
    } error;
} i2c_t;

#ifdef __linux__
int i2c_open(i2c_t *i2c, const char *path);
int i2c_transfer(i2c_t *i2c, struct i2c_msg *msgs, size_t count);
int i2c_close(i2c_t *i2c);
int i2c_fd(i2c_t *i2c);
int i2c_tostring(i2c_t *i2c, char *str, size_t len);
int i2c_errno(i2c_t *i2c);
const char *i2c_errmsg(i2c_t *i2c);
#endif


#endif
