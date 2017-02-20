/*


  _________                      ____  __.                                      .__        
 /   _____/____    ___________  |    |/ _|____ ____________    ____   _______  _|__| ____  
 \_____  \\__  \  /  ___/\__  \ |      < \__  \\_  __ \__  \  /    \ /  _ \  \/ /  |/ ___\ 
 /        \/ __ \_\___ \  / __ \|    |  \ / __ \|  | \// __ \|   |  (  <_> )   /|  \  \___ 
/_______  (____  /____  >(____  /____|__ (____  /__|  (____  /___|  /\____/ \_/ |__|\___  >
        \/     \/     \/      \/        \/    \/           \/     \/                    \/ 


Description: 
				Very simple example of how to use nRF24L01 and PIC16F1829 to send
				humidity and temperature values from Si7021 sensor over wireless network.
				For simplicity this code does address following features: low-power, encryption,
				handshaking, mesh network, network discovery, network healing. 
				Please approach your IoT application seriously and implement those features
				properly and responsibly.
				
				If you end up using this code or parts of it, I would love to hear from you
				and see what did you end up making.
				
URL: 			http://sasakaranovic.com/portfolio/new-project-sensor-board-for-smart-home/
*/
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
    UART_Debug("----------------\r\n");
    UART_Debug("Sensor Board\r\n");
    UART_Debug("Firmware version: 1.2\r\n");
    UART_Debug("----------------\r\n");
    __delay_ms(500);
    StatusLED(LED_ALL);
    
	//nRF24 Configuration
    wide_band                   = true;
    p_variant                   = false;
    payload_size                = 32;
    ack_payload_available       = true;
    dynamic_payloads_enabled    = true;
    
    //nRF24 Init and Start
    nrf_begin();
    nrf_enableDynamicPayloads();
    nrf_setAutoAck_all(1);
    nrf_setDataRate(RF24_250KBPS);
    nrf_setPALevel(RF24_PA_MAX);
    nrf_setChannel(70);
    nrf_setRetries(15,15);
    nrf_setCRCLength(RF24_CRC_8);
    
    //nRF24 Writting address
    uint8_t nRF_pipe_writing[5] = {0xe1, 0xf0, 0xf0, 0xf0, 0xf0};
    //nRF24 Reading address
    uint8_t nRF_pipe_reading[5] = {0x31, 0x76, 0x72, 0x65, 0x73};
    
	//Open read and write pipes
    nrf_openWritingPipe(nRF_pipe_writing);
    nrf_openReadingPipe(1, nRF_pipe_reading);

    StatusLED(LED_ONLINE);
    
    i2c_init();
    
    UART_Debug("Printf is working\r\n");

	//Send configuration details over UART
	//Disable when not testing/debugging
    //nrf_printDetails();
    

    __delay_ms(2000);
    
    
    uint8_t i;
	
	
    //Very simple example
    while(1) {
        StatusLED(LED_ONLINE);	//Turn on On/Off LED
        nrf_powerUp();			//Power up nRF (wake up)
        __delay_ms(50);			//delay so the module has some time to set-up
        
        sb_readTemperature();	//Read temperature value from Si7021 sensor
        sb_readHumidity();		//Read humidity value from Si7021 sensor
        
		//Other side is expecting payload formated as follows: 	ID	:	Type	:	Value1	:	Value2
		//														E1	:	TH		:	27.00	:	20.00
        sprintf(sendPayload, "%02X:TH:%d.%02d:%d.%02d", BOARD_ID, si7021_tempNum, si7021_tempDec, si7021_humidNum, si7021_humidDec);	//Format string
        printf("Sending data: %s\r\n", sendPayload);	//Avoid printf! This is just a simple example

        StatusLED(LED_WORKING);	//Turn on Activity LED
		
        
        nrf_stopListening();	// Stop listening and write to radio
        __delay_ms(10);
        
		
        // Try to send data to hub
        if ( nrf_write( sendPayload, 32) ) {
            UART_Debug("Send successful\n\r");	//Uart debug message
            StatusLED(LED_ONLINE);				//Set Activity LED to success
        } 
        else {
            UART_Debug("Send failed\n\r");		//Uart debug message
            StatusLED(LED_FAIL);				//Set Activity LED to fail
        }
     
    
        
        
        nrf_startListening();					//wait response
        __delay_ms(20);
		
		
		//Read all available data from reading pipe
        while ( nrf_available(nRF_pipe_reading) ) 		
		{
            uint8_t len = nrf_getDynamicPayloadSize();
            nrf_read( receivePayload, len);

            __delay_ms(10);
        } // End while
        
        
        __delay_ms(1000);						//Delay a bit before going to sleep
        StatusLED(LED_ONLINE);					//Set device On/Off LED
        

		//Implement your low-power sleep routine if device is battery powered or you want to preserve energy
		//in this example we will just wait for ~2minutes (1s * 120) and then reread humidity and temperature
		//and send it back to the hub
        for(i=0; i<120; i++) {
            __delay_ms(1000);
        }
        
    }
}
