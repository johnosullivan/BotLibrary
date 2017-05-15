#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <errno.h>
#include "gpio.h"

#define FILE_PATH_MAX  256

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
/*
* Background reading on the linux kernal gpio pins.
* https://www.kernel.org/doc/Documentation/gpio/sysfs.txt
* http://elinux.org/RPi_GPIO_Code_Samples
*/

const char *pin_errmsg(gpio_t *gpio) { return gpio->error.errmsg; }
int pin_errno(gpio_t *gpio) { return gpio->error.c_errno; }
unsigned int pin(gpio_t *gpio) { return gpio->pin; }
int pin_fd(gpio_t *gpio) { return gpio->fd; }

static int pin_status(struct gpio_handle *pin, int code, int c_errno, const char *args, ...) {
    va_list ap;
    pin->error.c_errno = c_errno;
    va_start(ap, args);
    vsnprintf(pin->error.errmsg, sizeof(pin->error.errmsg), args, ap);
    va_end(ap);
    if (c_errno) {
        char buf[64];
        strerror_r(c_errno, buf, sizeof(buf));
        snprintf(pin->error.errmsg+strlen(pin->error.errmsg), sizeof(pin->error.errmsg)-strlen(pin->error.errmsg), ": %s error# %d", buf, c_errno);
    }
    return code;
}
//Opens the pin with direction
int pin_open(gpio_t *gpio, unsigned int pin, gpio_direction_t direction) {
    //Check if valid direction
    if (direction != GPIO_DIRECTION_IN && direction != GPIO_DIRECTION_OUT && direction != GPIO_DIRECTION_OUT_LOW && direction != GPIO_DIRECTION_OUT_HIGH && direction != GPIO_DIRECTION_PRESERVE) {
      //Not valid direction
      return pin_status(gpio, GPIO_ERROR_ARG, 0, "Invalid Direction!");
    }
    //GPIO path char
    char gpio_path[FILE_PATH_MAX];
    //File
    struct stat stat_buf;
    char buf[16];
    int fd;
    /*
    * http://pubs.opengroup.org/onlinepubs/7908799/xsh/open.html
    * http://pubs.opengroup.org/onlinepubs/009695399/functions/stat.html
    * Background on the open operatation
    */
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d", pin);
    if (stat(gpio_path, &stat_buf) < 0) {
        snprintf(buf, sizeof(buf), "%d", pin);
        if ((fd = open("/sys/class/gpio/export", O_WRONLY)) < 0) {
          return pin_status(gpio, GPIO_ERROR_EXPORT, errno, "Exporting GPIO: opening 'export'");
        }
        if (write(fd, buf, strlen(buf)+1) < 0) {
          int errsv = errno;
          close(fd);
          return pin_status(gpio, GPIO_ERROR_EXPORT, errsv, "Exporting GPIO: writing 'export'");
        }
        if (close(fd) < 0) {
          //Closing pin
          return pin_status(gpio, GPIO_ERROR_EXPORT, errno, "Exporting GPIO: closing 'export'");
        }
        if (stat(gpio_path, &stat_buf) < 0) {
          //Check pin direction exists
          return pin_status(gpio, GPIO_ERROR_EXPORT, errno, "Exporting GPIO: stat 'gpio%d/'", pin);
        }
    }
    // Not preserving  direction
    if (direction != GPIO_DIRECTION_PRESERVE) {
        //Writing the direction
        snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/direction", pin);
        if ((fd = open(gpio_path, O_WRONLY)) < 0) {
          //Opening
          return pin_status(gpio, GPIO_ERROR_SET_DIRECTION, errno, "Configuring GPIO: opening 'direction'");
        }
        if (write(fd, gpio_direction_to_string[direction], strlen(gpio_direction_to_string[direction])+1) < 0) {
          int errsv = errno;
          close(fd);
          return pin_status(gpio, GPIO_ERROR_SET_DIRECTION, errsv, "Configuring GPIO: writing 'direction'");
        }
        if (close(fd) < 0) {
          //Close
          return pin_status(gpio, GPIO_ERROR_SET_DIRECTION, errno, "Configuring GPIO: closing 'direction'");
        }
    }
    //Copy characters
    memset(gpio, 0, sizeof(struct gpio_handle));
    gpio->pin = pin;
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/value", pin);
    if ((gpio->fd = open(gpio_path, O_RDWR)) < 0) {
      //Opening the pin
      return pin_status(gpio, GPIO_ERROR_OPEN, errno, "Opening GPIO 'gpio%d/value'", pin);
    }
    return 0;
}
//Closes the pin
int pin_close(gpio_t *gpio) {
  if (gpio->fd < 0) {
    return 0;
  }
  /* Close fd */
  if (close(gpio->fd) < 0) {
    //Closes pin
    return pin_status(gpio, GPIO_ERROR_CLOSE, errno, "Closing GPIO 'value'");
  }
  gpio->fd = -1;
  return 0;
}
//Read the current value at pin
int pin_read(gpio_t *gpio, bool *value) {
  char buf[2];
  if (read(gpio->fd, buf, 2) < 0) {
      return pin_status(gpio, GPIO_ERROR_IO, errno, "Reading GPIO 'value'");
  }
  /* Rewind */
  if (lseek(gpio->fd, 0, SEEK_SET) < 0) {
      return pin_status(gpio, GPIO_ERROR_IO, errno, "Rewinding GPIO 'value'");
  }

  if (buf[0] == '0') {
      *value = false;
  } else if (buf[0] == '1') {
      *value = true;
  } else {
      return pin_status(gpio, GPIO_ERROR_IO, 0, "Unknown value");
  }
  return 0;
}
//Writes to pin in the gpio object
int pin_write(gpio_t *gpio, bool value) {
  char outputs[][2] = {"0", "1"};
  //Writes the value to the pin
  if (write(gpio->fd, outputs[value], 2) < 0) {
    return pin_status(gpio, GPIO_ERROR_IO, errno, "Writing GPIO 'value'");
  }
  if (lseek(gpio->fd, 0, SEEK_SET) < 0) {
    return pin_status(gpio, GPIO_ERROR_IO, errno, "Rewinding GPIO 'value'");
  }
  return 0;
}
int pin_poll(gpio_t *gpio, int timeout_ms) {
    struct pollfd fds[1];
    int ret;
    /* Seek to end */
    if (lseek(gpio->fd, 0, SEEK_END) < 0) {
      return pin_status(gpio, GPIO_ERROR_IO, errno, "Seeking to end of GPIO 'value'");
    }
    /* Poll */
    fds[0].fd = gpio->fd;
    fds[0].events = POLLPRI | POLLERR;
    if ((ret = poll(fds, 1, timeout_ms)) < 0) {
        return pin_status(gpio, GPIO_ERROR_IO, errno, "Polling GPIO 'value'");
    }
    /* GPIO edge interrupt occurred */
    if (ret) {
        /* Rewind */
        if (lseek(gpio->fd, 0, SEEK_SET) < 0) {
            return pin_status(gpio, GPIO_ERROR_IO, errno, "Rewinding GPIO 'value'");
        }
        return 1;
    }
    /* Timed out */
    return 0;
}

