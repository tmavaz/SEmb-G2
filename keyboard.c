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
/*
 * Função que é ativada quando a interrupção do teclado é ativada
 */

/*void keyboard_int_handler(void) {
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

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
            Lcd_Write_String("Bem vindo! ;)");
    }
    if(key_pressed == '2')  //temperatura
    {
        //Recebe dados da fila
        xQueueReceiveFromISR(temp_queue, &receivedData, portMAX_DELAY);

        ftoa(receivedData, string, 2);

        if(receivedData > TEMP_THRESHOLD)
        {
            xQueueSendFromISR(buzzer_queue, 3, &xHigherPriorityTaskWoken);
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
       xQueueSendFromISR(buzzer_queue, 2, &xHigherPriorityTaskWoken);
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





