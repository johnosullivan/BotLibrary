#include "WAPIRS.h"

sensor_t *config_WAPIRS(int output) {
  gpio_t gpio_in;
  if (!DEBUG) {
    if (pin_open(&gpio_in, output, GPIO_DIRECTION_IN) < 0) {
      fprintf(stderr, "pin_open: %s\n", pin_errmsg(&gpio_in));
    }
  }
  return sensor_create(gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in);
}
bool read_WAPIRS(sensor_t *sensor_object) {
  bool output = false;
  if (!DEBUG) {
    gpio_t gpio_in = sensor_get_pin(sensor_object,1);
    if (pin_read(&gpio_in, &output) < 0) {
      fprintf(stderr, "pin_read: %s\n", pin_errmsg(&gpio_in));
    }
  } else {
    output = false;
  }
  return output;
}
void destroy_WAPIRS(sensor_t *sensor_object) {
  if (!DEBUG) {
    gpio_t gpio_in = sensor_get_pin(sensor_object,1);
    pin_close(&gpio_in);
  }
}
char*  tostring_WAPIRS(sensor_userdata_t *sensor_data) {
  char *tostring_output;
  if (!DEBUG) {
    asprintf(&tostring_output, "WAPIRS - Wide Angle PIR Sensor\nName: %s CallBack: %s \nOutput: %d", sensor_data->name, sensor_data->callback, sensor_get_pin(sensor_data->s,1).pin);
  } else {
    asprintf(&tostring_output, "WAPIRS - Wide Angle PIR Sensor\nName: %s CallBack: %s \nOutput: 12", sensor_data->name, sensor_data->callback);
  }
  return tostring_output;
}
char* info_WAPIRS() {
  return "WAPIRS - Wide Angle PIR Sensor\n180˚ detection angle for widespread motion detection\nTypically detects people up to 30 ft away\n4-pin SIP package perfect for breadboard-friendly projects\n";
}
