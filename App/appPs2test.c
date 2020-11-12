/* stm32f103 Timer one shot mode init test */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "ps2.h"

volatile uint32_t kbd_rxof = 0, mouse_rxof = 0;
// ----------------------------------------------------------------------------
void ps2_kbd_cbrx(uint8_t kbd_data_rx)
{
}

void ps2_kbd_cbrxerror(uint32_t rx_errorcode)
{
  if(rx_errorcode == PS2_ERROR_OVF)
  {
    printf("Kbd overflow error\r\n");
  }
  else if(rx_errorcode == PS2_ERROR_PARITY)
    printf("Kbd parity error\r\n");
}

// ----------------------------------------------------------------------------
void ps2_mouse_cbrx(uint32_t rx_datanum)
{
  printf("mouse packet size:%d\r\n", (unsigned int)rx_datanum);
}

void ps2_mouse_cbrxerror(uint32_t rx_errorcode)
{
  if(rx_errorcode == PS2_ERROR_OVF)
  {
    printf("Mouse overflow error\r\n");
  }
  else if(rx_errorcode == PS2_ERROR_PARITY)
    printf("Mouse parity error\r\n");
}

// ----------------------------------------------------------------------------
void mainApp(void)
{
  uint8_t ch, prebtn = 0;
  ps2_MouseData MouseData;

  printf("\r\nControl start\r\n");

  for(;;)
  {
    /* here we simulate the time of other activities in the program loop */
    HAL_Delay(30);

    /* get keyboard */
    while(ps2_kbd_getkey(&ch) == 1)
    { /* recevied keyboard data */
      if(ch >= 32)
        printf("Kbd: '%c', 0x%X\r\n", ch, ch);
      else
        printf("Kbd:       0x%X\r\n", ch);
    }

    /* get mouse */
    #if MOUSE_METHOD == 1
    if(ps2_mouse_getmove(&MouseData) == 1)
    #else
    while(ps2_mouse_getmove(&MouseData) == 1)
    #endif
    { /* recevied mouse data */
      if((prebtn != MouseData.btns) || (MouseData.xmove != 0) || (MouseData.ymove != 0) || (MouseData.zmove != 0))
        printf("mouse:%d, %d, %d, %X\r\n", MouseData.xmove, MouseData.ymove, MouseData.zmove, MouseData.btns);
      prebtn = MouseData.btns;
    }
  }
}
