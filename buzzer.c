#include "buzzer.h"

/*
 * Fun��o que faz a inicializa��o do pino do buzzer
 *
 */

void buzzer_init()
{
    //Inicializa��o do pino do buzzer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)){}
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_7);
}

