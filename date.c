#include "date.h"

/*
 * Função que adiciona um segundo à contagem
 *
 */
void add_second()
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());

    if (date.segundo == 59)
    {
        date.segundo = 0;
        if (date.minuto == 59)
        {
            date.minuto = 0;
            if (date.hora == 23)
                date.hora = 0;
            else
                date.hora++;
        }
        else
            date.minuto++;
    }
    else
        date.segundo++;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Indica que uma mudança de contexto pode ser realizada
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*
 * Configuracao inicial do Timer 0
 *
 */
void timer_init()
{
    // Habilita o periférico do Timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Configura o Timer 0 para operar em modo periódico (up-count) e habilita o timer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerEnable(TIMER0_BASE, TIMER_A);

    // Define o valor de recarga do Timer 0 para 10 segundo
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());

    // Registra a função de tratamento de interrupção para o Timer 0
    TimerIntRegister(TIMER0_BASE, TIMER_A, add_second);

    // Habilita a interrupção de estouro do Timer 0
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

/*
 * Faz a leitura do teclado
 *
 */
char key_reader()
{
    char key_read = 'G';
    char key_pressed;

    while (key_read == 'G')
        key_read = keyboard_reader();
    key_pressed = key_read;
    Lcd_Write_Char(key_read);
    while (key_read != 'G')
        key_read = keyboard_reader();

    return key_pressed;
}

/*
 * Pede ao utilizador a data e a hora e guarda
 *
 */
void date_init()
{
    char mes[3];
    char dia[3];
    char ano[5];
    char hora[3];
    char minuto[3];
    char segundo[3];

    Lcd_Clear();
    Lcd_Write_String("MM-DD-YYYY hh:mm:ss");
    Lcd_Set_Cursor(1, 1);
    mes[0] = key_reader();
    mes[1] = key_reader();
    mes[2] = '\0';
    Lcd_Set_Cursor(1, 4);
    dia[0] = key_reader();
    dia[1] = key_reader();
    dia[2] = '\0';
    Lcd_Set_Cursor(1, 7);
    ano[0] = key_reader();
    ano[1] = key_reader();
    ano[2] = key_reader();
    ano[3] = key_reader();
    ano[4] = '\0';
    Lcd_Set_Cursor(1, 12);
    hora[0] = key_reader();
    hora[1] = key_reader();
    hora[2] = '\0';
    Lcd_Set_Cursor(1, 15);
    minuto[0] = key_reader();
    minuto[1] = key_reader();
    minuto[2] = '\0';
    Lcd_Set_Cursor(1, 18);
    segundo[0] = key_reader();
    segundo[1] = key_reader();
    segundo[2] = '\0';

    date.mes = atof(mes);
    date.dia = atof(dia);
    date.ano = atof(ano);
    date.hora = atof(hora);
    date.minuto = atof(minuto);
    date.segundo = atof(segundo);
}
