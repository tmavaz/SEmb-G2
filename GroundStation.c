//*****************************************************************************
// EstaÃ§Ã£o Terrestre para uma LoRa PocketQube
//
// Realizado por:
//  - JoÃ£o Cordeiro,    99150
//  - Maria Francisca Rodrigues,    99156
//  - Tiago Vaz,    110984
//
//  Ano Letivo 2023/2024
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
#include "UART.h"

// Define o tamanho da queue temp
#define TEMP_QUEUE_LENGTH 1
// Define o tamanho da queue buzzer
#define BUZZER_QUEUE_LENGTH 20
// Define o tamanho da queue uart
#define UART_QUEUE_LENGTH 20
// Define o tamanho dos itens na queue temp
#define ITEM_SIZE sizeof(float)
// Define um threshold para o sensor de temperatura
#define TEMP_THRESHOLD 28
// Define o tamanho dos itens na queue uart
#define UART_SIZE 100*sizeof(char)

// Define o tamanho dos vÃ¡rios parÃ¢metros do pacote
#define PACKET_NUMBER 1
#define LIGHT_SENSOR 2
#define CPU_TEMP 3
#define AVERAGE_TEMP 4
#define HUMIDITY 5
#define ACCELERATION 6
#define PRESSURE 7
#define RSSI 8
#define SNR 9

// Estrutura dos dados enviados pelo satÃ©lite
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

// DeclaraÃ§Ã£o das queues
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

/*
 * FunÃ§Ã£o que faz a divisÃ£o dos vÃ¡rios parÃ¢metros recebidos por um pacote
 */
float packet_division(char *buffer, int number)
{
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
    if (buffer[0] == 'P')
    {
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

        //copiar a temperatura mÃ©dia do satÃ©lite
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

        //copiar a aceleraÃ§Ã£o
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

        //copiar a pressÃ£o
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
    }

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

    return 0;
}

/*
 * FunÃ§Ã£o para encontrar o pacote pedido pelo utilizador
 */
void packetSearch(float npacket)
{
    char buffer[50];
    char bufferall[20][50];
    char npacketstring[6];
    BaseType_t sucessfulReceived;
    int j = 0;
    int i = 0;
    int h = 0;

    Lcd_Clear();
    Lcd_Write_String("A procurar");

    // Percorre a matriz com os dados do pacote
    while (j < UART_QUEUE_LENGTH)
    {
        //Recebe dados da fila
        sucessfulReceived = xQueueReceive(uart_queue, &buffer, 100);

        //Se o pacote da queue for bem recebido
        if (sucessfulReceived == pdTRUE)
        {
            //Copia para a matriz os caracteres da string do pacote
            for (h = 0; h < 50; h++)
            {
                bufferall[j][h] = buffer[h];
            }

            //Volta a colocar o pacote na fila
            xQueueSend(uart_queue, &buffer, 100);
            j++;
            if (bufferall[j-1][0] != 'P')
            {
                j--;
            }
        }
    }

    Lcd_Clear();

    // Percorre a matriz Ã  procura do pacote pedido pelo utilizador
    for (j = 0; j < 20; j++)
    {
        for (h = 0; h < 50; h++)
        {
            buffer[h] = bufferall[j][h];
        }

        packet_division(&buffer, PACKET_NUMBER);

        if (npacket == packet.packet_number)
        {
            Lcd_Clear();
            for (i = 0; i < 20; i++)
                Lcd_Write_Char(buffer[i]);
            return;
        }
    }

    Lcd_Clear();
    Lcd_Write_String("Nao foi encontrado");
}

