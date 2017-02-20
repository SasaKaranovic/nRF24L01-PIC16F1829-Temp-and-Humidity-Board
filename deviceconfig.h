#ifndef DEVICECONFIG_H
#define	DEVICECONFIG_H

//Configuration bits for PIC16F1829
#ifdef _16F1829
// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF       // PLL Enable (4x PLL disable)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)



//Activity (LED Anode) PIN
#define TRISLEDRED          TRISCbits.TRISC2
#define ANSELLEDRED         ANSELCbits.ANSC2
#define LEDRED              LATCbits.LATC2

#define TRISLEDGREEN        TRISCbits.TRISC0
#define ANSELLEDGREEN       ANSELCbits.ANSC0
#define LEDGREEN            LATCbits.LATC0

#define TRISLEDBLUE         TRISCbits.TRISC1
#define ANSELLEDBLUE        ANSELCbits.ANSC1
#define LEDBLUE             LATCbits.LATC1


#define EnableInterrupts()  (INTCONbits.GIE = 1)
#define DisableInterrupts() (INTCONbits.GIE = 0)




#endif

void Device_SetUp();
void Device_Initialize();
void Device_SetUpInterrupts();
void UART_Setup(uint16_t baudrate);
void putch(uint8_t data);

#define LED_OFF     0
#define LED_ONLINE  1
#define LED_WORKING 2
#define LED_FAIL    3
#define LED_ALL     4

void StatusLED(uint8_t status);


void Timer0_Setup();
void Timer1_Setup();
void SetUpInterrupts();



#endif	/* DEVICECONFIG_H */

