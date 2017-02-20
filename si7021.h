#ifndef SI7021_H
#define	SI7021_H

#define SI_Address  0x80

//Commands
#define SI_ReadHumid_Hold       0xE5
#define SI_ReadHumid_NoHold     0xF5
#define SI_ReadTemp_Hold        0xE3
#define SI_ReadTemp_NoHold      0xF3
#define SI_ReadTemp_Previous    0xE0

#define Si_ReadID1Byte          0xF1,0x0F
#define Si_ReadID2Byte          0xFC,0xC9

double si7021_ReadHumidity();
double si7021_ReadTemperature();


#endif	/* SI7021_H */

