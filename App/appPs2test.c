/* stm32f103 Timer one shot mode init test */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "ps2.h"

volatile uint32_t kbd_rxof = 0, mouse_rxof = 0;
// ----------------------------------------------------------------------------
void ps2_kbd_cbrxof(void)
{
  kbd_rxof++;
}

// ----------------------------------------------------------------------------
void ps2_mouse_cbrxof(void)
{
  mouse_rxof++;
}

// ----------------------------------------------------------------------------
void mainApp(void)
{
  uint32_t pre_kbd_rxof = 0, pre_mouse_rxof = 0;
  uint8_t ps2rt, ch, prebtn = 0;
  ps2_MouseData MouseData;

  printf("\r\nControl start\r\n");

  for(;;)
  {
    /* here we simulate the time of other activities in the program loop */
    HAL_Delay(30);

    /* get keyboard */
    ps2rt = ps2_kbd_getkey(&ch);
    if(ps2rt)
    { /* recevied keyboard data */
      if(ch >= 32)
        printf("Kbd: '%c', 0x%X\r\n", ch, ch);
      else
        printf("Kbd:       0x%X\r\n", ch);
    }

    /* keyboard RX buffer overflow detect */
    if(kbd_rxof > pre_kbd_rxof)
    {
      printf("keyboard rx buffer overflowed:%d\r\n", (unsigned int)kbd_rxof);
      pre_kbd_rxof = kbd_rxof;
    }

    /* get mouse */
    ps2rt = ps2_mouse_getmove(&MouseData);
    if(ps2rt)
    { /* recevied mouse data */
      if((prebtn != MouseData.btns) || (MouseData.xmove != 0) || (MouseData.ymove != 0) || (MouseData.zmove != 0))
        printf("mouse:%d, %d, %d, %X\r\n", MouseData.xmove, MouseData.ymove, MouseData.zmove, MouseData.btns);
      prebtn = MouseData.btns;
    }

    /* mouse RX buffer overflow detect */
    if(mouse_rxof > pre_mouse_rxof)
    {
      printf("mouse rx buffer overflowed:%d\r\n", (unsigned int)mouse_rxof);
      pre_mouse_rxof = mouse_rxof;
    }
  }
}
