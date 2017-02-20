#ifndef HEADER_H
#define	HEADER_H

#ifndef INPUT
#define INPUT 1
#endif
#ifndef OUTPUT
#define OUTPUT 0
#endif
#ifndef ANALOG
#define ANALOG 1
#endif
#ifndef DIGITAL
#define DIGITAL 0
#endif


//Device Oscillator Frequency in Hz
#define _XTAL_FREQ  32000000

//Comment the line below to disable debugging mode!
#define _DEBUG_UART_    //Comment this line if you don't want the UART output


//The files we will be needing :)
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "i2cbysk.h"

#include "nRF_lib/RF24.h"
#include "deviceconfig.h"
#include "uarttools.h"
#include "spi.h"
#include "si7021.h"

#include "sensorBoard.h"


void TimerDelayMs(uint16_t delay);

uint8_t sendPayload[32] = { 0 };
uint8_t receivePayload[32] = { 0 };

#endif	/* HEADER_H */