int pin_set_direction(gpio_t *gpio, gpio_direction_t direction) {
    char gpio_path[FILE_PATH_MAX];
    int fd;
    if (direction != GPIO_DIRECTION_IN && direction != GPIO_DIRECTION_OUT && direction != GPIO_DIRECTION_OUT_LOW && direction != GPIO_DIRECTION_OUT_HIGH) {
        return pin_status(gpio, GPIO_ERROR_ARG, 0, "Invalid Direction");
    }
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/direction", gpio->pin);
    if ((fd = open(gpio_path, O_WRONLY)) < 0) {
        return pin_status(gpio, GPIO_ERROR_SET_DIRECTION, errno, "Opening GPIO 'direction'");
    }
    if (write(fd, gpio_direction_to_string[direction], strlen(gpio_direction_to_string[direction])+1) < 0) {
        int errsv = errno;
        close(fd);
        return pin_status(gpio, GPIO_ERROR_SET_DIRECTION, errsv, "Writing GPIO 'direction'");
    }
    if (close(fd) < 0) {
        return pin_status(gpio, GPIO_ERROR_SET_DIRECTION, errno, "Closing GPIO 'direction'");
    }
    return 0;
}

int pin_get_direction(gpio_t *gpio, gpio_direction_t *direction) {
    char gpio_path[FILE_PATH_MAX];
    char buf[8];
    int fd, ret;
    /* Read direction */
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/direction", gpio->pin);
    if ((fd = open(gpio_path, O_RDONLY)) < 0) {
        return pin_status(gpio, GPIO_ERROR_GET_DIRECTION, errno, "Opening GPIO 'direction'");
    }
    if ((ret = read(fd, buf, sizeof(buf))) < 0) {
        int errsv = errno;
        close(fd);
        return pin_status(gpio, GPIO_ERROR_GET_DIRECTION, errsv, "Writing GPIO 'direction'");
    }
    if (close(fd) < 0) {
        return pin_status(gpio, GPIO_ERROR_GET_DIRECTION, errno, "Closing GPIO 'direction'");
    }
    buf[ret] = '\0';
    if (strcmp(buf, "in\n") == 0) {
        *direction = GPIO_DIRECTION_IN;
    } else if (strcmp(buf, "out\n") == 0) {
        *direction = GPIO_DIRECTION_OUT;
    } else {
        return pin_status(gpio, GPIO_ERROR_GET_DIRECTION, 0, "Unknown Direction");
    }
    return 0;
}

