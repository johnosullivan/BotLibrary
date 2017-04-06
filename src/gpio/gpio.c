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
*/
//Opens the pin with direction
int pin_open(gpio_t *gpio, unsigned int pin, gpio_direction_t direction) {
    //Check if valid direction
    if (direction != GPIO_DIRECTION_IN && direction != GPIO_DIRECTION_OUT && direction != GPIO_DIRECTION_OUT_LOW && direction != GPIO_DIRECTION_OUT_HIGH && direction != GPIO_DIRECTION_PRESERVE) {
      //Not valid direction
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
          //Opening the pin
          return 0;
        }
        if (write(fd, buf, strlen(buf)+1) < 0) {
          close(fd);
          //Wrote to pin
          return 0;
        }
        if (close(fd) < 0) {
          //Closing pin
          return 0;
        }
        if (stat(gpio_path, &stat_buf) < 0) {
          //Check pin direction exists
          return 0;
        }
    }
    //Copy characters
    memset(gpio, 0, sizeof(struct gpio_handle));
    gpio->pin = pin;
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/value", pin);
    if ((gpio->fd = open(gpio_path, O_RDWR)) < 0) {
      //Opening the pin
      return 0;
    }
    return 0;
}
//Read the current value at pin
int pin_read(gpio_t *gpio, bool *value) {
  return 0;
}
//Writes to pin in the gpio object
int pin_write(gpio_t *gpio, bool value) {
  char outputs[][2] = {"0", "1"};
  //Writes the value to the pin
  if (write(gpio->fd, outputs[value], 2) < 0) {
    return 0;
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
    return 0;
  }
  gpio->fd = -1;
  return 0;
}
