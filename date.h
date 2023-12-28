#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include <time.h>
#include "keyboard.h"
#include "LCD.h"




#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "task.h"

//Definição dos parâmetros da data e hora
typedef struct Date
{
    float mes;
    float dia;
    float ano;
    float hora;
    float minuto;
    float segundo;
} Date;

Date date;

void timer_init();
void date_init();
char key_reader();
void add_second();


