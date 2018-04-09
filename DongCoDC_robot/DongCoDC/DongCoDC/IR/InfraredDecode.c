/*==============================================================================
**                       www.codientu.org(dungdothe)
**             Proprietary - Copyright (C) 2013
**------------------------------------------------------------------------------
** Supported MCUs      : AVRs
** Supported Compilers : WinAVR, AVRStudio
**------------------------------------------------------------------------------
** File name         : InfraredDecode.c
** Generation date: Tuesday, June 25, 2013
=============================================================================*/

#include "InfraredDecode.h"
#include <avr/pgmspace.h>

#define OFFSET                  5
#define MAX_KEY_CODE_IDX        8

/***** KEY_CODE TABLE **********/
const uint8_t PROGMEM KeyCode_Table[MAX_KEY_CODE_IDX] = {
 0x69, 0x00, 0x10, 0xA4,
 0x49, 0x00, 0x10, 0xA4
};

/***** LOCAL VARIABLES  **********/
volatile uint16_t MAX = 0;
volatile uint16_t MIN = 0;
volatile uint8_t nIdx = 0;
volatile uint16_t startBitsCode[4] = {0, 0, 0, 0};
volatile BOOL bMatchStartCode0 = FALSE;
volatile BOOL bMatchStartCode1 = FALSE;
volatile BOOL bMatchStartCode2 = FALSE;
volatile uint32_t keycode = 0;
volatile uint8_t keycode_idx = 0;
volatile uint16_t stick = 0;

/***** LOCAL FUNCTIONS  **********/
BOOL compare_equal(uint16_t a, uint16_t b);

/***** FUNCTION IMPLEMENTATION **********/
void InfraredDecode_Init(void)
{
  /*** Led configuration ***/
  LED_DDR |= (1 << LED_PIN);
  /*** Timer1: Input capture, Output Compare Mode (CTC) ***/
  TCCR1A = 0x00; /* Normal port operation, OC1A/OC1B disconnected */
  TCCR1B =  ((1 << ICNC1)|  /* Activates the Input Capture Noise Canceler */
            (0 << ICES1)|  /* Input Capture Edge Select: falling edge */
            (1 << WGM12)|  /* CTC mode */
            (0 << CS12)|(0 << CS11)|(1 << CS10)); /* PRESCALE = 1 */
  /****** Set interrupt 100us *********/
  /* fo = 12Mhz / 1 = 12000Khz */
  /* ==> OCR1A = 0.1ms * 12000Khz/PRESCALE */
  OCR1A = F_CPU_Khz / 10;
  TIMSK |= (1 << TICIE1);  /* Input Capture Interrupt Enable */
}

void ClearVars(void)
{
  bMatchStartCode0 = FALSE;
  bMatchStartCode1 = FALSE;
  bMatchStartCode2 = FALSE;
  MAX = 0;
  MIN = 0;
  keycode = 0;
  keycode_idx = 0;
}

void TimerStop(void)
{
  LED_PORT &= ~(1 << LED_PIN); /* turn off led */
  TCNT1 = 0;
  stick = 0;
  nIdx = 0;
  ClearVars();
  TIFR |= (1 << ICF1) | (1 << OCF1A);
  TIMSK &= ~(1 << OCIE1A);
}

ISR(TIMER1_CAPT_vect)
{
  uint8_t sreg;
  uint16_t nStick = 0;
  nStick = stick; /* logging stick*/
  /* Save Global Interrupt Flag*/
  sreg = SREG;
  /* Disable interrupts */
  cli();
  LED_PORT |= (1 << LED_PIN); /* turn on led */
  if (nIdx > 0) //Ignore the first ISR
  {
    if (nIdx <= 4) // Start Bit Codes
    {
      /* Save Start Bits Code */
      startBitsCode[nIdx - 1] = nStick;
    }
    else
    {
      /* Check if overlap */
      if (compare_equal(nStick, startBitsCode[0]))
      {
        bMatchStartCode0 = TRUE;
      }
      else if ((bMatchStartCode0 == TRUE) && (compare_equal(nStick, startBitsCode[1])))
      {
        bMatchStartCode0 = FALSE;
        bMatchStartCode1 = TRUE;
      }
      else if ((bMatchStartCode1 == TRUE) && (compare_equal(nStick, startBitsCode[2])))
      {
        bMatchStartCode0 = FALSE;
        bMatchStartCode1 = FALSE;
        bMatchStartCode2 = TRUE;
      }
      else if ((bMatchStartCode2 == TRUE) && (compare_equal(nStick, startBitsCode[3])))
      {
        ClearVars();
      }
      else /* Not overlap*/
      {
        if (MIN == 0)
        {
          if (MAX == 0)
          {
            MAX = nStick;
          }
          else
          {
            if (compare_equal(nStick, MAX) == FALSE)
            {
              if (nStick < MAX)
              {
                MIN = nStick;
                keycode <<= 1;
                keycode |= 1;
                keycode <<= 1;
              }
              else
              {
                MIN = MAX;
                MAX = nStick;
                keycode |= 1;
                keycode <<= 2;
              }
              keycode_idx += 2;
            }
          }
        }
        /* we have max, min --> so we calculate KeyCode */
        else if ((MIN > 0) && (MAX > 0) && (keycode_idx < 31))
        {
          if (compare_equal(nStick, MIN))
          {
            keycode <<= 1;
            keycode_idx++;
          }
          else if (compare_equal(nStick, MAX))
          {
            keycode |= 1;
            keycode <<= 1;
            keycode_idx++;
          }
          else
          {
            /* do nothing */
          }
       }
      }
    }
  }
  TCNT1 = 0;
  stick = 0;
  TIMSK |= (1 << OCIE1A);   /* Output Compare A Match Interrupt Enable */
  nIdx++;                   /* Incre number of bits */
  /* Restore Global Interrupt Flag*/
  SREG = sreg;
}

BOOL compare_equal(uint16_t a, uint16_t b)
{
  BOOL ret;
  ret = ((a <= (b + OFFSET)) && (a >= (b - OFFSET)));
  return ret;
}

ISR(TIMER1_COMPA_vect)
{
  stick++;
}

uint8_t InfraredDecode_GetKeyCode(void)
{
  uint32_t keycodeMemory = 0;
  uint8_t key = KEY_INVALID;
  uint8_t i = 0;
  uint8_t idx = 0;
  BOOL bExitLoop = FALSE;
  if ((stick >= 500))
  {
    for(i = 0; ((i < MAX_KEY_CODE_IDX) && (bExitLoop == FALSE)); i++)
    {
      keycodeMemory = pgm_read_byte(&KeyCode_Table[i++]);
      keycodeMemory = (keycodeMemory << 8) | pgm_read_byte(&KeyCode_Table[i++]);
      keycodeMemory = (keycodeMemory << 8) | pgm_read_byte(&KeyCode_Table[i++]);
      keycodeMemory = (keycodeMemory << 8) | pgm_read_byte(&KeyCode_Table[i]);
      if (keycodeMemory == keycode)
      {
        key = idx;
        bExitLoop = TRUE;
      }
      idx++;
    }
    TimerStop();
  }
  return key;
}

