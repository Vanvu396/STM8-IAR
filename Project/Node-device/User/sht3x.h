#include "stm8s_gpio.h"
#include "stm8s_it.h"
#include "stm8s_i2c.h"

#define SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW 0x44

#define SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH 0x2c06

void SHT30_Init(void);

uint8_t get_sht30(uint8_t *pData);


