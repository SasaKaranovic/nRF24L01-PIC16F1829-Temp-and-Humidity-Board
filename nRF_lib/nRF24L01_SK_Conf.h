#ifndef NRF24L01_SK_CONF_H
#define	NRF24L01_SK_CONF_H

#ifndef LOW
#define LOW     0
#endif
#ifndef HIGH
#define HIGH     1
#endif

#ifndef OUTPUT
#define OUTPUT     0
#endif
#ifndef INPUT
#define INPUT     1
#endif


//Define pins

#define csn_pin         TRISCbits.TRISC3
#define TRIS_NRF_CSN    TRISCbits.TRISC3
#define NRF_CSN         LATCbits.LATC3


#define ce_pin          TRISCbits.TRISC3
#define TRIS_NRF_CE     TRISCbits.TRISC6
#define NRF_CE          LATCbits.LATC6


//Try to somewhat maintain similarity with ported library
#define nrf_pinMode(x,y) (x=y)


#endif	/* NRF24L01_SK_CONF_H */

