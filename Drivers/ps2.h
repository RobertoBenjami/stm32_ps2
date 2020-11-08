#ifndef __PS2_H__
#define __PS2_H__

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
/* PS2 keyboard and mouse driver for stm32xxxx
     author  : Roberto Benjami
     version : 2020.11.04

   For settings note:
   - PS2_IRQPRIORITY: keyboard EXTI, mouse EXTI, timer interrupt priority (0..15)
       note: 0 = the highest priority, 15 = the lowest priority

   - PS2_TIM: the timer number used for the timers
       note: which one you choose depends on the processor family you are using, 
	         look at the processor-specific header

   - PS2_GETTIME(): get milliseconds function name
       note: (HAL_GetTick() or  osKernelSysTick() or ... )

   - PS2_KBDCLK, PS2_KBDDATA, PS2_MOUSECLK, PS2_MOUSEDATA: port name, pin number
       note: if not used -> X, 0

   - KBDRBUF_SIZE, KBDTBUF_SIZE, MOUSERBUF_SIZE, MOUSETBUF_SIZE: buffer size (8,16,32,64,128,256,512,1024,2048,...)
     - KBDRBUF_SIZE: recommended minimum 32
     - KBDTBUF_SIZE: enough 8
     - MOUSERBUF_SIZE: see MOUSE_METHOD note
     - MOUSETBUF_SIZE: see MOUSE_METHOD note
       note: the buffer size should be (2 ^ n) !

   - MOUSE_METHOD: mouse get move method
     - 1: waiting for the mouse to respond
     - 2: returns the result of the previous query immediately and then starts the new query
     - 3: enable the continuous mouse reporting
       note: method 1 and 2: enough MOUSERBUF_SIZE number = 8
           method 3: recommended MOUSERBUF_SIZE >= 32
           all method: enough MOUSETBUF_SIZE number = 8

   - uint8_t ps2_kbd_getkey(uint8_t * kbd_key) : get one character from keyboard buffer
       note: if return = 0 -> there was no keyboard event
             if return = 1 -> &kbd_key = asc code

   - uint8_t ps2_kbd_getscan(uint8_t * kbd_scan) : get one scancode
       note: if return = 0 -> there was no keyboard event
             if return = 1 -> &kbd_scan = scan code

   - uint8_t ps2_kbd_ctrlstatus(void) : get the modify buttons status
       return = buttons status (see the modify buttons statusbits)

   - uint8_t ps2_kbd_lockstatus(void) : get the keyboard lock status (caps lock, num lock, scroll lock)
       return = keyboard lock status (see the lock buttons statusbits and leds)

   - uint8_t ps2_kbd_setlocks(uint8_t kbd_locks) : set the keyboard lock status
       param: lock status (see the lock buttons statusbits and leds)

   - void ps2_kbd_cbrxof: if you want to know that an keyboard RX buffer is overflowed, do a function with that name
       note: if this function is is activated, keyboard RX data loss has occurred
             attention, it will be operated from an interruption!

   - uint8_t ps2_mouse_getmove(ps2_MouseData * mouse_data) : get the mouse move and mouse buttons status
       param: pointer to ps2_MouseData type variable
       if return = 0 -> there was no mouse event
       if return = 1 -> the mouse event data is placed in the variable (see typedef ps2_MouseData)

   - void ps2_mouse_cbrxof: if you want to know that an mouse RX buffer is overflowed, do a function with that name
       note: if this function is is activated, mouse RX data loss has occurred
             attention, it will be operated from an interruption!
*/

// ============================================================================
/* Configurations chapter */

#define PS2_IRQPRIORITY   15

#define PS2_TIM            3

#define PS2_GETTIME()   HAL_GetTick()

/* PS2 pin settings and buffer size definitions (size: 2^n) */
#define PS2_KBDCLK      B, 7
#define PS2_KBDDATA     B, 6
#define KBDRBUF_SIZE      32
#define KBDTBUF_SIZE       8

#define PS2_MOUSECLK    B, 4
#define PS2_MOUSEDATA   A, 15
#define MOUSERBUF_SIZE    64
#define MOUSETBUF_SIZE     8

#define MOUSE_METHOD       3

// ============================================================================
/* Fix chapter */

/* keyboard key codes */
#define PS2_TAB              9
#define PS2_ENTER           13
#define PS2_LINEFEED        10
#define PS2_BACKSPACE      127
#define PS2_ESC             27
#define PS2_INSERT         157
#define PS2_DELETE         127
#define PS2_HOME           158
#define PS2_END            159
#define PS2_PAGEUP          25
#define PS2_PAGEDOWN        26
#define PS2_UPARROW         11
#define PS2_LEFTARROW        8
#define PS2_DOWNARROW       12
#define PS2_RIGHTARROW      21
#define PS2_F1             145
#define PS2_F2             146
#define PS2_F3             147
#define PS2_F4             148
#define PS2_F5             149
#define PS2_F6             150
#define PS2_F7             151
#define PS2_F8             152
#define PS2_F9             153
#define PS2_F10            154
#define PS2_F11            155
#define PS2_F12            156
#define PS2_SCROLL           0

/* keyboard lock buttons scan code */
#define SCN_SCRLOCK       0x7E
#define SCN_NUMLOCK       0x77
#define SCN_CAPSLOCK      0x58

/* keyboard modify buttons statusbits (ctrl, shift, alt, altgr) */
#define ST_KBDBREAK       0x01
#define ST_KBDMODIFIER    0x02
#define ST_KBDSHIFT_L     0x04
#define ST_KBDSHIFT_R     0x08
#define ST_KBDALTGR       0x10
#define ST_KBDCTRL        0x20

/* keyboard lock buttons statusbits (capslock, numlock, scrollock) */
#define ST_KBDSCRLOCK     0x01
#define ST_KBDNUMLOCK     0x02
#define ST_KBDCAPSLOCK    0x04

//-----------------------------------------------------------------------------
/* keyboard */
uint8_t ps2_kbd_getkey(uint8_t * kbd_key);    /* if return == 1 -> key code read */
uint8_t ps2_kbd_getscan(uint8_t * kbd_scan);  /* if return == 1 -> scan code read */
uint8_t ps2_kbd_ctrlstatus(void);             /* return: see the modify buttons statusbits */
uint8_t ps2_kbd_lockstatus(void);             /* return: see the lock buttons statusbits (and leds) */
uint8_t ps2_kbd_setlocks(uint8_t kbd_locks);  /* see the lock buttons statusbits (and leds) */
__weak  void ps2_kbd_cbrxof(void);

//-----------------------------------------------------------------------------
/* mouse */
typedef struct
{
  int16_t  xmove;
  int16_t  ymove;
  int16_t  zmove;
  int8_t   btns;
}ps2_MouseData;

uint8_t ps2_mouse_getmove(ps2_MouseData * mouse_data);
__weak  void ps2_mouse_cbrxof(void);

#ifdef __cplusplus
}
#endif

#endif
