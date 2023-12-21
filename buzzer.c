#include "buzzer.h"

/*
 * Função que faz a inicialização do pino do buzzer
 *
 */

void buzzer_init(){

    //Inicialização do pino do buzzer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)){}
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_7);
}

/*
 * Alterar a função de for pelo delay de sistema do freeRTOS
 *
 * Confirmar os delays de todas as funçóes de todas as bibliotecas e alterar pelos do freeRTOS
 *
 * Confirmar com o stor se prefere funções de ROM [ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);]
 * ou de RAM [SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);]
 */

/*
 * Função que faz tocar o buzzer
 */
void buzzer(int n){

    volatile uint32_t ui32Loop;
    int i,j;
    for(j=0;j<n;j++){
        for(i=0;i<500;i++){
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_PIN_7);
            for(ui32Loop = 0; ui32Loop < 1000; ui32Loop++){}       //alterar isto
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, 0x0);
            for(ui32Loop = 0; ui32Loop < 1000; ui32Loop++){}       //alterar isto
         }
        for(ui32Loop = 0; ui32Loop < 1000000; ui32Loop++){}
    }
}

