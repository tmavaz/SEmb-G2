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
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "LCD.h"

#define buffer_size 100

SemaphoreHandle_t uart_semaphore;

void UART_handler();
void UART_init();
void UART_receive();
