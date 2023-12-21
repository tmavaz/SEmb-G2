//*****************************************************************************
//
// freertos_demo.c - Simple FreeRTOS example.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "led_task.h"
#include "switch_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "keyboard.h"
#include "blinky.h"

// Define o tamanho da queue temp
#define TEMP_QUEUE_LENGTH 1
// Define o tamanho da queue buzzer
#define BUZZER_QUEUE_LENGTH 20
// Define o tamanho dos itens na queue
#define ITEM_SIZE sizeof(float)
// Define um threshold para o sensor de temperatura
#define TEMP_THRESHOLD 30

// Declaração da queue para guardar os valores da temperatura
QueueHandle_t temp_queue;
QueueHandle_t buzzer_queue;
//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif
//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

void keyboardTask()
{
    float receivedData;
    char string[6];
    while (1){
        char key_pressed = keyboard_reader();
        if(key_pressed == '1'){
            taskENTER_CRITICAL();
            Lcd_Clear();
            Lcd_Write_String("Bem vindo Rocha! ;)");
            taskEXIT_CRITICAL();
        }
        if(key_pressed == '2')  //temperatura
        {
            //Recebe dados da fila
            xQueueReceive(temp_queue, &receivedData, portMAX_DELAY);

            ftoa(receivedData, string, 2);

            if(receivedData > TEMP_THRESHOLD)
            {
                xQueueSend(buzzer_queue, 3, portMAX_DELAY);
            }
            taskENTER_CRITICAL();
            Lcd_Clear();
            Lcd_Write_String("Temperatura: ");
            Lcd_Write_String(string);
            taskEXIT_CRITICAL();
       }
       if(key_pressed == '3')  //Temperatura
       {
           taskENTER_CRITICAL();
           Lcd_Write_Char('3');
           taskEXIT_CRITICAL();
       }
       if(key_pressed == '4')   //Tocar o buzzer
       {
           xQueueSend(buzzer_queue, 2, portMAX_DELAY);
       }
       if(key_pressed == 'C')  //Clear
       {
           taskENTER_CRITICAL();
           Lcd_Clear();
           taskEXIT_CRITICAL();
       }
    }
}

/*
 * Função que é ativada quando a interrupção do teclado é ativada
 */

/*void keyboard_int_handler(void) {
    // Desativa a flag da interrupção
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_7 | GPIO_INT_PIN_6 | GPIO_INT_PIN_5 | GPIO_INT_PIN_4);

    GPIOPinWrite(GPIO_PORTB_BASE,
                 GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
                 0);

    float receivedData;
    char string[6];
    // Lê o teclado para ver a tecla que foi pressionada
    char key_pressed = keyboard_reader();

    if(key_pressed == '1'){
            Lcd_Clear();
            Lcd_Write_String("Bem vindo Rocha! ;)");
    }
    if(key_pressed == '2')  //temperatura
    {
        //Recebe dados da fila
        xQueueReceiveFromISR(temp_queue, &receivedData, portMAX_DELAY);

        ftoa(receivedData, string, 2);

        if(receivedData > TEMP_THRESHOLD)
        {
            xQueueSendFromISR(buzzer_queue, 3, portMAX_DELAY);
        }
        Lcd_Clear();
        Lcd_Write_String("Temperatura: ");
        Lcd_Write_String(string);
   }
   if(key_pressed == '3')  //temperatura
   {
       Lcd_Write_Char('3');
   }
   if(key_pressed == '4')   //Tocar o buzzer
   {
       xQueueSendFromISR(buzzer_queue, 2, portMAX_DELAY);
   }
   if(key_pressed == 'C')  //temperatura
   {
       Lcd_Clear();
   }
}*/

/*
 * Função que configura as interrupções do teclado
 */

/*void keyboard_int_init() {

    keyboard_init();
    // Configure GPIO interrupts
    GPIOIntRegister(GPIO_PORTC_BASE, keyboard_int_handler);                             // Regista o handler da interrupção
    GPIOIntTypeSet(GPIO_PORTC_BASE,
                   GPIO_INT_PIN_7 | GPIO_INT_PIN_6 | GPIO_INT_PIN_5 | GPIO_INT_PIN_4,   // Define estes pinos para a interrupção
                   GPIO_RISING_EDGE);                                                   // Define a interrupção para funcionar no flanco ascendente
    GPIOIntEnable(GPIO_PORTC_BASE,
                  GPIO_INT_PIN_7 | GPIO_INT_PIN_6 | GPIO_INT_PIN_5 | GPIO_INT_PIN_4);   // Ativa as interrupções para estes pinos do porto C

    //deixa os pinos de output a high para que a interrupção possa ser ativada
    GPIOPinWrite(GPIO_PORTB_BASE,
                 GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
                 GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}*/



void tempTask()
{
    float temperature;

    I2Cinit();
    I2CSend(0x01);
    I2CSend(0x60);

    while (1) {
        //leitura da temperatura
        temperature = I2CReceiveTemp(0b1001001);

        // Envia dados para a fila
        xQueueSend(temp_queue, &temperature, portMAX_DELAY);

        // Aguarda por algum tempo antes de enviar o próximo dado
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void buzzerTask()
{
    int i,j,n;
    while(1){
        xQueueReceive(buzzer_queue, &n, portMAX_DELAY);
        for(j=0;j<n;j++){
            for(i=0;i<500;i++){
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_PIN_7);
                vTaskDelay(pdMS_TO_TICKS(1));
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, 0x0);
                vTaskDelay(pdMS_TO_TICKS(1));
             }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int
main(void)
{
    //melhorar///////////////////////////////////////////////////////////////////////
    buzzer_init();
    Lcd_pin_Init();
    Lcd_Init();
    UART_init();
    keyboard_init();
    //keyboard_int_init();
    //IntMasterEnable();

    Lcd_Clear();
    Lcd_Write_Char('H');

    ////////////////////////////////////////////////////////////////////////////////////
    BaseType_t xkeyboardTask, xtempTask, xbuzzerTask;

    // Inicialização da temp_queue
    temp_queue = xQueueCreate(TEMP_QUEUE_LENGTH, ITEM_SIZE);
    // Inicialização do buzzer_queue
    buzzer_queue = xQueueCreate(BUZZER_QUEUE_LENGTH, ITEM_SIZE);

    // Verificação temp_queue e buzzer_queue
    if(temp_queue == NULL || buzzer_queue == NULL)
    {
        printf("Erro na criacao da queue.");
    }

    // Criação das tarefas
    xkeyboardTask = xTaskCreate(keyboardTask, "Keyboard", 200, NULL, 1, NULL);
    xtempTask = xTaskCreate(tempTask, "Temperature", 200, NULL, 1, NULL);
    xbuzzerTask = xTaskCreate(buzzerTask, "Buzzer", 200, NULL, 1, NULL);

    // Verficação da criação das tasks
    if(xkeyboardTask != pdPASS || xtempTask != pdPASS || xbuzzerTask != pdPASS)
    //if(xtempTask != pdPASS || xbuzzerTask != pdPASS)
    {
        printf("Erro na criacao da task");
    }

    vTaskStartScheduler();

    while(1)
    {

    }

}
