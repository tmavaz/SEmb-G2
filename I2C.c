#include "I2C.h"

/*
 * Configuração dos pins do I2C2
*/
void I2Cinit()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C2)){}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)){}

    GPIOPinConfigure(GPIO_PE4_I2C2SCL); //SCL
    GPIOPinConfigure(GPIO_PE5_I2C2SDA); //SDA

    GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);
    GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);

    I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), true);
}

/*
 * Funcao que envia o pedido de dados ao sensor de temperatura
*/
void I2CSend(uint8_t slave_addr)
{
    // Escreve o endereço do slave no barramento
    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, false);

    // Envia os bytes de dados
    I2CMasterDataPut(I2C2_BASE, 0x01);

    // Inicia a transferência
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C2_BASE)){}

    I2CMasterDataPut(I2C2_BASE, 0x60);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
    while(I2CMasterBusy(I2C2_BASE)){}
}

/*
 * Funcao que recebe os dados da temperatura medida
*/
float I2CReceiveTemp(uint8_t slave_addr)
{
    uint16_t tempH,tempL;
    float cTemp;

    // Escreve o endereço do escravo no barramento para leitura
    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, true);

    // Inicia a leitura de um byte de dados
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

    // Aguarda a conclusão da leitura
    while (I2CMasterBusy(I2C2_BASE)) {}

    // Retorna o byte recebido
    tempH = I2CMasterDataGet(I2C2_BASE);

    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    while (I2CMasterBusy(I2C2_BASE)) {}

    tempL = I2CMasterDataGet(I2C2_BASE);

    cTemp = conversao((tempH<<8) | tempL);

    return cTemp;
}

/*
 * Funcao que converte os bits recebidos por I2C
 * para a temperatura em graus
*/
float conversao(uint16_t temperature) {
    float cTemp = (temperature) / 256.0;
    return cTemp;
}
