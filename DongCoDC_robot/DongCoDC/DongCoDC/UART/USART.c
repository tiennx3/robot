#include "USART.h"
/*
*USART initialize, page 308
0. Default F_CPU = 12000000UL (12MHz)
1. Set up baud, using UBRR = UBRRH and UBRRL
  1.1  UBRR = (F_CPU/(16* BAUDRATE)) - 1;
  1.2 if we want double bit -> UCSRA = _BV(U2X). Note only in asynchronous
2. Set up framem, default is  Asynchronous mode, 1 Start bit , 8 data bits , no parity, 1 stop bit,
  UCSRC = (1 << URSEL) |(3<<UCSZ0)
3. Enable receiver and transmitter
  UCSRB = (1 << RXEN) | (1 << TXEN)
4.  baud == 115200 --> USART_Init(BAUD)
    else --> Change baudrate from globalConfigure and then USART_Init(MYUBRR)
*/
void USART_Init (void)
{
#if (BAUD == BAUD_115200)  
  UBRRH = 0;
  UBRRL = 12;
  //double bit
  UCSRA = _BV(U2X); 
#else
  UBRRH = (unsigned char)(UBRR_BAUD >> 8);
  UBRRL = (unsigned char)(UBRR_BAUD);  
#endif /**/
  //Asynchronous mode, 1 Start bit , 8 data bits , no parity, 1 stop bit
  UCSRC = (1 << URSEL) |(3<<UCSZ0);
  //Enable receiver and transmitter
  UCSRB = (1<<RXCIE)|(1 << RXEN)| (1 << TXEN);
}

void USART_Transmit(uint8_t data)
{
    /* Wait for empty transmit buffer */
  while ( !( UCSRA & (1<<UDRE)) )
  ;
  /* Put data into buffer, sends the data */
  UDR = data;
}

uint8_t USART_Receive( void )
{
  /* Wait for data to be received */
  while ( !(UCSRA & (1<<RXC)) )
  ;
  /* Get and return received data from buffer */
  return UDR;
}
