#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"
#include "LCD.h"

void Lcd_pin_Init();