int pin_interrupts(gpio_t *gpio, bool *supported) {
    char gpio_path[FILE_PATH_MAX];
    struct stat stat_buf;
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/edge", gpio->pin);
    if (stat(gpio_path, &stat_buf) < 0) {
        if (errno == ENOENT) {
            *supported = false;
            return 0;
        }
        return pin_status(gpio, GPIO_ERROR_IO, errno, "Exporting GPIO: stat 'gpio%d/edge'", gpio->pin);
    }
    *supported = true;
    return 0;
}

int pin_set_edge(gpio_t *gpio, gpio_edge_t edge) {
    char gpio_path[FILE_PATH_MAX];
    int fd;
    if (edge != GPIO_EDGE_NONE && edge != GPIO_EDGE_RISING && edge != GPIO_EDGE_FALLING && edge != GPIO_EDGE_BOTH) {
        return pin_status(gpio, GPIO_ERROR_ARG, 0, "Invalid Edge");
    }
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/edge", gpio->pin);
    if ((fd = open(gpio_path, O_WRONLY)) < 0)
        return pin_status(gpio, GPIO_ERROR_SET_EDGE, errno, "Opening GPIO 'edge'");
    if (write(fd, gpio_edge_to_string[edge], strlen(gpio_edge_to_string[edge])+1) < 0) {
        int errsv = errno;
        close(fd);
        return pin_status(gpio, GPIO_ERROR_SET_EDGE, errsv, "Writing GPIO 'edge'");
    }
    if (close(fd) < 0) {
        return pin_status(gpio, GPIO_ERROR_SET_EDGE, errno, "Closing GPIO 'edge'");
    }
    return 0;
}

int pin_get_edge(gpio_t *gpio, gpio_edge_t *edge) {
    char gpio_path[FILE_PATH_MAX];
    char buf[16];
    int fd, ret;
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/edge", gpio->pin);
    if ((fd = open(gpio_path, O_RDONLY)) < 0)
        return pin_status(gpio, GPIO_ERROR_GET_EDGE, errno, "Opening GPIO 'edge'");
    if ((ret = read(fd, buf, sizeof(buf))) < 0) {
        int errsv = errno;
        close(fd);
        return pin_status(gpio, GPIO_ERROR_GET_EDGE, errsv, "Writing GPIO 'edge'");
    }
    if (close(fd) < 0) {
        return pin_status(gpio, GPIO_ERROR_GET_EDGE, errno, "Closing GPIO 'edge'");
    }
    buf[ret] = '\0';
    if (strcmp(buf, "none\n") == 0) {
        *edge = GPIO_EDGE_NONE;
    } else if (strcmp(buf, "rising\n") == 0) {
        *edge = GPIO_EDGE_RISING;
    } else if (strcmp(buf, "falling\n") == 0) {
        *edge = GPIO_EDGE_FALLING;
    } else if (strcmp(buf, "both\n") == 0) {
        *edge = GPIO_EDGE_BOTH;
    } else {
        return pin_status(gpio, GPIO_ERROR_GET_EDGE, 0, "Unknown Edge");
    }
    return 0;
}
//Convert pin to string
int pin_tostring(gpio_t *gpio, char *str, size_t len) {
    gpio_direction_t direction;
    const char *direction_str;
    gpio_edge_t edge;
    const char *edge_str;
    if (pin_get_direction(gpio, &direction) < 0) {
        direction_str = "?";
    } else {
        direction_str = gpio_direction_to_string[direction];
    }
    if (pin_get_edge(gpio, &edge) < 0) {
        edge_str = "?";
    } else {
        edge_str = gpio_edge_to_string[edge];
    }
    return snprintf(str, len, "GPIO %d (fd=%d, direction=%s, edge=%s)", gpio->pin, gpio->fd, direction_str, edge_str);
}
