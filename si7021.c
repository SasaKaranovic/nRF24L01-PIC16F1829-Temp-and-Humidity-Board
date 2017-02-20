#include "main.h"

double si7021_ReadHumidity()
{
    uint8_t readData[2] = {0, 0};

//    UART_Debug("I2C Write start\r");
    I2C1_WriteByte(SI_Address, SI_ReadHumid_NoHold);
//    UART_Debug("I2C Read start\r");
    __delay_ms(60);
    I2C1_ReadFrame(SI_Address, readData, 3);
//    UART_Debug("I2C Now calculate...\r");

    //Calculate humidity
    uint16_t rawHumidity = (readData[0] << 8) | readData[1];
    
    double result =(((125 * (float) (rawHumidity)) / 65536) - 6);
    
    return result;
}


double si7021_ReadTemperature()
{
    uint8_t readData[2] = {0, 0};

//    UART_Debug("I2C Write start\r");
    I2C1_WriteByte(SI_Address, SI_ReadTemp_NoHold);
//    UART_Debug("I2C Read start\r");
    __delay_ms(60);
    I2C1_ReadFrame(SI_Address, readData, 3);
//    UART_Debug("I2C Now calculate...\r");

    //Calculate Temperature
    uint16_t rawTemp = (readData[0] << 8) | readData[1];
    
    double result = (175.25*rawTemp/65536)-46.85;
    
    return result;
}



