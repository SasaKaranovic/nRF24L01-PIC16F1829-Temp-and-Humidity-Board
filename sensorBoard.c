#include "main.h"


uint8_t si7021_tempNum, si7021_tempDec, si7021_humidNum, si7021_humidDec;


void sb_readTemperature() 
{
    uint16_t temp;
    temp = (uint16_t)(si7021_ReadTemperature()*100);
    
    
    si7021_tempNum = temp /100;
    si7021_tempDec = temp % 100;
}


void sb_readHumidity() 
{
    uint16_t  humid;
    humid = (uint16_t)(si7021_ReadHumidity()*100);
    
    si7021_humidNum = humid / 100;
    si7021_humidDec = humid % 100;
}

