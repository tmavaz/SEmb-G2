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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "keyboard.h"
#include "blinky.h"
#include "date.h"

// Define o tamanho da queue temp
#define TEMP_QUEUE_LENGTH 1
// Define o tamanho da queue buzzer
#define BUZZER_QUEUE_LENGTH 20
// Define o tamanho da queue uart
#define UART_QUEUE_LENGTH 20
// Define o tamanho dos itens na queue temp
#define ITEM_SIZE sizeof(float)
// Define um threshold para o sensor de temperatura
#define TEMP_THRESHOLD 30
// Define o tamanho dos itens na queue uart
#define UART_SIZE 100*sizeof(char)

#define PACKET_NUMBER 1
#define LIGHT_SENSOR 2
#define CPU_TEMP 3
#define AVERAGE_TEMP 4
#define HUMIDITY 5
#define ACCELERATION 6
#define PRESSURE 7
#define RSSI 8
#define SNR 9

// Estrutura dos dados enviados pelo satélite
typedef struct Packet
{
    float packet_number;
    float light_sensor;
    float CPU_temp;
    float average_temp;
    float humidity;
    float acceleration;
    float pressure;
    float rssi;
    float snr;
} Packet;

// Declaração das queues
QueueHandle_t temp_queue;
QueueHandle_t buzzer_queue;
QueueHandle_t uart_queue;

Packet packet;
float packetcount = 0;
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
void vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while (1)
    {
    }
}

float packet_division(char *buffer, int number)
{
    //char buffer[];
    int index = 0;
    int j = 0;
    char packet_number[6];
    char light_sensor[4];
    char CPU_temp[4];
    char average_temp[4];
    char humidity[4];
    char acceleration[4];
    char pressure[5];
    char rssi[4];
    char snr[4];

    //copiar o packet number
    for (index = 3; buffer[index] != ','; index++)
    {
        packet_number[j] = buffer[index];
        j++;
    }

    // Adiciona o caracter nulo ao final da string
    packet_number[5] = '\0';
    packet.packet_number = atof(packet_number);

    index++;
    j = 0;
    //copiar o light sensor
    for (; buffer[index] != ','; index++)
    {
        light_sensor[j] = buffer[index];
        j++;
    }
    // Adiciona o caracter nulo ao final da string
    light_sensor[3] = '\0';
    packet.light_sensor = atof(light_sensor);

    //copiar a temperatura do CPU
    index++;
    j = 0;
    for (; buffer[index] != ','; index++)
    {
        CPU_temp[j] = buffer[index];
        j++;
    }
    // Adiciona o caracter nulo ao final da string
    CPU_temp[3] = '\0';
    packet.CPU_temp = atof(CPU_temp);

    //copiar a temperatura média do satélite
    index++;
    j = 0;
    for (; buffer[index] != ','; index++)
    {
        average_temp[j] = buffer[index];
        j++;
    }
    // Adiciona o caracter nulo ao final da string
    average_temp[3] = '\0';
    packet.average_temp = atof(average_temp);

    //copiar a humidade
    index++;
    j = 0;
    for (; buffer[index] != ','; index++)
    {
        humidity[j] = buffer[index];
        j++;
    }
    // Adiciona o caracter nulo ao final da string
    humidity[3] = '\0';
    packet.humidity = atof(humidity);

    //copiar a aceleração
    index++;
    j = 0;
    for (; buffer[index] != ','; index++)
    {
        acceleration[j] = buffer[index];
        j++;
    }
    // Adiciona o caracter nulo ao final da string
    acceleration[3] = '\0';
    packet.acceleration = atof(acceleration);

    //copiar a pressão
    index++;
    j = 0;
    for (; buffer[index] != ';'; index++)
    {
        pressure[j] = buffer[index];
        j++;
    }
    // Adiciona o caracter nulo ao final da string
    pressure[4] = '\0';
    packet.pressure = atof(pressure);

    //copiar o rssi
    index++;
    for (; buffer[index] != '-'; index++)
    {
        j = 0;
    };

    index++;
    j = 0;
    for (; buffer[index] != ','; index++)
    {
        rssi[j] = buffer[index];
        j++;
    }
    // Adiciona o caracter nulo ao final da string
    rssi[3] = '\0';
    packet.rssi = atof(rssi);

    //copiar o snr
    index++;
    j = 0;
    for (; buffer[index] != '\0'; index++)
    {
        snr[j] = buffer[index];
        j++;
    }
    // Adiciona o caracter nulo ao final da string
    snr[3] = '\0';
    packet.snr = atof(snr);

    switch (number)
    {
    case PACKET_NUMBER:
        return packet.packet_number;
    case LIGHT_SENSOR:
        return packet.light_sensor;
    case CPU_TEMP:
        return packet.CPU_temp;
    case AVERAGE_TEMP:
        return packet.average_temp;
    case HUMIDITY:
        return packet.humidity;
    case ACCELERATION:
        return packet.acceleration;
    case PRESSURE:
        return packet.pressure;
    case RSSI:
        return packet.rssi;
    case SNR:
        return packet.snr;
    }
}

