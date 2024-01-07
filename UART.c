#include "UART.h"

/*
 * Funcao de interrupcao do UART que desbloqueia o semaforo para
 * fazer a tarefa uartTask
 */
void UART_handler()
{
    // Verifica a fonte da interrupção UART
    uint32_t ui32Status = UARTIntStatus(UART7_BASE, true);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Limpa as interrupções UART
    UARTIntClear(UART7_BASE, ui32Status);

    xSemaphoreGiveFromISR(uart_semaphore,&xHigherPriorityTaskWoken);

    // Indica que uma mudança de contexto pode ser realizada
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*
 * Função que faz a inicialização dos pinos do UART
 */
void UART_init()
{

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)){}
    GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART7)){}

    GPIOPinConfigure(GPIO_PE0_U7RX); //RX
    GPIOPinConfigure(GPIO_PE1_U7TX); //TX

    // Inicialização do UART a 9600 bits/segundo, sem bit de paridade,
    // e com oito bits de dados
    UARTConfigSetExpClk(
            UART7_BASE, SysCtlClockGet(), 9600,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //Ativação da interrupção
    IntEnable(INT_UART7);
    IntPrioritySet(INT_UART7, 0);
    UARTIntRegister(UART7_BASE, UART_handler);
    UARTIntEnable(UART7_BASE, UART_INT_RX);
}

/*
 * Função que recebe os dados do uart e escreve no LCD o pacote recebido
 */
void UART_receive()
{
    uint8_t i = 0;
    char receivedChar;
    char buffer[buffer_size];
    int charP = 0;

    while (i < buffer_size - 1)
    {
        // Aguarda ate que um caracter esteja disponivel para leitura
        while (!UARTCharsAvail(UART7_BASE));

        // Le um caractere da porta UART
        receivedChar = UARTCharGet(UART7_BASE);

        // Verifica se é um caracter de iniciação
        if (receivedChar == 'P')
        {
            charP = 1;
        }

        // Verifica se é um caracter de terminaçao
        if (receivedChar == '\n' || receivedChar == '\r')
        {
            charP = 0;
            break;
        }

        // Armazena o caracter no buffer
        if (charP == 1)
        {
            Lcd_Write_Char(receivedChar);
            i++;
        }
    }

    // Adiciona o caracter nulo ao final da string
    buffer[i] = '\0';
}
