#include "HCSR04.h"

sensor_t *config_HCSR04(int trigger, int echo) {
  gpio_t gpio_in, gpio_out;
  if (!DEBUG) {
    if (pin_open(&gpio_in, echo, GPIO_DIRECTION_IN) < 0) {
      fprintf(stderr, "pin_open: %s\n", pin_errmsg(&gpio_in));
    }
    if (pin_open(&gpio_out, trigger, GPIO_DIRECTION_OUT) < 0) {
      fprintf(stderr, "pin_open: %s\n", pin_errmsg(&gpio_out));
    }
  }
  return sensor_create(gpio_out,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in,gpio_in);
}
double read_HCSRO4(sensor_t *sensor_object) {
  double distance;
  if (!DEBUG) {
    gpio_t gpio_in = sensor_get_pin(sensor_object,2);
    gpio_t gpio_out = sensor_get_pin(sensor_object,1);
    double start, stop;
    if (pin_write(&gpio_out, true) < 0) {
        fprintf(stderr, "pin_write: %s\n", pin_errmsg(&gpio_out));
    }
    sleep((unsigned int)0.00001);
    if (pin_write(&gpio_out, false) < 0) {
        fprintf(stderr, "pin_write: %s\n", pin_errmsg(&gpio_out));
    }
    bool echo = false;
    start = timeout_gettime();
    stop = timeout_gettime();
    while (echo == false) {
      start = timeout_gettime();
      if (pin_read(&gpio_in, &echo) < 0) {
        fprintf(stderr, "pin_read: %s\n", pin_errmsg(&gpio_in));
      }
    };
    while (echo == true) {
      stop = timeout_gettime();
      if (pin_read(&gpio_in, &echo) < 0) {
        fprintf(stderr, "pin_read: %s\n", pin_errmsg(&gpio_in));
      }
    }
    double timeelapsed = stop - start;
    distance = (timeelapsed * 34300) / 2;
  } else {
    distance = 100.0;
  }
  return distance;
}
void destroy_HCSR04(sensor_t *sensor_object) {
  if (!DEBUG) {
    gpio_t gpio_in = sensor_get_pin(sensor_object,2);
    gpio_t gpio_out = sensor_get_pin(sensor_object,1);
    pin_close(&gpio_in);
    pin_close(&gpio_out);
  }
}
char*  tostring_HCSR04(sensor_userdata_t *sensor_data) {
  char *tostring_output;
  if (!DEBUG) {
    asprintf(&tostring_output, "HCSR04 - Ultrasonic Sensor\nName: %s CallBack: %s \nTrigger: %d Echo: %d", sensor_data->name, sensor_data->callback, sensor_get_pin(sensor_data->s,1).pin,sensor_get_pin(sensor_data->s,2).pin);
  } else {
    asprintf(&tostring_output, "HCSR04 - Ultrasonic Sensor\nName: %s CallBack: %s \nTrigger: 12 Echo: 24", sensor_data->name, sensor_data->callback);
  }
  return tostring_output;
}
char* info_HCSR04() {
  return "HCSR04 - Ultrasonic Sensor Information\nOperating Voltage: 5V DC \nOperating Current: 15mA \nMeasure Angle: 15°\nRanging Distance: 2cm - 4m\nVCC (Power), Trig (Trigger), Echo (Receive), GND (Ground).";
}
