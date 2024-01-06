/*
 *
 * Alterar estas bibliotecas
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "LCD.h"
#include "driverlib/i2c.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"
#include <math.h>
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define buffer_size 100

SemaphoreHandle_t uart_semaphore;

void UARTIntHandler();
void UART_init();
void UART_receive();
