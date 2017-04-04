//https://www.raspberrypi.org/documentation/usage/gpio-plus-and-raspi2/

#ifndef _ROLIB_GPIO_H
#define _ROLIB_GPIO_H

//variables types and bool library
#include <stddef.h>
#include <stdbool.h>

enum gpio_error_code {
    GPIO_ERROR_ARG = -1,
    GPIO_ERROR_EXPORT = -2,
    GPIO_ERROR_OPEN = -3,
    GPIO_ERROR_IO = -4,
    GPIO_ERROR_CLOSE = -5,
    GPIO_ERROR_SET_DIRECTION = -6,
    GPIO_ERROR_GET_DIRECTION = -7,
    GPIO_ERROR_SET_EDGE = -8,
    GPIO_ERROR_GET_EDGE = -9,
};

typedef struct gpio_handle {
    unsigned int pin;
    int fd;

    struct {
        int c_errno;
        char errmsg[96];
    } error;
} gpio_t;

typedef enum gpio_edge {
    GPIO_EDGE_NONE,
    GPIO_EDGE_RISING,
    GPIO_EDGE_FALLING,
    GPIO_EDGE_BOTH
} gpio_edge_t;

typedef enum gpio_direction {
    GPIO_DIRECTION_IN,
    GPIO_DIRECTION_OUT,
    GPIO_DIRECTION_OUT_LOW,
    GPIO_DIRECTION_OUT_HIGH,
    GPIO_DIRECTION_PRESERVE,
} gpio_direction_t;

//Opens the gpio pin and set direction
int pin_open(gpio_t *gpio, unsigned int pin, gpio_direction_t direction);
//Read the current value at pin
int pin_read(gpio_t *gpio, bool *value);
//Writes to pin in the gpio object
int pin_write(gpio_t *gpio, bool value);
//Closes the pin
int pin_close(gpio_t *gpio);

#endif