void keyboardTask()
{
    BaseType_t sucessfulReceived;
    float receivedData;
    char string[6];
    char buffer[50];
    int i;
    char key_pressed_previous = 'G';
    char stringmes[3];
    char stringdia[3];
    char stringano[5];
    char stringhora[3];
    char stringminuto[3];
    char stringsegundo[3];

    while (1)
    {
        char key_pressed = keyboard_reader();
        if (key_pressed_previous != key_pressed)
        {
            key_pressed_previous = key_pressed;

            //Temperatura
            if (key_pressed == '1')
            {
                //Recebe dados da fila
                sucessfulReceived = xQueueReceive(temp_queue, &receivedData,
                                                  100);
                if (sucessfulReceived == pdTRUE)
                {
                    ftoa(receivedData, string, 2);

                    if (receivedData > TEMP_THRESHOLD)
                    {
                        int three = 3;
                        xQueueSend(buzzer_queue, &three, 100);
                    }
                    taskENTER_CRITICAL();
                    Lcd_Clear();
                    Lcd_Write_String("Temperatura: ");
                    Lcd_Write_String(string);
                    taskEXIT_CRITICAL();
                }
                else
                {
                    taskENTER_CRITICAL();
                    Lcd_Clear();
                    Lcd_Write_String("Nao ha dados da temperatura");
                    taskEXIT_CRITICAL();
                }
            }

            //PocketQube - numero de pacotes recebidos
            if (key_pressed == '2')
            {
                ftoa(packetcount, string, 0);
                taskENTER_CRITICAL();
                Lcd_Clear();
                Lcd_Write_String("Nr de pacotes:");
                Lcd_Write_String(string);
                taskEXIT_CRITICAL();
            }

            //PocketQube - todos os dados
            if (key_pressed == '3')
            {
                //Recebe dados da fila
                sucessfulReceived = xQueueReceive(uart_queue, &buffer, 100);

                if (sucessfulReceived == pdTRUE)
                {
                    packet_division(&buffer, PACKET_NUMBER);
                    taskENTER_CRITICAL();
                    Lcd_Clear();
                    for (i = 0; i < 20; i++)
                        Lcd_Write_Char(buffer[i]);
                    taskEXIT_CRITICAL();
                }
                else
                {
                    taskENTER_CRITICAL();
                    Lcd_Clear();
                    Lcd_Write_String("Nao ha dados do satelite");
                    taskEXIT_CRITICAL();
                }
            }

            //PocketQube - numero do pacote
            if (key_pressed == 'F')
            {
                ftoa(packet.packet_number, string, 0);
                taskENTER_CRITICAL();
                Lcd_Clear();
                Lcd_Write_String("Nr do pacote:");
                Lcd_Write_String(string);
                taskEXIT_CRITICAL();

            }

            //PocketQube - sensor de luz
            if (key_pressed == '4')
            {
                ftoa(packet.light_sensor, string, 0);
                taskENTER_CRITICAL();
                Lcd_Clear();
                Lcd_Write_String("Sensor de luz:");
                Lcd_Write_String(string);
                taskEXIT_CRITICAL();
            }

            //PocketQube - temperatura do CPU
            if (key_pressed == '5')
            {
                ftoa(packet.CPU_temp, string, 0);
                taskENTER_CRITICAL();
                Lcd_Clear();
                Lcd_Write_String("Temp. do CPU:");
                Lcd_Write_String(string);
                taskEXIT_CRITICAL();
            }

            //PocketQube - temperatura média do satélite
            if (key_pressed == '6')
            {
                ftoa(packet.average_temp, string, 0);
                taskENTER_CRITICAL();
                Lcd_Clear();
                Lcd_Write_String("Temp. do sat:");
                Lcd_Write_String(string);
                taskEXIT_CRITICAL();
            }

            //PocketQube - humidade
            if (key_pressed == 'E')
            {
                ftoa(packet.humidity, string, 0);
                taskENTER_CRITICAL();
                Lcd_Clear();
                Lcd_Write_String("Humidade:");
                Lcd_Write_String(string);
                taskEXIT_CRITICAL();
            }

            //PocketQube - aceleração
            if (key_pressed == '7')
            {
                ftoa(packet.acceleration, string, 0);
                taskENTER_CRITICAL();
                Lcd_Clear();
                Lcd_Write_String("Aceleracao:");
                Lcd_Write_String(string);
                taskEXIT_CRITICAL();
            }

            //PocketQube - pressão
            if (key_pressed == '8')
            {
                ftoa(packet.pressure, string, 0);
                taskENTER_CRITICAL();
                Lcd_Clear();
                Lcd_Write_String("Pressao:");
                Lcd_Write_String(string);
                taskEXIT_CRITICAL();
            }

            //PocketQube - RSSI
            if (key_pressed == '9')
            {
                ftoa(packet.rssi, string, 0);
                taskENTER_CRITICAL();
                Lcd_Clear();
                Lcd_Write_String("RSSI: -");
                Lcd_Write_String(string);
                taskEXIT_CRITICAL();
            }

            //PocketQube - SNR
            if (key_pressed == 'D')
            {
                ftoa(packet.snr, string, 0);
                taskENTER_CRITICAL();
                Lcd_Clear();
                Lcd_Write_String("SNR:");
                Lcd_Write_String(string);
                taskEXIT_CRITICAL();
            }

            //Data
            if (key_pressed == 'A')
            {
                ftoa(date.mes, stringmes, 0);
                ftoa(date.dia, stringdia, 0);
                ftoa(date.ano, stringano, 0);
                ftoa(date.hora, stringhora, 0);
                ftoa(date.minuto, stringminuto, 0);
                ftoa(date.segundo, stringsegundo, 0);

                taskENTER_CRITICAL();
                Lcd_Clear();
                Lcd_Write_String(stringmes);
                Lcd_Write_String("-");
                Lcd_Write_String(stringdia);
                Lcd_Write_String("-");
                Lcd_Write_String(stringano);
                Lcd_Write_String(" ");
                Lcd_Write_String(stringhora);
                Lcd_Write_String(":");
                Lcd_Write_String(stringminuto);
                Lcd_Write_String(":");
                Lcd_Write_String(stringsegundo);
                taskEXIT_CRITICAL();
            }

            //Tocar o buzzer
            if (key_pressed == 'B')
            {
                int two=2;
                xQueueSend(buzzer_queue, &two, 250);

            }

            //Clear
            if (key_pressed == 'C')
            {
                taskENTER_CRITICAL();
                Lcd_Clear();
                taskEXIT_CRITICAL();
            }
        }
    }
}

