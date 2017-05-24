#include "GY521.h"

sensor_t* config_GY521(int smbus,int address) {
  gpio_t gpio_em;
  sensor_t *obj = sensor_create(gpio_em,gpio_em,gpio_em,gpio_em,gpio_em,gpio_em,gpio_em,gpio_em,gpio_em,gpio_em);
  if (!DEBUG) {
    #ifdef __linux__
    i2c_t i2c;
    if (i2c_open(&i2c, "/dev/i2c-1") < 0) {
        fprintf(stderr, "Connection Error: %s\n", i2c_errmsg(&i2c));
    }
    obj->i2c = i2c;
    #endif
  }
  return obj;
}
double read_GY521(sensor_t *sensor_object) {
  #ifdef __linux__
  i2c_t i2c = sensor_object.i2c;
  uint8_t temp_addr[1] = { 0x41 };
  uint8_t temp_data[2] = { 0x00,0x00 };
  uint8_t gyro_addr[3] = { 0x43,0x45,0x47 };
  uint8_t gyro_data[3] = { 0x00,0x00,0x00 };
  uint8_t accel_addr[3] = { 0x3B,0x3D,0x3F };
  uint8_t accel_data[3] = { 0x00,0x00,0x00 };
  uint8_t config_addr[2] = { 0x1C,0x1B };
  uint8_t config_data[2] = { 0x00,0x00 };
  struct i2c_msg temp_obj[8] =
  {
   { .addr = GY521_I2C_ADDR, .flags = 0, .len = 1, .buf = temp_addr },
   { .addr = GY521_I2C_ADDR, .flags = I2C_M_RD, .len = 2, .buf = temp_data},
   { .addr = GY521_I2C_ADDR, .flags = 0, .len = 3, .buf = gyro_addr },
   { .addr = GY521_I2C_ADDR, .flags = I2C_M_RD, .len = 3, .buf = gyro_data},
   { .addr = GY521_I2C_ADDR, .flags = 0, .len = 3, .buf = accel_addr },
   { .addr = GY521_I2C_ADDR, .flags = I2C_M_RD, .len = 3, .buf = accel_data},
   { .addr = GY521_I2C_ADDR, .flags = 0, .len = 2, .buf = config_addr },
   { .addr = GY521_I2C_ADDR, .flags = I2C_M_RD, .len = 2, .buf = config_data},
  };
  if (i2c_transfer(&i2c, temp_obj, 8) < 0) { fprintf(stderr, "Sensor Read Error: %s\n", i2c_errmsg(&i2c)); }
  int high = temp_data[0];
  int low = temp_data[1];
  int value = (high << 8) + low;
  int raw_data = -((65535 - value) + 1);
  double temp = (raw_data / 340.0) + 36.53;
  double a_config = config_data[0];
double g_config = config_data[1];
  double gyro_scale_modifier = 0.0;
  if (g_config == GYRO_RANGE_250DEG) { gyro_scale_modifier = GYRO_SCALE_MODIFIER_250DEG; }
  else if (g_config == GYRO_RANGE_500DEG) { gyro_scale_modifier = GYRO_SCALE_MODIFIER_500DEG; }
  else if (g_config == GYRO_RANGE_1000DEG) { gyro_scale_modifier = GYRO_SCALE_MODIFIER_1000DEG; }
  else if (g_config == GYRO_RANGE_2000DEG) { gyro_scale_modifier = GYRO_SCALE_MODIFIER_2000DEG; }
  else { gyro_scale_modifier = GYRO_SCALE_MODIFIER_250DEG; }
  double accel_scale_modifier = 0.0;
  if (a_config == ACCEL_RANGE_2G) { accel_scale_modifier = ACCEL_SCALE_MODIFIER_2G; }
  else if (a_config == ACCEL_RANGE_4G) { accel_scale_modifier = ACCEL_SCALE_MODIFIER_4G; }
  else if (a_config == ACCEL_RANGE_8G) { accel_scale_modifier = ACCEL_SCALE_MODIFIER_8G; }
  else if (a_config == ACCEL_RANGE_16G) { accel_scale_modifier = GYRO_SCALE_MODIFIER_2000DEG; }
  else { accel_scale_modifier = ACCEL_SCALE_MODIFIER_2G; }
double g_x = gyro_data[0] / gyro_scale_modifier;
double g_y = gyro_data[1] / gyro_scale_modifier;
double g_z = gyro_data[2] / gyro_scale_modifier;
double a_x = accel_data[0] / accel_scale_modifier;
double a_y = accel_data[1] / accel_scale_modifier;
double a_z = accel_data[2] / accel_scale_modifier;

printf("%f°C\n",temp);
printf("a_config: %f g_config: %f\n",a_config,g_config);
printf("a_x: %f a_y: %f a_z: %f\n",a_x,a_y,a_z);
printf("g_x: %f g_y: %f g_z: %f\n",g_x,g_y,g_z);
  #endif
  return 10.2;
}

void  destroy_GY521(sensor_t *sensor_object) {
  #ifdef __linux__
  i2c_close(&sensor_object->i2c);
  #endif
}
