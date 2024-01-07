#include "buzzer.h"

/*
 * Função que faz a inicialização do pino do buzzer
 *
 */

void buzzer_init()
{
    //Inicialização do pino do buzzer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)){}
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_7);
}