/*
 * Gere o que o sistema deve fazer quando uma
 * tecla especÃ­fica do teclado  Ì�e pressionada
*/
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
    char npacote[6];
    float npacket;

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
                    Lcd_Clear();
                    Lcd_Write_String("Temperatura: ");
                    Lcd_Write_String(string);
                }
                else
                {
                    Lcd_Clear();
                    Lcd_Write_String("Nao ha dados da temperatura");
                }
            }

            //PocketQube - numero de pacotes recebidos
            if (key_pressed == '2')
            {
                ftoa(packetcount, string, 0);
                Lcd_Clear();
                Lcd_Write_String("Nr de pacotes:");
                Lcd_Write_String(string);
            }

            //PocketQube - todos os dados
            if (key_pressed == '3')
            {
                //Recebe dados da fila
                sucessfulReceived = xQueuePeek(uart_queue, &buffer, 100);
                //sucessfulReceived = xQueueReceive(uart_queue, &buffer, 100);

                if (sucessfulReceived == pdTRUE)
                {
                    packet_division(&buffer, PACKET_NUMBER);
                    Lcd_Clear();
                    for (i = 0; i < 20; i++)
                        Lcd_Write_Char(buffer[i]);
                }
                else
                {
                    Lcd_Clear();
                    Lcd_Write_String("Nao ha dados do satelite");
                }
            }

            //PocketQube - numero do pacote
            if (key_pressed == 'F')
            {
                ftoa(packet.packet_number, string, 0);
                Lcd_Clear();
                Lcd_Write_String("Nr do pacote:");
                Lcd_Write_String(string);

            }

            //PocketQube - sensor de luz
            if (key_pressed == '4')
            {
                ftoa(packet.light_sensor, string, 0);
                Lcd_Clear();
                Lcd_Write_String("Sensor de luz:");
                Lcd_Write_String(string);
            }

            //PocketQube - temperatura do CPU
            if (key_pressed == '5')
            {
                ftoa(packet.CPU_temp, string, 0);
                Lcd_Clear();
                Lcd_Write_String("Temp. do CPU:");
                Lcd_Write_String(string);
            }

            //PocketQube - temperatura mÃ©dia do satÃ©lite
            if (key_pressed == '6')
            {
                ftoa(packet.average_temp, string, 0);
                Lcd_Clear();
                Lcd_Write_String("Temp. do sat:");
                Lcd_Write_String(string);
            }

            //PocketQube - humidade
            if (key_pressed == 'E')
            {
                ftoa(packet.humidity, string, 0);
                Lcd_Clear();
                Lcd_Write_String("Humidade:");
                Lcd_Write_String(string);
            }

            //PocketQube - aceleraÃ§Ã£o
            if (key_pressed == '7')
            {
                ftoa(packet.acceleration, string, 0);
                Lcd_Clear();
                Lcd_Write_String("Aceleracao:");
                Lcd_Write_String(string);
            }

            //PocketQube - pressÃ£o
            if (key_pressed == '8')
            {
                ftoa(packet.pressure, string, 0);
                ;
                Lcd_Clear();
                Lcd_Write_String("Pressao:");
                Lcd_Write_String(string);
            }

            //PocketQube - RSSI
            if (key_pressed == '9')
            {
                ftoa(packet.rssi, string, 0);
                Lcd_Clear();
                Lcd_Write_String("RSSI: -");
                Lcd_Write_String(string);
            }

            //PocketQube - SNR
            if (key_pressed == 'D')
            {
                ftoa(packet.snr, string, 0);
                Lcd_Clear();
                Lcd_Write_String("SNR:");
                Lcd_Write_String(string);
            }

            //PocketQube - Pedir um pacote
            if (key_pressed == '0')
            {
                //Pedir ao utilizador o nÃºmero do pacote
                Lcd_Clear();
                Lcd_Write_String("Nr pacote: *****");
                Lcd_Set_Cursor(1, 12);
                npacote[0] = key_reader();
                npacote[1] = key_reader();
                npacote[2] = key_reader();
                npacote[3] = key_reader();
                npacote[4] = key_reader();
                npacote[5] = '\0';

                //ConversÃ£o para float
                npacket = atof(npacote);

                //Vai procurar o pacote introduzido pelo utilizador
                packetSearch(npacket);
            }

            //Data e Hora
            if (key_pressed == 'A')
            {
                ftoa(date.mes, stringmes, 0);
                ftoa(date.dia, stringdia, 0);
                ftoa(date.ano, stringano, 0);
                ftoa(date.hora, stringhora, 0);
                ftoa(date.minuto, stringminuto, 0);
                ftoa(date.segundo, stringsegundo, 0);

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
            }

            //Tocar o buzzer
            if (key_pressed == 'B')
            {
                int two = 2;
                xQueueSend(buzzer_queue, &two, 250);
            }

            //Clear
            if (key_pressed == 'C')
            {
                Lcd_Clear();
            }
        }
    }
}

