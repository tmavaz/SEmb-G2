#include "UART.h"

/*
 * Função que faz a inicialização dos pinos do UART
 */
void UART_init(){

   SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
   while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART7)) {}

   SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
   while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)) {}

   GPIOPinConfigure(GPIO_PE0_U7RX); //RX
   GPIOPinConfigure(GPIO_PE1_U7TX); //TX
   GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);

   // Initialize the UART. Set the baud rate, number of data bits,
   //turn off parity, number of stop bits, and stick mode.
   //The UART is enabled by the function call
   UARTConfigSetExpClk(UART7_BASE, SysCtlClockGet(), 9600,
                           (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE));
}


/*
 * Função que lê o UART e retorna a string que foi lida
 */

/*
 * Alterar esta função para retornar a string em vez de estar a escrever no lcd
 */

void UART_receive()
{
    uint8_t i = 0;
    char receivedChar;
    char buffer[buffer_size];

    while (i < buffer_size - 1) {
        // Aguarda ate que um caracter esteja disponivel para leitura
        while (!UARTCharsAvail(UART7_BASE));

        // Le um caractere da porta UART
        receivedChar = UARTCharGet(UART7_BASE);

        // Verifica se e um caracter de terminaçao
        if (receivedChar == '\n' || receivedChar == '\r') {
            break;
        }

        // Armazena o caracter no buffer
        buffer[i++] = receivedChar;
        //Lcd_Write_Char(receivedChar);
        //return buffer;
    }

    // Adiciona o caracter nulo ao final da string
    buffer[i] = '\0';
}
