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

#define PATH_MAX  256

static const char *gpio_direction_to_string[] = {
    [GPIO_DIR_IN]         = "in",
    [GPIO_DIR_OUT]        = "out",
    [GPIO_DIR_OUT_LOW]    = "low",
    [GPIO_DIR_OUT_HIGH]   = "high",
};

static const char *gpio_edge_to_string[] = {
    [GPIO_EDGE_NONE]      = "none",
    [GPIO_EDGE_RISING]    = "rising",
    [GPIO_EDGE_FALLING]   = "falling",
    [GPIO_EDGE_BOTH]      = "both",
};

//Opens the pin with direction
int pin_open(gpio_t *gpio, unsigned int pin, gpio_direction_t direction) {
  return 0;
}
//Read the current value at pin
int pin_read(gpio_t *gpio, bool *value) {
  return 0;
}
//Writes to pin in the gpio object
int pin_write(gpio_t *gpio, bool value) {
  return 0;
}
//Closes the pin
int pin_close(gpio_t *gpio) {
  return 0;
}
