#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#ifdef __linux__
#include <sys/ioctl.h>
#endif

#include "i2c.h"

#ifdef __linux__

static int _i2c_error(struct i2c_handle *i2c, int code, int c_errno, const char *fmt, ...) {
    #ifdef __linux__
    va_list ap;
    i2c->error.c_errno = c_errno;
    va_start(ap, fmt);
    vsnprintf(i2c->error.errmsg, sizeof(i2c->error.errmsg), fmt, ap);
    va_end(ap);
    if (c_errno) {
        char buf[64];
        strerror_r(c_errno, buf, sizeof(buf));
        snprintf(i2c->error.errmsg+strlen(i2c->error.errmsg), sizeof(i2c->error.errmsg)-strlen(i2c->error.errmsg), ": %s [errno %d]", buf, c_errno);
    }
    #endif
    return code;
}

int i2c_open(i2c_t *i2c, const char *path) {
    #ifdef __linux__
    uint32_t supported_funcs;
    memset(i2c, 0, sizeof(struct i2c_handle));
    if ((i2c->fd = open(path, O_RDWR)) < 0) {
        return _i2c_error(i2c, I2C_ERROR_OPEN, errno, "Opening I2C device \"%s\"", path);
    }
    if (ioctl(i2c->fd, I2C_FUNCS, &supported_funcs) < 0) {
        int errsv = errno;
        close(i2c->fd);
        return _i2c_error(i2c, I2C_ERROR_QUERY_SUPPORT, errsv, "Querying I2C_FUNCS");
    }
    if (!(supported_funcs & I2C_FUNC_I2C)) {
        close(i2c->fd);
        return _i2c_error(i2c, I2C_ERROR_NOT_SUPPORTED, 0, "I2C not supported on %s", path);
    }
    #endif
    return 0;
}

int i2c_transfer(i2c_t *i2c, struct i2c_msg *msgs, size_t count) {
    #ifdef __linux__
    struct i2c_rdwr_ioctl_data i2c_rdwr_data;
    memset(&i2c_rdwr_data, 0, sizeof(struct i2c_rdwr_ioctl_data));
    i2c_rdwr_data.msgs = msgs;
    i2c_rdwr_data.nmsgs = count;
    if (ioctl(i2c->fd, I2C_RDWR, &i2c_rdwr_data) < 0) {
        return _i2c_error(i2c, I2C_ERROR_TRANSFER, errno, "I2C transfer");
    }
    #endif
    return 0;
}

int i2c_close(i2c_t *i2c) {
    #ifdef __linux__
    if (i2c->fd < 0) {
        return 0;
    }
    if (close(i2c->fd) < 0) {
        return _i2c_error(i2c, I2C_ERROR_CLOSE, errno, "Closing I2C device");
    }
    i2c->fd = -1;
    #endif
    return 0;
}

int i2c_tostring(i2c_t *i2c, char *str, size_t len) {
    return snprintf(str, len, "I2C (fd=%d)", i2c->fd);
}

const char *i2c_errmsg(i2c_t *i2c) {
    return i2c->error.errmsg;
}

int i2c_errno(i2c_t *i2c) {
    return i2c->error.c_errno;
}

int i2c_fd(i2c_t *i2c) {
    return i2c->fd;
}

#endif
