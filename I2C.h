#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "LCD.h"
#include "driverlib/i2c.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"

void I2Cinit();
void I2CSend(uint8_t slave_addr);
float I2CReceiveTemp(uint8_t slave_addr);
float conversao(uint16_t temperature);