void tempTask()
{
    float temperature;

    I2Cinit();
    I2CSend(0x01);
    I2CSend(0x60);

    while (1)
    {
        //leitura da temperatura
        taskENTER_CRITICAL();
        temperature = I2CReceiveTemp(0b1001001);
        taskEXIT_CRITICAL();

        // Queue Reset antes de ser colocado um novo valor
        xQueueReset(temp_queue);

        // Envia dados para a fila
        xQueueSend(temp_queue, &temperature, portMAX_DELAY);

        // Aguarda por algum tempo antes de enviar o próximo dado
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void buzzerTask()
{
    int i, j;
    int n = 0;
    while (1)
    {
        xQueueReceive(buzzer_queue, &n, portMAX_DELAY);
        for (j = 0; j < n; j++)
        {
            for (i = 0; i < 500; i++)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_PIN_7);
                vTaskDelay(pdMS_TO_TICKS(1) / 100);
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, 0x0);
                vTaskDelay(pdMS_TO_TICKS(1) / 100);
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

void uartTask()
{
    uint8_t i = 0;
    char receivedChar;
    char buffer[buffer_size];
    char buffer_trash[buffer_size];
    int charP = 0;

    while (1)
    {
        while (i < buffer_size - 1)
        {
            //Aguarda ate que um caracter esteja disponivel para leitura
            while (!UARTCharsAvail(UART7_BASE))
                vTaskDelay(pdMS_TO_TICKS(1));

            // Le um caractere da porta UART
            receivedChar = UARTCharGet(UART7_BASE);

            // Verifica se e um caracter de terminaçao
            if (receivedChar == 'P')
            {
                charP = 1;
                //taskENTER_CRITICAL();
            }

            // Verifica se e um caracter de terminaçao
            if (receivedChar == '\n' || receivedChar == '\r')
            {
                //taskEXIT_CRITICAL();
                charP = 0;
                // Adiciona o caracter nulo ao final da string
                buffer[i] = '\0';

                //Faz o buffer circular se a fila de 20 pacotes estiver cheia ...
                //faz a receção de um deles que elimina da fila e escreve na fila o novo
                while (xQueueSend(uart_queue, &buffer, 100) != pdTRUE)
                {
                    xQueueReceive(uart_queue, &buffer_trash, 100);
                }
                packetcount++;
                i = 0;
                break;
            }

            // Armazena o caracter no buffer
            if (charP == 1)
            {
                buffer[i++] = receivedChar;
                //Lcd_Write_Char(receivedChar);
                //i++;
                //return buffer;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int main(void)
{
    //melhorar///////////////////////////////////////////////////////////////////////
    buzzer_init();
    Lcd_pin_Init();
    Lcd_Init();
    UART_init();
    keyboard_init();

    Lcd_Clear();

    date_init();
    timer_init();

    Lcd_Clear();
    Lcd_Write_String("Bem vindo!");

    ////////////////////////////////////////////////////////////////////////////////////
    BaseType_t xkeyboardTask, xtempTask, xbuzzerTask, xuartTask;

    // Inicialização da temp_queue
    temp_queue = xQueueCreate(TEMP_QUEUE_LENGTH, ITEM_SIZE);
    // Inicialização do buzzer_queue
    buzzer_queue = xQueueCreate(BUZZER_QUEUE_LENGTH, ITEM_SIZE);
    // Inicialização do uart_queue
    uart_queue = xQueueCreate(UART_QUEUE_LENGTH, UART_SIZE);

    // Verificação temp_queue e buzzer_queue
    if (temp_queue == NULL || buzzer_queue == NULL || uart_queue == NULL)
    {
        printf("Erro na criacao da queue.");
    }

    // Criação das tarefas
    xkeyboardTask = xTaskCreate(keyboardTask, "Keyboard", 200, NULL, 1, NULL);
    xtempTask = xTaskCreate(tempTask, "Temperature", 200, NULL, 1, NULL);
    xbuzzerTask = xTaskCreate(buzzerTask, "Buzzer", 200, NULL, 1, NULL);
    xuartTask = xTaskCreate(uartTask, "UART", 200, NULL, 2, NULL);

    // Verficação da criação das tasks
    if (xkeyboardTask != pdPASS || xtempTask != pdPASS || xbuzzerTask != pdPASS
            || xuartTask != pdPASS)
    {
        printf("Erro na criacao da task");
    }

    vTaskStartScheduler();

    while (1)
    {

    }
}
