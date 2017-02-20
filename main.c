#include "main.h"

void main(void) {
    Device_SetUp();
    Device_Initialize();
    Timer1_Setup();
    spi_init();
    __delay_ms(1000);
    UART_Debug("Device initialized.\r\n");
    StatusLED(LED_ALL);
    __delay_ms(500);
    StatusLED(LED_OFF);
    __delay_ms(500);
    StatusLED(LED_ONLINE);
    //UART_Debug("Device initialized.\n");
    UART_Debug("----------------\r\n");
    UART_Debug("Sensor Board\r\n");
    UART_Debug("Firmware version: 1.2\r\n");
    UART_Debug("----------------\r\n");
    __delay_ms(500);
    StatusLED(LED_ALL);
    
    
    /*
    wide_band(true), 
    p_variant(false), 
    payload_size(32), 
    ack_payload_available(false), 
    dynamic_payloads_enabled(false),
    pipe0_reading_address(0)
     */
    wide_band                   = true;
    p_variant                   = false;
    payload_size                = 32;
    ack_payload_available       = true;
    dynamic_payloads_enabled    = true;
    
    //nRF24
    nrf_begin();
    nrf_enableDynamicPayloads();
    nrf_setAutoAck_all(1);
    nrf_setDataRate(RF24_250KBPS);
    nrf_setPALevel(RF24_PA_MAX);
    nrf_setChannel(70);
    nrf_setRetries(15,15);
    nrf_setCRCLength(RF24_CRC_8);
    
    //0xf0 f0 f0 f0 e1
    //uint8_t nRF_pipe_writing[5] = {0xf0, 0xf0, 0xf0, 0xf0, 0xe1};
    uint8_t nRF_pipe_writing[5] = {0xe1, 0xf0, 0xf0, 0xf0, 0xf0};
    //0x73 65 72 76 31 LL
    //uint8_t nRF_pipe_reading[5] = {0x73, 0x65, 0x72, 0x76, 0x31};
    uint8_t nRF_pipe_reading[5] = {0x31, 0x76, 0x72, 0x65, 0x73};
    
    nrf_openWritingPipe(nRF_pipe_writing);
    nrf_openReadingPipe(1, nRF_pipe_reading);

    StatusLED(LED_ONLINE);
    
    i2c_init();
    
    printf("Printf is working\r\n");

    //nrf_printDetails();
    
    
//    sb_readTemperature();
//    sb_readHumidity();
//  
//    printf("Temp: %d.%02d\r\nHumid %d.%02d\r\n", si7021_tempNum, si7021_tempDec, si7021_humidNum, si7021_humidDec);
//    
//    while(1);

    __delay_ms(2000);
    
    
    uint8_t i;
    
    while(1) {
        StatusLED(LED_ONLINE);
        nrf_powerUp();
        __delay_ms(50);
        
        sb_readTemperature();
        sb_readHumidity();
        //E1:TH:27.00:20.00
        sprintf(sendPayload, "%02X:TH:%d.%02d:%d.%02d", BOARD_ID, si7021_tempNum, si7021_tempDec, si7021_humidNum, si7021_humidDec);
        printf("Sending data: %s\r\n", sendPayload);

        StatusLED(LED_WORKING);
        // Stop listening and write to radio
        nrf_stopListening();
        __delay_ms(10);
        
        // Send to hub
        if ( nrf_write( sendPayload, 32) ) {
            UART_Debug("Send successful\n\r");
            StatusLED(LED_ONLINE);
        } 
        else {
            UART_Debug("Send failed\n\r");
            StatusLED(LED_FAIL);
        }
     
    
        
        //wait response
        nrf_startListening();
        
        __delay_ms(20);
        while ( nrf_available(nRF_pipe_reading) ) {
            uint8_t len = nrf_getDynamicPayloadSize();
            nrf_read( receivePayload, len);

            __delay_ms(10);
        } // End while
        
        
        __delay_ms(1000);
        StatusLED(LED_ONLINE);
        

        for(i=0; i<120; i++) {
            __delay_ms(1000);
        }
        
    }
}
