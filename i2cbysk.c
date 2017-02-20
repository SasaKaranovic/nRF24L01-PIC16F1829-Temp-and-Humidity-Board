#include "main.h"

/************* THIS IS THE USER GENERATED I2C SOURCE FILE ************************************/



// Function Purpose: Configure I2C module
void i2c_init(void)
{
    ANSELB              = 0x50;
    WPUB = 0x00;

    LATBbits.LATB5      = 0;
    LATBbits.LATB7      = 0;
    TRISBbits.TRISB5    = 1;
    TRISBbits.TRISB7    = 1;


    // BF RCinprocess_TXcomplete; UA dontupdate; SMP Standard Speed; P stopbit_notdetected; S startbit_notdetected; R_nW write_noTX; CKE Idle to Active; D_nA lastbyte_address;
    SSP2STAT = 0x80;
    // SSPEN enabled; WCOL no_collision; SSPOV no_overflow; CKP Idle:Low, Active:High; SSPM FOSC/4_SSPxADD;
    SSP2CON1 = 0x28;
    // BOEN disabled; AHEN disabled; SBCDE disabled; SDAHT 300ns; DHEN disabled; ACKTIM ackseq; PCIE disabled; SCIE disabled;
    SSP2CON3 = 0x08;
    // Baud Rate Generator Value: SSPADD 79;
    SSP2ADD = 0x4F;

    /* Byte sent or received */
    // clear the master interrupt flag
    PIR4bits.SSP2IF = 0;
}


void i2c_start() {
    SSP2CON2bits.SEN = 1;
    while(SSP2CON2bits.SEN);
}

void i2c_restart() {
    SSP2CON2bits.RSEN = 1;
    while(SSP2CON2bits.RSEN);
}

void i2c_stop() {
    SSP2CON2bits.PEN = 1;
    while(SSP2CON2bits.PEN);
}

void i2c_waitIDLE() {
    while( SSP2STATbits.R_nW | SSP2CON2bits.SEN | SSP2CON2bits.RSEN |  SSP2CON2bits.PEN |  SSP2CON2bits.RCEN | SSP2CON2bits.ACKEN ) {};
    // | SSP2CON2bits.ACKEN
}

void i2c_sendACK() {
    SSP2CON2bits.ACKDT = 0;
    SSP2CON2bits.ACKEN = 1;
}

void i2c_sendNACK() {
    SSP2CON2bits.ACKDT = 1;
    SSP2CON2bits.ACKEN = 1;
}

unsigned char i2c_sendByte(unsigned char data) {
    i2c_waitIDLE();
    SSP2BUF = data;
    i2c_waitIDLE();

    return (unsigned char)(!SSP2CON2bits.ACKSTAT);
}

unsigned char i2c_readByte() {
    SSP2CON2bits.RCEN = 1;
    while (SSP2CON2bits.RCEN == 1);
    return SSP2BUF;
}



/**
  Prototype:        uint8_t I2C1_ReadFrame(uint8_t i2c1SlaveAddress, uint8_t *i2c1ReadPointer, uint8_t i2c1FrameLength)
  Input:            i2c1SlaveAddress : Address of slave sending data.
                    *i2c1ReadPointer : Pointer to starting location in file register where data is written.
                    i2c1FrameLength : Number of bytes to receive.
  Output:           none
  Description:      This function is used to read from the I2C bus and store into the file register from
                    the starting location passed as an argument. This is a blocking function and will wait until
                    all the data is received.
  Usage:            I2C1_ReadFrame(i2c1SlaveAddress, (char *)i2c1ReadPointer, i2c1FrameLength);
*/
uint8_t I2C1_ReadFrame(uint8_t i2c1SlaveAddress, uint8_t *i2c1ReadPointer, uint8_t i2c1FrameLength)
{
    if(SSP2STATbits.S)
    {
        return I2C1_BUS_BUSY;
    }

    // initiate start condition
    SSP2CON2bits.SEN = 1;
    while (SSP2CON2bits.SEN)
    {
    }

    //check for bus collision
    if(PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }

    // send slave address with Read/Write bit set
    SSP2BUF = i2c1SlaveAddress | 0x01;
    while ((SSP2STATbits.BF || SSP2STATbits.R_nW) && !PIR2bits.BCL1IF);

    //check for bus collision
    if(PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }

    // check for acknowledgement status
    if (SSP2CON2bits.ACKSTAT)
    {
        SSP2CON2bits.PEN = 1;
        while(SSP2CON2bits.PEN)
		{
		}
        return I2C1_ACK_NOT_RECEIVED;
    }


    while (i2c1FrameLength)
    {
        // receive byte of data
        SSP2CON2bits.RCEN = 1;
        while(SSP2CON2bits.RCEN)
	{
	}
        *i2c1ReadPointer++ = SSP2BUF;

        // set acknowledgement status
        if(i2c1FrameLength == 1)
        {
            SSP2CON2bits.ACKDT = 1;
        }
        else
        {
            SSP2CON2bits.ACKDT = 0;
        }
        // send acknowledgement
        SSP2CON2bits.ACKEN = 1;
        while (SSP2CON2bits.ACKEN)
	{
	}
        i2c1FrameLength--;

        //check for bus collision
        if (PIR2bits.BCL1IF)
        {
            PIR2bits.BCL1IF = 0;
            return I2C1_BUS_COLLISION;
        }

    }
    // initiate stop condition
    SSP2CON2bits.PEN = 1;
    while (SSP2CON2bits.PEN)
    {
    }

    //check for bus collision
    if (PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF=0;
        return I2C1_BUS_COLLISION;
    }
    return I2C1_SUCCESS;
}

