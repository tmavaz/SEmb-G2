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
#include "driverlib/uart.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "LCD.h"
#include "keyboard.h"
#include "lcd1.h"
#include "buzzer.h"
#include "I2C.h"

void reverse(char* str, int len);
int intToStr(int x, char str[], int d);
void ftoa(float n, char* res, int afterpoint);

