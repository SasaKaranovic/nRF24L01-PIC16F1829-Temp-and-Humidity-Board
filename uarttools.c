#include "main.h"

void UART_SendDouble(uint16_t value)
{
    value = value / 10;     //Last 2 digits will be used as decimal point
    
    uint8_t temp10000, temp1000, temp100, temp10, temp1;

    temp1       = value     % 10;
    temp10      = value     / 10        % 10;
    temp100     = value     / 100       % 10;
    temp1000    = value     / 1000      % 10;
    temp10000   = value     / 10000     % 10;

    
    if(temp10000)
    {
        putch(48 + temp10000);
    }
    if(temp1000)
    {
        putch(48 + temp1000);
    }
    putch(48 + temp100);

    putch('.');
    putch(48 + temp10);
    putch(48 + temp1);
}

void UART_SendInteger(uint16_t value)
{
    uint8_t temp10000, temp1000, temp100, temp10, temp1;

    temp1       = value     % 10;
    temp10      = value     / 10        % 10;
    temp100     = value     / 100       % 10;
    temp1000    = value     / 1000      % 10;
    temp10000   = value     / 10000     % 10;


    if(temp10000) {  putch(48 + temp10000); }
    if(temp1000) {  putch(48 + temp1000); }
    if(temp100) {  putch(48 + temp100); }
    if(temp10) {  putch(48 + temp10); }
    putch(48 + temp1);
}

void UART_NewLine()
{
    putch(10);
    putch(13);
}




void UART_Debug(const char *str) {
#ifdef _DEBUG_UART_
    while(*str) {
        putch(*str);
        *str++;
    }
#endif
}

void UART_Write(const char *str) {
    while(*str) {
        putch(*str);
        *str++;
    }
}

void uart_puts(const char *str) {
    while(*str) {
        putch(*str);
        *str++;
    }
}

void UART_SendHex8(uint16_t num) {
	putch(HEX_CHARS[(num >> 4)   % 0x10]);
	putch(HEX_CHARS[(num & 0x0f) % 0x10]);
}

void UART_SendHex16(uint16_t num) {
	uint8_t i;
	for (i = 12; i > 0; i -= 4) putch(HEX_CHARS[(num >> i) % 0x10]);
	putch(HEX_CHARS[(num & 0x0f) % 0x10]);
}

void UART_SendHex32(uint32_t num) {
	uint8_t i;
	for (i = 28; i > 0; i -= 4)	putch(HEX_CHARS[(num >> i) % 0x10]);
	putch(HEX_CHARS[(num & 0x0f) % 0x10]);
}

