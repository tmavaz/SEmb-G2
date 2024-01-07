#include "LCD.h"

/*
 * Envio dos pinos da data do LCD
 *
 */
void Lcd_Port(char a)
{
    if (a & 1)
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_PIN_0);
    else
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0);
    if (a & 2)
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
    else
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
    if (a & 4)
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);
    else
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
    if (a & 8)
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PIN_3);
    else
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
}

/*
 * Configuração dos comandos do LCD
 *
 */
void Lcd_Cmd(char a)
{
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0); // => RS = 0
    Lcd_Port(a);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2); // => E = 1
    SysCtlDelay(30000);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0); // => E = 0
    SysCtlDelay(30000); // adding delay
}

/*
 * Apaga o ecrã
 *
 */
void Lcd_Clear(void)
{
    Lcd_Cmd(0);
    Lcd_Cmd(1);
}

/*
 * Configuração da posição do cursor do LCD
 *
 */
void Lcd_Set_Cursor(char a, char b)
{
    char temp, z, y;
    if (a == 1)
    {
        temp = 0x80 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }

    else if (a == 2)
    {
        temp = 0xC0 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }
}

/*
 * Inicialização do LCD
 *
 */
void Lcd_Init(void)
{
    Lcd_Port(0x00); //clear
    SysCtlDelay(30000);
    Lcd_Cmd(0x03);  //
    SysCtlDelay(30000);
    Lcd_Cmd(0x03);
    SysCtlDelay(30000);
    Lcd_Cmd(0x03);
/////////////////////////////////////////////////////
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x02);  //Function set 1, 0-4bits
    Lcd_Cmd(0x00);  // nº linhas  font 5x8 Nº de linhas 1

    Lcd_Cmd(0x00);  // display on/off
    Lcd_Cmd(0x0F);  // 1, Display-on, Cursor - 1, Blink -0

    Lcd_Cmd(0x00);  // entry mode set
    Lcd_Cmd(0x06);  // increment the address by 1, shift the cursor to right

}

/*
 * Escreve um caracter no LCD
 *
 */
void Lcd_Write_Char(char a)
{
    char temp, y;
    temp = a & 0x0F;
    y = a & 0xF0;
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_PIN_3); // => RS = 1
    Lcd_Port((y >> 4)); //Data transfer
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
    SysCtlDelay(30000);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0);
    Lcd_Port(temp);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
    SysCtlDelay(30000);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0);
}

/*
 * Escreve uma string no LCD
 *
 */
void Lcd_Write_String(const char *a)
{
    int i;
    for (i = 0; a[i] != '\0'; i++)
        Lcd_Write_Char(a[i]);
}

/*
 * Desloca para a direita o texto do LCD
 *
 */
void Lcd_Shift_Right(void)
{
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x0C);
}

/*
 * Desloca para a esquerda o texto do LCD
 *
 */
void Lcd_Shift_Left(void)
{
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x08);
}

/*
 * Escreve no LCD um inteiro
 *
 */
void Lcd_Write_Integer(int v)
{
    char buf[8];
    Lcd_Write_String(itoa(buf, v, 10));
}

/*
 * Escreve no LCD um float
 *
 */
void Lcd_Write_Float(float f)
{
    char *buf11;
    ftoa(f, buf11, 0);
    Lcd_Write_String(buf11);
}

/*
 * Atraso em milisegundos
 *
 */
void __delay_ms(uint16_t u16milisecond)
{
    SysCtlDelay(SysCtlClockGet() / 3000 * u16milisecond); //1ms
}

/*
 * Atraso em microsegundos
 *
 */
void __delay_us(uint16_t u16usecond)
{
    SysCtlDelay(SysCtlClockGet() / 3 * u16usecond); //1us
}

