#ifndef SENSORBOARD_H
#define	SENSORBOARD_H

#define BOARD_ID    0xA1


extern uint8_t si7021_tempNum, si7021_tempDec, si7021_humidNum, si7021_humidDec;

void sb_readTemperature();
void sb_readHumidity();


#endif	/* SENSORBOARD_H */

