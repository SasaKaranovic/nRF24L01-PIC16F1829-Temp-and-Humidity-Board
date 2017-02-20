#include "main.h"

void Device_SetUp()
{
#ifdef _PIC16F1829_H_
    //Configure oscillator for 32MHz (8MHz 4xPLL)
    OSCCONbits.SPLLEN   = 1;        //1 = 4xPLL Is enabled
    OSCCONbits.IRCF     = 0b1110;   //1110 = 8 MHz or 32 MHz HF(see Section 5.2.2.1 ?HFINTOSC?)
    OSCCONbits.SCS      = 0b00;     //00 = Clock determined by FOSC<2:0> in Configuration Word 1.

    //wait for oscillator to stabilise
    while(!OSCSTATbits.HFIOFR);         //1 = HFINTOSC is ready
    while(!OSCSTATbits.HFIOFS);         //1 = HFINTOSC is at least 0.5% accurate
    while(!OSCSTATbits.PLLR);           //1 = 4xPLL is ready

    //Configure Activity LED
    ANSELLEDBLUE    = DIGITAL;
    ANSELLEDGREEN   = DIGITAL;
    ANSELLEDRED     = DIGITAL;
    
    TRISLEDRED      = OUTPUT;
    TRISLEDGREEN    = OUTPUT;
    TRISLEDBLUE     = OUTPUT;

    LEDRED = LEDGREEN = LEDBLUE = 1;

#else
#warning "DeviceSetup() function is empty for this device"
#endif
}

void SetUpInterrupts()
{
#ifdef _PIC16F1829_H_
    INTCONbits.PEIE = 1;
    PIE1bits.RCIE   = 1;
    PIE1bits.TMR1IE = 1;

#else
#warning "SetUpInterrupts() function is empty for this device"
#endif

}


void UARTSetup(uint16_t baudrate)
{
#ifndef _XTAL_FREQ
    #warning _XTAL_FREQ is not defined!;
#endif
#ifdef _PIC16F1829_H_
    //Select and configure RX/TX pins
    APFCON0bits.RXDTSEL = 1;    //1 = RX/DT function is on RC5
    APFCON0bits.TXCKSEL = 1;    //1 = TX/CK function is on RC4

    
    TRISCbits.TRISC5    = INPUT;
    TRISCbits.TRISC4    = OUTPUT;
    LATCbits.LATC4      = 1;

    //Configure baudrate
    BAUDCONbits.BRG16   = 1;    //1 = 16-bit Baud Rate Generator is used
    uint16_t baud = (( (_XTAL_FREQ / baudrate) / 4 ) - 1);
    SPBRGL  = (baud & 0xFF);
    SPBRGH  = (baud >> 8);

    //Configure TX and RC registers
    TXSTAbits.BRGH  = 1;        //1 = High speed
    TXSTAbits.TXEN  = 1;        //1 = Transmit enabled
    RCSTAbits.CREN  = 1;        //1 = Enable Receive
    RCSTAbits.SPEN  = 1;        //1 = Serial port enabled (configures RX/DT and TX/CK pins as serial port pins)

    __delay_ms(10);  //Give UART some time to stabilise (not necessary)

#else
#warning "SetupUART() function is empty for this device"
#endif
    
}


void Timer0_Setup()
{
    OPTION_REGbits.T0CS = 0;        //0 = Internal instruction cycle clock (FOSC/4)
    OPTION_REGbits.PSA  = 0;        //0 = Prescaler is assigned to the Timer0 module
    OPTION_REGbits.PS   = 0b000;    //1:2

    //Enable Timer0 interrupt
    INTCONbits.PEIE = 1;
    INTCONbits.T0IE = 1;
}


void Timer1_Setup()
{
    T1CONbits.TMR1CS    = 0b01;   //01 = Timer1 clock source is system clock (FOSC)
    T1CONbits.TMR1ON    = 1;      //1 = Enables Timer1

    //Enable Timer0 interrupt
    INTCONbits.PEIE = 1;
    PIE1bits.TMR1IE = 1;
}



void putch(uint8_t data) {
    while(TXSTAbits.TRMT != 1);
    TXREG = data;
}


void StatusLED(uint8_t status)
{
    switch(status) {
        case LED_OFF:
            LEDRED      = 0;
            LEDGREEN    = 0;
            LEDBLUE     = 0;
            break;
            
        case LED_ONLINE:
            LEDRED      = 0;
            LEDGREEN    = 1;
            LEDBLUE     = 0;
            break;
            
        case LED_WORKING:
            LEDRED      = 0;
            LEDGREEN    = 1;
            LEDBLUE     = 1;
            break;
            
        case LED_FAIL:
            LEDRED      = 1;
            LEDGREEN    = 1;
            LEDBLUE     = 0;
            break;
            
        case LED_ALL:
            LEDRED      = 1;
            LEDGREEN    = 1;
            LEDBLUE     = 1;
            break;
            
        default:
            break;
    }
}


void Device_Initialize()
{
    UARTSetup(57600);
    __delay_ms(100);
    putch(27);
    UART_Debug("UART IS ON\n");
    __delay_ms(500);
}