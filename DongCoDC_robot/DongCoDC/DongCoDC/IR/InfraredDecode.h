/*==============================================================================
**                       www.codientu.org(dungdothe)
**             Proprietary - Copyright (C) 2013
**------------------------------------------------------------------------------
** Supported MCUs      : AVRs
** Supported Compilers : WinAVR, AVRStudio
**------------------------------------------------------------------------------
** File name         : InfraredDecode.h
** Generation date: Tuesday, June 25, 2013
=============================================================================*/

#include <avr/io.h>
#include <avr/interrupt.h>

/***** USER DEFINE: Change these value base on your hardware **********/
/* Define F_CPU in Khz, default is 12Mhz = 12000Khz */
#define F_CPU_Khz           12000
/* Led configure */
#define LED_PIN            PD6
#define LED_DDR            DDRD
#define LED_PORT           PORTD
/***** END USER DEFINE **********/

#ifndef TRUE
#define TRUE          1
#endif
#ifndef FALSE
#define FALSE         0
#endif
#ifndef BOOL
#define BOOL         uint8_t
#endif

/***** DEFINITION FOR KEY CODE  **********/
#define KEY_0    0
#define KEY_1    1
#define KEY_INVALID        0xFF

/***** DEFINITION FOR GLOBAL FUNCTIONS  **********/
void InfraredDecode_Init(void);
uint8_t InfraredDecode_GetKeyCode(void);
