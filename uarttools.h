#ifndef UARTTOOLS_H
#define	UARTTOOLS_H

#define HEX_CHARS      "0123456789ABCDEF"

void UART_SendDouble(uint16_t value);

/**
 * @Description Send uint16_t value as ASCII integer over UART
 * @Input uint16_t Value to be sent as ASCII Integer
 * @Output Sends ASCII value over UART
 */
void UART_SendInteger(uint16_t value);


void UART_NewLine();

/**
 * @Description Send *string over UART, used only in debugging mode. If you wan't always to
 * send data use UART_Write() function istead!
 * @Input const char *str
 * @Output void
 * @WARNING This function will NOT work if you don't uncomment #define _UART_DEBUG_ line
 */
void UART_Debug(const char *str);

/**
 * @Description Send *string over UART
 * @Input const char *str
 * @Output void
 */
void UART_Write(const char *str);

void uart_puts(const char *str);

void UART_SendInt(int32_t num);
void UART_SendInt0(int32_t num);
void UART_SendHex8(uint16_t num);
void UART_SendHex16(uint16_t num);
void UART_SendHex32(uint32_t num);

#endif	/* UARTTOOLS_H */