/*
 * LÃª os valores do sensor de temperatura usando o protocolo I2C
 * e envia para o keyboardTask
*/
void tempTask()
{
    float temperature;

    I2Cinit();
    I2CSend(0x01);
    I2CSend(0x60);

    while (1)
    {
        // Leitura da temperatura
        taskENTER_CRITICAL();
        temperature = I2CReceiveTemp(0b1001001);
        taskEXIT_CRITICAL();

        // Queue Reset antes de ser colocado um novo valor
        xQueueReset(temp_queue);

        // Envia dados para a fila
        xQueueSend(temp_queue, &temperature, portMAX_DELAY);

        // Aguarda por algum tempo antes de enviar o prÃ³ximo dado
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/*
 * Produz um som do buzzer
*/
void buzzerTask()
{
    int i, j;
    int n = 0;
    while (1)
    {
        // Recebe da queue o nÃºmero de vezes que o buzzer vai tocar
        xQueueReceive(buzzer_queue, &n, portMAX_DELAY);

        // Gera um sinal PWM
        for (j = 0; j < n; j++)
        {
            for (i = 0; i < 500; i++)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_PIN_7); //High
                vTaskDelay(pdMS_TO_TICKS(1));
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, 0x0); //Low
                vTaskDelay(pdMS_TO_TICKS(1));
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

/*
 * LÃª os dados enviados pelo satÃ©lite para o PocketQube via UART
*/
void uartTask()
{
    uint8_t i = 0;
    char receivedChar;
    char buffer[buffer_size];
    char buffer_trash[buffer_size];
    int charP = 0;

    while (1)
    {
        // Se o semÃ¡foto for desbloqueado
        if (xSemaphoreTake(uart_semaphore,portMAX_DELAY) == pdTRUE)
        {
            // Percorre o pacote
            while (i < buffer_size - 1)
            {
                // Aguarda ate que um caracter esteja disponivel para leitura
                while (!UARTCharsAvail(UART7_BASE))
                    vTaskDelay(pdMS_TO_TICKS(1));

                // Le um caracter da porta UART
                receivedChar = UARTCharGet(UART7_BASE);

                // Verifica se Ã© um caracter de iniciaÃ§Ã£o
                if (receivedChar == 'P')
                {
                    charP = 1;
                }

                // Verifica se Ã© um caracter de terminaÃ§ao
                if ((receivedChar == '\n' || receivedChar == '\r')
                        && charP == 1)
                {
                    charP = 0;
                    // Adiciona o caracter nulo ao final da string
                    buffer[i] = '\0';

                    // Faz o buffer circular se a fila de 20 pacotes estiver cheia
                    // faz a receÃ§Ã£o de um deles que elimina da fila e escreve na fila o novo
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
                }
            }
        }
    }
}

//*****************************************************************************
//
// FunÃ§Ã£o principal
//
//*****************************************************************************
int main(void)
{
    // InicializaÃ§Ãµes
    buzzer_init();
    Lcd_pin_Init();
    Lcd_Init();
    keyboard_init();
    date_init();
    timer_init();
    UART_init();

    // Habilita as interrupÃ§Ãµes globalmente
    IntMasterEnable();

    // Mensagem inicial que aparece no LCD
    Lcd_Clear();
    Lcd_Write_String("Bem vindo!");

    BaseType_t xkeyboardTask, xtempTask, xbuzzerTask, xuartTask;

    // InicializaÃ§Ã£o da temp_queue
    temp_queue = xQueueCreate(TEMP_QUEUE_LENGTH, ITEM_SIZE);
    // InicializaÃ§Ã£o do buzzer_queue
    buzzer_queue = xQueueCreate(BUZZER_QUEUE_LENGTH, ITEM_SIZE);
    // InicializaÃ§Ã£o do uart_queue
    uart_queue = xQueueCreate(UART_QUEUE_LENGTH, UART_SIZE);

    // VerificaÃ§Ã£o temp_queue e buzzer_queue
    if (temp_queue == NULL || buzzer_queue == NULL || uart_queue == NULL)
    {
        Lcd_Write_String("Erro na criacao da queue.");
    }

    // InicializaÃ§Ã£o do semaforo
    uart_semaphore = xSemaphoreCreateBinary();

    if (uart_semaphore == NULL)
    {
        Lcd_Write_String("Erro na criacao do semaforo.");
    }

    // CriaÃ§Ã£o das tarefas
    xkeyboardTask = xTaskCreate(keyboardTask, "Keyboard", 1000, NULL, 1, NULL);
    xtempTask = xTaskCreate(tempTask, "Temperature", 200, NULL, 1, NULL);
    xbuzzerTask = xTaskCreate(buzzerTask, "Buzzer", 200, NULL, 2, NULL);
    xuartTask = xTaskCreate(uartTask, "UART", 200, NULL, 3, NULL);

    // VerficaÃ§Ã£o da criaÃ§Ã£o das tasks
    if (xkeyboardTask != pdPASS || xtempTask != pdPASS || xbuzzerTask != pdPASS
            || xuartTask != pdPASS)
    {
        Lcd_Write_String("Erro na criacao da task");
    }

    vTaskStartScheduler();

    while (1)
    {

    }
}
