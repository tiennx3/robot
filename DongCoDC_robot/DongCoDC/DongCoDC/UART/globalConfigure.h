#ifndef __GLOBAL_CONFIGURE_H
#define __GLOBAL_CONFIGURE_H



#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define __disable_interrupt()     cli()
#define __enable_interrupt()      sei()

/****************** LED1 ***********************/
#define LED1_PIN                        PD6
#define LED1_DDR                        DDRD
#define LED1_PORT                       PORTD

/****************** LED2 ***********************/
#define LED2_PIN                        PD7
#define LED2_DDR                        DDRD
#define LED2_PORT                       PORTD

/******************* USART *************************/
#define BAUD_9600                       9600UL
#define BAUD_115200                     115200UL
#define BAUD                            BAUD_115200
#define UBRR_BAUD                       (F_CPU/(16*BAUD))-1

#endif /*__GLOBAL_CONFIGURE_H*/
