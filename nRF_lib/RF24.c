/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "../main.h"
#include "nRF24L01.h"
#include "RF24_config.h"
#include "RF24.h"
#include "nRF24L01_SK_Conf.h"

/****************************************************************************/

void nrf_csn(int mode)
{
  // Minimum ideal SPI bus speed is 2x data rate
  // If we assume 2Mbs data rate and 16Mhz clock, a
  // divider of 4 is the minimum we want.
  // CLK:BUS 8Mhz:2Mhz, 16Mhz:4Mhz, or 20Mhz:5Mhz
#ifdef ARDUINO
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
#endif
  //digitalWrite(csn_pin,mode);
  NRF_CSN = mode;
}

/****************************************************************************/

void nrf_ce(int level)
{
  //digitalWrite(ce_pin,level);
    NRF_CE = level;
}

/****************************************************************************/

uint8_t nrf_read_register_len(uint8_t reg, uint8_t* buf, uint8_t len)
{
  uint8_t status;

  nrf_csn(LOW);
  status = spi_fast_shift( R_REGISTER | ( REGISTER_MASK & reg ) );
  while ( len-- )
    *buf++ = spi_fast_shift(0xff);

  nrf_csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t nrf_read_register(uint8_t reg)
{
  nrf_csn(LOW);
  spi_fast_shift( R_REGISTER | ( REGISTER_MASK & reg ) );
  uint8_t result = spi_fast_shift(0xff);

  nrf_csn(HIGH);
  return result;
}

/****************************************************************************/

uint8_t nrf_write_register_len(uint8_t reg, const uint8_t* buf, uint8_t len)
{
  uint8_t status;

  nrf_csn(LOW);
  status = spi_fast_shift( W_REGISTER | ( REGISTER_MASK & reg ) );
  while ( len-- )
    spi_fast_shift(*buf++);

  nrf_csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t nrf_write_register(uint8_t reg, uint8_t value)
{
  uint8_t status;

#ifdef IF_SERIAL_DEBUG
    printf("write_register(%02x,%02x)\r\n", reg, value);
#endif


  nrf_csn(LOW);
  status = spi_fast_shift( W_REGISTER | ( REGISTER_MASK & reg ) );
  spi_fast_shift(value);
  nrf_csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t nrf_write_payload(uint8_t *buf, uint8_t len)
{
  uint8_t status;

  //const uint8_t* current = reinterpret_cast<const uint8_t*>(buf);

  uint8_t data_len = min(len, payload_size);
  uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;
  
  //printf("[Writing %u bytes %u blanks]",data_len,blank_len);
  
  uint8_t i=0;
  
  nrf_csn(LOW);
  status = spi_fast_shift( W_TX_PAYLOAD );
  while ( data_len-- ){
    spi_fast_shift((unsigned char) sendPayload[i++]);
  }
  while ( blank_len-- ){
    spi_fast_shift(0);
  }
  nrf_csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t nrf_read_payload(uint8_t *buf, uint8_t len)
{
  uint8_t status;
  //uint8_t* current = reinterpret_cast<uint8_t*>(buf);

  uint8_t data_len = min(len,payload_size);
  uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;
  
  //printf("[Reading %u bytes %u blanks]",data_len,blank_len);
  uint8_t i =0;
  
  nrf_csn(LOW);
  status = spi_fast_shift( R_RX_PAYLOAD );
  while ( data_len-- )
    buf[i++] = spi_fast_shift(0xff);
  while ( blank_len-- )
    spi_fast_shift(0xff);
  nrf_csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t nrf_flush_rx(void)
{
  uint8_t status;

  nrf_csn(LOW);
  status = spi_fast_shift( FLUSH_RX );
  nrf_csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t nrf_flush_tx(void)
{
  uint8_t status;

  nrf_csn(LOW);
  status = spi_fast_shift( FLUSH_TX );
  nrf_csn(HIGH);

  return status;
}

/****************************************************************************/

uint8_t nrf_get_status(void)
{
  uint8_t status;

  nrf_csn(LOW);
  status = spi_fast_shift( NRF_NOP );
  nrf_csn(HIGH);

  return status;
}

/****************************************************************************/

void nrf_print_status(uint8_t status)
{
  printf("STATUS= 0x%02x \r\nRX_DR=%x \r\nTX_DS=%x \r\nMAX_RT=%x \r\nRX_P_NO=%x \r\nTX_FULL=%x\r\n\r\n",
           status,
           (status & _BV(RX_DR))    ?1:0,
           (status & _BV(TX_DS))    ?1:0,
           (status & _BV(MAX_RT))   ?1:0,
           ((status >> RX_P_NO) & 0b00000111),
           (status & _BV(TX_FULL))?1:0
          );
}

/****************************************************************************/

void nrf_print_observe_tx(uint8_t value)
{
  printf_P(PSTR("OBSERVE_TX=%02x: POLS_CNT=%x ARC_CNT=%x\r\n"),
           value,
           (value >> PLOS_CNT) & 0b00001111,
           (value >> ARC_CNT) & 0b00001111
          );
}

/****************************************************************************/

void nrf_print_byte_register(const char* name, uint8_t reg, uint8_t qty)
{
  char extra_tab = strlen_P(name) < 8 ? '\t' : 0;
  printf_P(PSTR(PRIPSTR"\t%c ="),name,extra_tab);
  while (qty--)
    printf_P(PSTR(" 0x%02x"), nrf_read_register(reg++));
  printf_P(PSTR("\r\n"));
}

/****************************************************************************/

void nrf_print_address_register(const char* name, uint8_t reg, uint8_t qty)
{
  char extra_tab = strlen_P(name) < 8 ? '\t' : 0;
  printf_P(PSTR(PRIPSTR"\t%c ="),name,extra_tab);

  while (qty--)
  {
    uint8_t buffer[5];
    nrf_read_register_len(reg++,buffer,sizeof buffer);

    printf_P(PSTR(" 0x"));
    uint8_t* bufptr = buffer + sizeof buffer;
    while( --bufptr >= buffer )
      printf_P(PSTR("%02x"),*bufptr);
  }

  printf_P(PSTR("\r\n"));
}

/****************************************************************************/

//RF24::RF24(uint8_t _cepin, uint8_t _cspin):
//  ce_pin(_cepin), csn_pin(_cspin), wide_band(true), p_variant(false), 
//  payload_size(32), ack_payload_available(false), dynamic_payloads_enabled(false),
//  pipe0_reading_address(0)
//{
//}

/****************************************************************************/

void nrf_setChannel(uint8_t channel)
{
  // TODO: This method could take advantage of the 'wide_band' calculation
  // done in setChannel() to require certain channel spacing.

  const uint8_t max_channel = 127;
  nrf_write_register(RF_CH, min(channel,max_channel));
}

/****************************************************************************/

void nrf_setPayloadSize(uint8_t size)
{
  const uint8_t max_payload_size = 32;
  payload_size = min(size,max_payload_size);
}

/****************************************************************************/

uint8_t nrf_getPayloadSize(void)
{
  return payload_size;
}

/****************************************************************************/

static const char rf24_datarate_e_str_0[] PROGMEM = "1MBPS";
static const char rf24_datarate_e_str_1[] PROGMEM = "2MBPS";
static const char rf24_datarate_e_str_2[] PROGMEM = "250KBPS";
static const char * const rf24_datarate_e_str_P[] PROGMEM = {
  rf24_datarate_e_str_0,
  rf24_datarate_e_str_1,
  rf24_datarate_e_str_2,
};
static const char rf24_model_e_str_0[] PROGMEM = "nRF24L01";
static const char rf24_model_e_str_1[] PROGMEM = "nRF24L01+";
static const char * const rf24_model_e_str_P[] PROGMEM = {
  rf24_model_e_str_0,
  rf24_model_e_str_1,
};
static const char rf24_crclength_e_str_0[] PROGMEM = "Disabled";
static const char rf24_crclength_e_str_1[] PROGMEM = "8 bits";
static const char rf24_crclength_e_str_2[] PROGMEM = "16 bits" ;
static const char * const rf24_crclength_e_str_P[] PROGMEM = {
  rf24_crclength_e_str_0,
  rf24_crclength_e_str_1,
  rf24_crclength_e_str_2,
};
static const char rf24_pa_dbm_e_str_0[] PROGMEM = "PA_MIN";
static const char rf24_pa_dbm_e_str_1[] PROGMEM = "PA_LOW";
static const char rf24_pa_dbm_e_str_2[] PROGMEM = "LA_MED";
static const char rf24_pa_dbm_e_str_3[] PROGMEM = "PA_HIGH";
static const char * const rf24_pa_dbm_e_str_P[] PROGMEM = { 
  rf24_pa_dbm_e_str_0,
  rf24_pa_dbm_e_str_1,
  rf24_pa_dbm_e_str_2,
  rf24_pa_dbm_e_str_3,
};

void nrf_printDetails(void)
{
  nrf_print_status(nrf_get_status());

  nrf_print_address_register("RX_ADDR_P0-1",RX_ADDR_P0,2);
  nrf_print_byte_register("RX_ADDR_P2-5",RX_ADDR_P2,4);
  nrf_print_address_register("TX_ADDR",TX_ADDR, 1);

  nrf_print_byte_register("RX_PW_P0-6",RX_PW_P0,6);
  nrf_print_byte_register("EN_AA",EN_AA, 1);
  nrf_print_byte_register("EN_RXADDR",EN_RXADDR, 1);
  nrf_print_byte_register("RF_CH",RF_CH, 1);
  nrf_print_byte_register("RF_SETUP",RF_SETUP, 1);
  nrf_print_byte_register("CONFIG",CONFIG, 1);
  nrf_print_byte_register("DYNPD/FEATURE",DYNPD,2);

  
  printf("Data Rate\t = ");
  switch(nrf_getDataRate()) {
      case 0:
          printf("1MBPS");
          break;
      case 1:
          printf("2MBPS");
          break;
      case 2:
          printf("250KBPS");
          break;
  }
  
  printf("\r\nCRC Length\t = ");
  switch(nrf_getCRCLength()) {
      case 0:
          printf("Disabled");
          break;
      case 1:
          printf("8 bits");
          break;
      case 2:
          printf("16 bits");
          break;
  }
  
  printf("\r\nPA Power\t\t = ");
  switch(nrf_getPALevel()) {
      case 0:
          printf("PA_MIN\r\n");
          break;
      case 1:
          printf("PA_LOW\r\n");
          break;
      case 2:
          printf("PA_MED\r\n");
          break;
      case 3:
          printf("PA_HIGH\r\n");
          break;
  }
  //printf_P(PSTR("Data Rate\t = %S\r\n"), rf24_datarate_e_str_P[nrf_getDataRate()]);
  //printf_P(PSTR("Model\t\t = %S\r\n"), rf24_model_e_str_P[nrf_isPVariant()]);
  //printf_P(PSTR("CRC Length\t = %S\r\n"), rf24_crclength_e_str_P[nrf_getCRCLength()]);
  //printf_P(PSTR("PA Power\t = %S\r\n"), rf24_pa_dbm_e_str_P[nrf_getPALevel()]);
}

/****************************************************************************/

void nrf_begin(void)
{
  // Initialize pins
  nrf_pinMode(ce_pin,OUTPUT);
  nrf_pinMode(csn_pin,OUTPUT);

  // Initialize SPI bus
  //SPI.begin();
  //Start your SPI before calling this function and you should be fine

  
  nrf_ce(LOW);
  nrf_csn(HIGH);

  // Must allow the radio time to settle else configuration bits will not necessarily stick.
  // This is actually only required following power up but some settling time also appears to
  // be required after resets too. For full coverage, we'll always assume the worst.
  // Enabling 16b CRC is by far the most obvious case if the wrong timing is used - or skipped.
  // Technically we require 4.5ms + 14us as a worst case. We'll just call it 5ms for good measure.
  // WARNING: Delay is based on P-variant whereby non-P *may* require different timing.
  __delay_ms( 5 ) ;

  // Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
  // WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
  // sizes must never be used. See documentation for a more complete explanation.
  nrf_write_register(SETUP_RETR,(0b00000100 << ARD) | (0b00001111 << ARC));

  // Restore our default PA level
  nrf_setPALevel( RF24_PA_MAX ) ;

  // Determine if this is a p or non-p RF24 module and then
  // reset our data rate back to default value. This works
  // because a non-P variant won't allow the data rate to
  // be set to 250Kbps.
  if( nrf_setDataRate( RF24_250KBPS ) )
  {
    p_variant = true ;
  }
  
  // Then set the data rate to the slowest (and most reliable) speed supported by all
  // hardware.
  nrf_setDataRate( RF24_1MBPS ) ;

  // Initialize CRC and request 2-byte (16bit) CRC
  nrf_setCRCLength( RF24_CRC_16 ) ;
  
  // Disable dynamic payloads, to match dynamic_payloads_enabled setting
  nrf_write_register(DYNPD,0);

  // Reset current status
  // Notice reset and flush is the last thing we do
  nrf_write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

  // Set up default configuration.  Callers can always change it later.
  // This channel should be universally safe and not bleed over into adjacent
  // spectrum.
  nrf_setChannel(76);

  // Flush buffers
  nrf_flush_rx();
  nrf_flush_tx();
}

/****************************************************************************/

void nrf_startListening(void)
{
    nrf_write_register(CONFIG, nrf_read_register(CONFIG) | _BV(PWR_UP) | _BV(PRIM_RX));
    nrf_write_register(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

    // Restore the pipe0 adddress, if exists
    if (pipe0_reading_address) {
      nrf_write_register_len(RX_ADDR_P0, pipe0_reading_address, 5);
    }
    // Flush buffers
    nrf_flush_rx();
    nrf_flush_tx();

    // Go!
    nrf_ce(HIGH);

    // wait for the radio to come up (130us actually only needed)
    __delay_us(130);
}

/****************************************************************************/

void nrf_stopListening(void)
{
  nrf_ce(LOW);
  nrf_flush_tx();
  nrf_flush_rx();
}

/****************************************************************************/

void nrf_powerDown(void)
{
  nrf_write_register(CONFIG, nrf_read_register(CONFIG) & ~_BV(PWR_UP));
}

/****************************************************************************/

void nrf_powerUp(void)
{
  nrf_write_register(CONFIG, nrf_read_register(CONFIG) | _BV(PWR_UP));
}

/******************************************************************/

bool nrf_write( uint8_t *buf, uint8_t len )
{
  bool result = false;

  // Begin the write
  nrf_startWrite(buf,len);

  // ------------
  // At this point we could return from a non-blocking write, and then call
  // the rest after an interrupt

  // Instead, we are going to block here until we get TX_DS (transmission completed and ack'd)
  // or MAX_RT (maximum retries, transmission failed).  Also, we'll timeout in case the radio
  // is flaky and we get neither.

  // IN the end, the send should be blocking.  It comes back in 60ms worst case, or much faster
  // if I tighted up the retry logic.  (Default settings will be 1500us.
  // Monitor the send
  uint8_t observe_tx;
  uint8_t status;
  //uint32_t sent_at = millis();
  uint32_t timeout = 500000; //ms to wait for timeout
  
  do
  {
    status = nrf_read_register_len(OBSERVE_TX, &observe_tx, 1);
    timeout--;
#ifdef IF_SERIAL_DEBUG
    //printf("0x%02X\r\n", observe_tx);
#endif
    
  }
  while( ! ( status & ( _BV(TX_DS) | _BV(MAX_RT) ) ) && ( timeout > 0) );

  // The part above is what you could recreate with your own interrupt handler,
  // and then call this when you got an interrupt
  // ------------

  // Call this when you get an interrupt
  // The status tells us three things
  // * The send was successful (TX_DS)
  // * The send failed, too many retries (MAX_RT)
  // * There is an ack packet waiting (RX_DR)
  bool tx_ok, tx_fail;
  nrf_whatHappened(&tx_ok, &tx_fail, &ack_payload_available);
  
  //printf("%u%u%u\r\n",tx_ok,tx_fail,ack_payload_available);

  result = tx_ok;
#ifdef IF_SERIAL_DEBUG
    printf(result?"...OK.\r\n":"...Failed\r\n");
#endif

  // Handle the ack packet
  if ( ack_payload_available )
  {
    ack_payload_length = nrf_getDynamicPayloadSize();
#ifdef IF_SERIAL_DEBUG
    printf("\r\n[AckPacket]/\r\n");
#endif
    
#ifdef IF_SERIAL_DEBUG
    printf("\r\n[AckPacket]/\r\n");
#endif

    //IF_SERIAL_DEBUG(printfln(ack_payload_length,DEC));
  }

  // Yay, we are done.

  // Power down
  nrf_powerDown();

  // Flush buffers (Is this a relic of past experimentation, and not needed anymore??)
  nrf_flush_tx();

  return result;
}
/****************************************************************************/

void nrf_startWrite( uint8_t *buf, uint8_t len )
{
  // Transmitter power-up
  nrf_write_register(CONFIG, ( nrf_read_register(CONFIG) | _BV(PWR_UP) ) & ~_BV(PRIM_RX) );
  __delay_us(150);

  // Send the payload
  nrf_write_payload( buf, len );

  // Allons!
  nrf_ce(HIGH);
  __delay_us(15);
  nrf_ce(LOW);
}

/****************************************************************************/

uint8_t nrf_getDynamicPayloadSize(void)
{
  uint8_t result = 0;

  nrf_csn(LOW);
  spi_fast_shift( R_RX_PL_WID );
  result = spi_fast_shift(0xff);
  nrf_csn(HIGH);

  return result;
}

/****************************************************************************/

//bool nrf_available()
//{
//  return nrf_available(NULL);
//}

/****************************************************************************/

bool nrf_available(uint8_t *pipe_num)
{
  uint8_t status = nrf_get_status();

  // Too noisy, enable if you really want lots o data!!
  //IF_SERIAL_DEBUG(print_status(status));

  bool result = ( status & _BV(RX_DR) );

  if (result)
  {
    // If the caller wants the pipe number, include that
    if ( pipe_num )
      *pipe_num = ( status >> RX_P_NO ) & 0b00000111;

    // Clear the status bit

    // ??? Should this REALLY be cleared now?  Or wait until we
    // actually READ the payload?

    nrf_write_register(STATUS,_BV(RX_DR) );

    // Handle ack payload receipt
    if ( status & _BV(TX_DS) )
    {
      nrf_write_register(STATUS,_BV(TX_DS));
    }
  }

  return result;
}

/****************************************************************************/

bool nrf_read( void* buf, uint8_t len )
{
  // Fetch the payload
  nrf_read_payload( buf, len );

  // was this the last of the data available?
  return nrf_read_register(FIFO_STATUS) & _BV(RX_EMPTY);
}

/****************************************************************************/

void nrf_whatHappened(bool *tx_ok,bool *tx_fail,bool *rx_ready)
{
  // Read the status & reset the status in one easy call
  // Or is that such a good idea?
  uint8_t status = nrf_write_register(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

  // Report to the user what happened
  *tx_ok = status & _BV(TX_DS);
  *tx_fail = status & _BV(MAX_RT);
  *rx_ready = status & _BV(RX_DR);
}

/****************************************************************************/

void nrf_openWritingPipe(uint8_t *address)
{
  // Note that AVR 8-bit uC's store this LSB first, and the NRF24L01(+)
  // expects it LSB first too, so we're good.

  nrf_write_register_len(RX_ADDR_P0, address, 5);
  nrf_write_register_len(TX_ADDR, address, 5);

  const uint8_t max_payload_size = 32;
  nrf_write_register(RX_PW_P0,min(payload_size,max_payload_size));
}

/****************************************************************************/

static const uint8_t child_pipe[] PROGMEM =
{
  RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5
};
static const uint8_t child_payload_size[] PROGMEM =
{
  RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5
};
static const uint8_t child_pipe_enable[] PROGMEM =
{
  ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5
};

void nrf_openReadingPipe(uint8_t child, uint8_t *address)
{
  // If this is pipe 0, cache the address.  This is needed because
  // openWritingPipe() will overwrite the pipe 0 address, so
  // startListening() will have to restore it.
  if (child == 0) {
        pipe0_reading_address[0] = address[0];
        pipe0_reading_address[1] = address[1];
        pipe0_reading_address[2] = address[2];
        pipe0_reading_address[3] = address[3];
        pipe0_reading_address[4] = address[4];
  }
  if (child <= 6)
  {
    // For pipes 2-5, only write the LSB
    if ( child < 2 )
      nrf_write_register_len(child_pipe[child], address, 5);
    else
      nrf_write_register_len(child_pipe[child], address, 1);

    nrf_write_register(child_payload_size[child], payload_size);

    // Note it would be more efficient to set all of the bits for all open
    // pipes at once.  However, I thought it would make the calling code
    // more simple to do it this way.
    nrf_write_register(EN_RXADDR, nrf_read_register(EN_RXADDR) | _BV(child_pipe_enable[child]));
  }
}

/****************************************************************************/

void nrf_toggle_features(void)
{
  nrf_csn(LOW);
  spi_fast_shift( ACTIVATE );
  spi_fast_shift( 0x73 );
  nrf_csn(HIGH);
}

/****************************************************************************/

void nrf_enableDynamicPayloads(void)
{
  // Enable dynamic payload throughout the system
  nrf_write_register(FEATURE, nrf_read_register(FEATURE) | _BV(EN_DPL) );

  // If it didn't work, the features are not enabled
  if ( ! nrf_read_register(FEATURE) )
  {
    // So enable them and try again
    nrf_toggle_features();
    nrf_write_register(FEATURE, nrf_read_register(FEATURE) | _BV(EN_DPL) );
  }

#ifdef IF_SERIAL_DEBUG
    printf("FEATURE=%i\r\n", nrf_read_register(FEATURE));
#endif

  // Enable dynamic payload on all pipes
  //
  // Not sure the use case of only having dynamic payload on certain
  // pipes, so the library does not support it.
  nrf_write_register(DYNPD, nrf_read_register(DYNPD) | _BV(DPL_P5) | _BV(DPL_P4) | _BV(DPL_P3) | _BV(DPL_P2) | _BV(DPL_P1) | _BV(DPL_P0));

  dynamic_payloads_enabled = true;
}

/****************************************************************************/

void nrf_enableAckPayload(void)
{
  //
  // enable ack payload and dynamic payload features
  //

  nrf_write_register(FEATURE, nrf_read_register(FEATURE) | _BV(EN_ACK_PAY) | _BV(EN_DPL) );

  // If it didn't work, the features are not enabled
  if ( ! nrf_read_register(FEATURE) )
  {
    // So enable them and try again
    nrf_toggle_features();
    nrf_write_register(FEATURE, nrf_read_register(FEATURE) | _BV(EN_ACK_PAY) | _BV(EN_DPL) );
  }

#ifdef IF_SERIAL_DEBUG
    printf("FEATURE=%i\r\n", nrf_read_register(FEATURE));
#endif

  //
  // Enable dynamic payload on pipes 0 & 1
  //

  nrf_write_register(DYNPD, nrf_read_register(DYNPD) | _BV(DPL_P1) | _BV(DPL_P0));
}

/****************************************************************************/

void nrf_writeAckPayload(uint8_t pipe, const void* buf, uint8_t len)
{
  const uint8_t *current = (buf);

  nrf_csn(LOW);
  spi_fast_shift( W_ACK_PAYLOAD | ( pipe & 0b0111 ) );
  const uint8_t max_payload_size = 32;
  uint8_t data_len = min(len,max_payload_size);
  while ( data_len-- )
    spi_fast_shift(*current++);

  nrf_csn(HIGH);
}

/****************************************************************************/

bool nrf_isAckPayloadAvailable(void)
{
  bool result = ack_payload_available;
  ack_payload_available = false;
  return result;
}

/****************************************************************************/

bool nrf_isPVariant(void)
{
  return p_variant ;
}

/****************************************************************************/

void nrf_setAutoAck_all(bool enable)
{
  if ( enable )
    nrf_write_register(EN_AA, 0b111111);
  else
    nrf_write_register(EN_AA, 0);
}

/****************************************************************************/

void nrf_setAutoAck( uint8_t pipe, bool enable)
{
  if ( pipe <= 6 )
  {
    uint8_t en_aa = nrf_read_register( EN_AA ) ;
    if( enable )
    {
      en_aa |= _BV(pipe) ;
    }
    else
    {
      en_aa &= ~_BV(pipe) ;
    }
    nrf_write_register( EN_AA, en_aa ) ;
  }
}

/****************************************************************************/

bool nrf_testCarrier(void)
{
  return ( nrf_read_register(CD) & 1 );
}

/****************************************************************************/

bool nrf_testRPD(void)
{
  return ( nrf_read_register(RPD) & 1 ) ;
}

/****************************************************************************/

void nrf_setPALevel(rf24_pa_dbm_e level)
{
  uint8_t setup = nrf_read_register(RF_SETUP) ;
  setup &= ~(_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;

  // switch uses RAM (evil!)
  if ( level == RF24_PA_MAX )
  {
    setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;
  }
  else if ( level == RF24_PA_HIGH )
  {
    setup |= _BV(RF_PWR_HIGH) ;
  }
  else if ( level == RF24_PA_LOW )
  {
    setup |= _BV(RF_PWR_LOW);
  }
  else if ( level == RF24_PA_MIN )
  {
    // nothing
  }
  else if ( level == RF24_PA_ERROR )
  {
    // On error, go to maximum PA
    setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;
  }

  nrf_write_register( RF_SETUP, setup ) ;
}

/****************************************************************************/

rf24_pa_dbm_e nrf_getPALevel(void)
{
  rf24_pa_dbm_e result = RF24_PA_ERROR ;
  uint8_t power = nrf_read_register(RF_SETUP) & (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;

  // switch uses RAM (evil!)
  if ( power == (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) )
  {
    result = RF24_PA_MAX ;
  }
  else if ( power == _BV(RF_PWR_HIGH) )
  {
    result = RF24_PA_HIGH ;
  }
  else if ( power == _BV(RF_PWR_LOW) )
  {
    result = RF24_PA_LOW ;
  }
  else
  {
    result = RF24_PA_MIN ;
  }

  return result ;
}

/****************************************************************************/

bool nrf_setDataRate(rf24_datarate_e speed)
{
  bool result = false;
  uint8_t setup = nrf_read_register(RF_SETUP) ;

  // HIGH and LOW '00' is 1Mbs - our default
  wide_band = false ;
  setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH)) ;
  if( speed == RF24_250KBPS )
  {
    // Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
    // Making it '10'.
    wide_band = false ;
    setup |= _BV( RF_DR_LOW ) ;
  }
  else
  {
    // Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
    // Making it '01'
    if ( speed == RF24_2MBPS )
    {
      wide_band = true ;
      setup |= _BV(RF_DR_HIGH);
    }
    else
    {
      // 1Mbs
      wide_band = false ;
    }
  }
  nrf_write_register(RF_SETUP,setup);

  // Verify our result
  if ( nrf_read_register(RF_SETUP) == setup )
  {
    result = true;
  }
  else
  {
    wide_band = false;
  }

  return result;
}

/****************************************************************************/

rf24_datarate_e nrf_getDataRate( void )
{
  rf24_datarate_e result ;
  uint8_t dr = nrf_read_register(RF_SETUP) & (_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));
  
  // switch uses RAM (evil!)
  // Order matters in our case below
  if ( dr == _BV(RF_DR_LOW) )
  {
    // '10' = 250KBPS
    result = RF24_250KBPS ;
  }
  else if ( dr == _BV(RF_DR_HIGH) )
  {
    // '01' = 2MBPS
    result = RF24_2MBPS ;
  }
  else
  {
    // '00' = 1MBPS
    result = RF24_1MBPS ;
  }
  return result ;
}

/****************************************************************************/

void nrf_setCRCLength(rf24_crclength_e length)
{
  uint8_t config = nrf_read_register(CONFIG) & ~( _BV(CRCO) | _BV(EN_CRC)) ;
  
  // switch uses RAM (evil!)
  if ( length == RF24_CRC_DISABLED )
  {
    // Do nothing, we turned it off above. 
  }
  else if ( length == RF24_CRC_8 )
  {
    config |= _BV(EN_CRC);
  }
  else
  {
    config |= _BV(EN_CRC);
    config |= _BV( CRCO );
  }
  nrf_write_register( CONFIG, config ) ;
}

/****************************************************************************/

rf24_crclength_e nrf_getCRCLength(void)
{
  rf24_crclength_e result = RF24_CRC_DISABLED;
  uint8_t config = nrf_read_register(CONFIG) & ( _BV(CRCO) | _BV(EN_CRC)) ;

  if ( config & _BV(EN_CRC ) )
  {
    if ( config & _BV(CRCO) )
      result = RF24_CRC_16;
    else
      result = RF24_CRC_8;
  }

  return result;
}

/****************************************************************************/

void nrf_disableCRC( void )
{
  uint8_t disable = nrf_read_register(CONFIG) & ~_BV(EN_CRC) ;
  nrf_write_register( CONFIG, disable ) ;
}

/****************************************************************************/
void nrf_setRetries(uint8_t delay, uint8_t count)
{
 nrf_write_register(SETUP_RETR,(delay&0xf)<<ARD | (count&0xf)<<ARC);
}

// vim:ai:cin:sts=2 sw=2 ft=cpp

