#include "blinky.h"

//*****************************************************************************
// The error routine that is called if the driver library encounters an error.
//*****************************************************************************
#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line){
    while(1);
}
#endif


//*****************************************************************************
void intInit(void) {

    keyboard_int_init(); //ativa as interupções do teclado

    // Enable global interrupts
    IntMasterEnable();
}


void reverse(char* str, int len){
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int intToStr(int x, char str[], int d){
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Conversão float para string
void ftoa(float n, char* res, int afterpoint){
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

// Conversão string para float

/*int main2(void){


    buzzer_init();
    keyboard_init();

    float temperature;
    char string[6];

    Lcd_pin_Init();
    Lcd_Init();
    char key_pressed='H';
    char key_pressed_previous='T';
    Lcd_Clear();
    Lcd_Write_Char('H');

    I2Cinit();
    I2CSend(0x01);
    I2CSend(0x60);

    UART_init();

    //intInit();

    while(1){


        key_pressed = keyboard_reader();
        if(key_pressed!=key_pressed_previous && key_pressed!='G'){
            key_pressed_previous=key_pressed;
            Lcd_Clear();
            Lcd_Write_Char((char*)key_pressed);
        }

        if(key_pressed == '1'){
            Lcd_Clear();
            Lcd_Write_String("Bem vindo Rocha! ;)");
        }

        SysCtlDelay(30000);

        temperature = I2CReceiveTemp(0b1001001);

        ftoa(temperature, string, 2);

        if(key_pressed == '2'){ //temperatura
            Lcd_Clear();
            Lcd_Write_String("Temperatura: ");
            Lcd_Write_String(string);
        }
        if(key_pressed == '3'){  //uart
            Lcd_Clear();
            UART_receive();
        }
        if(key_pressed == '4'){  //buzzer a tocar
            buzzer();
        }
    }
}*/
