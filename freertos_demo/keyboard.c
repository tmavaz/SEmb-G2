#include "keyboard.h"

/*
 * Função que faz a inicialização dos pinos do teclado
 *
 */
void keyboard_init()
{
    //Inicialização dos pinos de output [Y]
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)){}
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);


    //Inicialização dos pinos de input [X]
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC)){}
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4);

}


/*
 * Função que lê o teclado e retorna a letra que foi lida
 */

/*
 * Alterar esta função para interrupções e só retorna a tecla lida, não o 'G'
 */
char keyboard_reader()
{
    int line, column;

    char keys[4][4] = {         //Definicao da matriz com as teclas do teclado
       {'1', '2', '3', 'F'},    //usada para verificar a tecla que foi pressionada
       {'4', '5', '6', 'E'},
       {'7', '8', '9', 'D'},
       {'A', '0', 'B', 'C'}
    };

    uint8_t y[] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3}; //Pinos de output do teclado [Y]
    uint8_t x[] = {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7}; //Pinos de input do teclado [X]

    for(column=0; column<4; column++){                              //Ciclo que passa pelos 4 outputs [Y]
        GPIOPinWrite(GPIO_PORTB_BASE, y[column], y[column]);        //Ativa o pino de saida
        for(line=0; line<4; line++){                                //Ciclo que passa pelos 4 inputs [X]
            if (GPIOPinRead(GPIO_PORTC_BASE, x[line]) == x[line]) { //Se o pino de input [X] estiver a high a tecla foi pressionada
                GPIOPinWrite(GPIO_PORTB_BASE, y[column], 0);        //Desativa o pino de output
                return keys[column][line];                          //Retorna a tecla pressionada
            }
        }
        GPIOPinWrite(GPIO_PORTB_BASE, y[column], 0);                //Desativa o pino de output
    }
    return 'G';
}


