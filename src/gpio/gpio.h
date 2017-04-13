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
//Type of detections for edges
typedef enum gpio_edge {
    GPIO_EDGE_NONE,
    GPIO_EDGE_RISING,
    GPIO_EDGE_FALLING,
    GPIO_EDGE_BOTH
} gpio_edge_t;
//Type of pin directions
typedef enum gpio_direction {
    GPIO_DIRECTION_IN,
    GPIO_DIRECTION_OUT,
    GPIO_DIRECTION_OUT_LOW,
    GPIO_DIRECTION_OUT_HIGH,
    GPIO_DIRECTION_PRESERVE,
} gpio_direction_t;


int pin_open(gpio_t *gpio, unsigned int pin, gpio_direction_t direction);
int pin_read(gpio_t *gpio, bool *value);
int pin_write(gpio_t *gpio, bool value);
int pin_close(gpio_t *gpio);
int pin_poll(gpio_t *gpio, int timeout_ms);
int pin_interrupts(gpio_t *gpio, bool *supported);
int pin_get_direction(gpio_t *gpio, gpio_direction_t *direction);
int pin_get_edge(gpio_t *gpio, gpio_edge_t *edge);
int pin_set_direction(gpio_t *gpio, gpio_direction_t direction);
int pin_set_edge(gpio_t *gpio, gpio_edge_t edge);
unsigned int pin(gpio_t *gpio);
int pin_fd(gpio_t *gpio);
int pin_tostring(gpio_t *gpio, char *str, size_t len);
int pin_errno(gpio_t *gpio);
const char *pin_errmsg(gpio_t *gpio);

#endif
