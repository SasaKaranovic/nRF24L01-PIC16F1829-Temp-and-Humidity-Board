/* 
 * File:   i2cbysk.h
 * Author: Sale
 *
 * Created on March 20, 2014, 12:33 AM
 */

#ifndef I2CBYSK_H
#define	I2CBYSK_H

// Define i2c pins
#define SDA		LATBbits.LATB5		// Data pin for i2c
#define SCK		LATBbits.LATB7		// Clock pin for i2c
#define SDA_DIR		TRISBbits.TRISB5        // Data pin direction
#define SCK_DIR		TRISBbits.TRISB7        // Clock pin direction

// Define i2c speed
#define I2C_SPEED	100000              // bps

enum
{
    I2C1_ACK_NOT_RECEIVED,
    I2C1_SUCCESS,
    I2C1_BUS_COLLISION,
    I2C1_BUS_BUSY
}I2C1_STATE;

//Function Declarations
void i2c_init(void);
void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);
void i2c_waitIDLE();
void i2c_sendACK(void);
void i2c_sendNACK(void);
unsigned char i2c_sendByte(unsigned char);
unsigned char i2c_readByte(void);


uint8_t I2C1_WriteByte(uint8_t i2c1SlaveAddress, uint8_t i2c1Data);
uint8_t I2C1_WriteFrame(uint8_t i2c1SlaveAddress, uint8_t *i2c1WritePointer, uint8_t i2c1FrameLength);
uint8_t I2C1_ReadFrame(uint8_t i2c1SlaveAddress, uint8_t *i2c1ReadPointer, uint8_t i2c1FrameLength);

#endif	/* I2CBYSK_H */

