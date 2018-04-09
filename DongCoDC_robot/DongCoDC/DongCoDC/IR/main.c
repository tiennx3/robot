#include "InfraredDecode.h"

#define LED1_PIN            PD7
#define LED1_DDR            DDRD
#define LED1_PORT           PORTD
#define led_toggle()        LED1_PORT ^= (1 << LED1_PIN)
int main(void)
{
  uint8_t code;

  InfraredDecode_Init();
  LED1_DDR |= (1 << LED1_PIN);
  /* Enable interrupts */
  sei();
  while(1)
  {
    code = InfraredDecode_GetKeyCode();
    
    switch (code)
    {
      case KEY_0:
        led_toggle();
        break;
      case KEY_1:
        led_toggle();
        break;
      default:
        break;
    }
  }
}