/**
  Prototype:        uint8_t I2C1_WriteFrame(uint8_t i2c1SlaveAddress, uint8_t *i2c1WritePointer, uint8_t i2c1FrameLength)
  Input:            i2c1SlaveAddress : Address of slave receiving data.
		    *i2c1WritePointer : Pointer to starting location in file register from where data is read.
                    i2c1FrameLength : Number of bytes to send.
  Output:           none
  Description:      This function is used to write into the I2C bus. This is a blocking function and will wait until
                    all the data is send.
  Usage:            I2C1_WriteFrame(i2c1SlaveAddress, (char *)i2c1WritePointer, i2c1FrameLength);
*/
uint8_t I2C1_WriteFrame(uint8_t i2c1SlaveAddress, uint8_t *i2c1WritePointer, uint8_t i2c1FrameLength)
{
   if (SSP2STATbits.S)
   {
      return I2C1_BUS_BUSY;
   }

    // initiate start condition
    SSP2CON2bits.SEN = 1;
    while (SSP2CON2bits.SEN)
    {
    }

    //check for bus collision
    if (PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }

    //write address into the buffer
    SSP2BUF = i2c1SlaveAddress;
    while (SSP2STATbits.BF || SSP2STATbits.R_nW)
    {
    }

    //Check for acknowledgement status
    if (SSP2CON2bits.ACKSTAT )
    {
        SSP2CON2bits.PEN = 1;
        while (SSP2CON2bits.PEN)
	{
	}
        return I2C1_ACK_NOT_RECEIVED;
    }

    while (i2c1FrameLength)
    {
        //write byte into the buffer
        SSP2BUF = *i2c1WritePointer++;
        while (SSP2STATbits.BF || SSP2STATbits.R_nW)
	{
	}

        //Check for acknowledgement status
        if ( SSP2CON2bits.ACKSTAT )
        {
            SSP2CON2bits.PEN = 1;
            while (SSP2CON2bits.PEN)
            {
            }
            return I2C1_ACK_NOT_RECEIVED;
        }

        // check for bus collision
        if (PIR2bits.BCL1IF)
        {
            PIR2bits.BCL1IF = 0;
            return I2C1_BUS_COLLISION;
        }
        i2c1FrameLength-- ;
    }

    //initiate stop condition
    SSP2CON2bits.PEN = 1;
    while (SSP2CON2bits.PEN)
    {
    }

    //check for bus collision
    if(PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }
    return I2C1_SUCCESS;
}

/**
  Prototype:        uint8_t I2C1_WriteByte(uint8_t i2c1SlaveAddress, uint8_t i2c1Data)
  Input:            i2c1SlaveAddress : Address of slave receiving data.
		    i2c1Data : data to be send.
  Output:           none
  Description:      This function is used to write into the I2C bus. This is a blocking function and will wait until
                    the data byte is send.
  Usage:            I2C1_WriteByte(i2c1SlaveAddress, i2c1Data);
*/
uint8_t I2C1_WriteByte(uint8_t i2c1SlaveAddress, uint8_t i2c1Data)
{
   if (SSP2STATbits.S)
   {
       return I2C1_BUS_BUSY;
   }

    // initiate start condition
    SSP2CON2bits.SEN = 1;
    while (SSP2CON2bits.SEN)
    {
    }

    //check for bus collision
    if (PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }

    //write address into the buffer
    SSP2BUF = i2c1SlaveAddress;
    while (SSP2STATbits.BF || SSP2STATbits.R_nW)
    {
    }

    //Check for acknowledgement status
    if (SSP2CON2bits.ACKSTAT )
    {
        SSP2CON2bits.PEN = 1;
        while (SSP2CON2bits.PEN)
	{
	}
        return I2C1_ACK_NOT_RECEIVED;
    }

    //write byte into the buffer
    SSP2BUF = i2c1Data;
    while (SSP2STATbits.BF || SSP2STATbits.R_nW)
    {
    }

    //Check for acknowledgement status
    if ( SSP2CON2bits.ACKSTAT )
    {
        SSP2CON2bits.PEN = 1;
        while (SSP2CON2bits.PEN)
	{
	}
        return I2C1_ACK_NOT_RECEIVED;
    }

    // check for bus collision
    if (PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }

    //initiate stop condition
    SSP2CON2bits.PEN = 1;
    while (SSP2CON2bits.PEN)
    {
    }

    //check for bus collision
    if(PIR2bits.BCL1IF)
    {
        PIR2bits.BCL1IF = 0;
        return I2C1_BUS_COLLISION;
    }
    return I2C1_SUCCESS;
}
