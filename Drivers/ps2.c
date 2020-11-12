/* PS/2 keyboard and mouse interface driver
   author: Roberto Benjami
   version: 2020.11.12
*/

#include <stdint.h>
#include "main.h"
#include "ps2.h"
#include "ps2_codepage.h"

#if defined(STM32F0)
#include "ps2_stm32f0xx.h"
#elif defined(STM32F1)
#include "ps2_stm32f1xx.h"
#elif defined(STM32F2)
#include "ps2_stm32f2xx.h"
#elif defined(STM32F3)
#include "ps2_stm32f3xx.h"
#elif defined(STM32F4)
#include "ps2_stm32f4xx.h"
#elif defined(STM32F7)
#include "ps2_stm32f7xx.h"
#elif defined(STM32H7)
#include "ps2_stm32h7xx.h"
#elif defined(STM32G0)
#include "ps2_stm32g0xx.h"
#elif defined(STM32G4)
#include "ps2_stm32g0xx.h"
#elif defined(STM32L0)
#include "ps2_stm32l0xx.h"
#elif defined(STM32L1)
#include "ps2_stm32l1xx.h"
#elif defined(STM32L4)
#include "ps2_stm32l4xx.h"
#elif defined(STM32L5)
#include "ps2_stm32l5xx.h"
#else
#error unknown processor family
#endif

// ============================================================================
/* Configurations chapter */

/* - printf debug off: 0
   - printf debug on:  1 */
#define PS2_PRINTF_DEBUG        0

/* - pin debug off: 0
   - pin debug irq: 1 (PS2_PIN_DEBUG_1 <- EXT interrupt, PS2_PIN_DEBUG_2 <- TIM interrupt)
   - pin debug processor usage time: 2 (PS2_PIN_DEBUG_1 <- keyboard, PS2_PIN_DEBUG_2 <- mouse) */
#define PS2_PIN_DEBUG           0

/* pin debug pins (only if PS2_PIN_DEBUG == 1) */
#define PS2_PIN_DEBUG_1      E, 0
#define PS2_PIN_DEBUG_2      E, 1

/* - clock filter off: 0
 * - clock filter on:  1 */
#define PS2_CLOCKFILTER         1

/* start clock impulse (microsecond) */
#define PS2_STARTIMPULSEWIDTH 800

/* timeout constans (millisecond for timeout) */
#define PS2_MOUSE_RESETTIME   750
#define PS2_MOUSE_IDTIME       50
#define PS2_MOUSE_RATETIME     50
#define PS2_MOUSE_READTIME     80

// ============================================================================
#if   PS2_PRINTF_DEBUG == 0
#define  ps2_printf(args...)
#elif PS2_PRINTF_DEBUG == 1
#define  ps2_printf(args...)  printf(args)
#endif

//-----------------------------------------------------------------------------
/* if the pin not definied */
#ifndef PS2_KBDCLK
#define PS2_KBDCLK            X, 0
#endif
#ifndef PS2_KBDDATA
#define PS2_KBDDATA           X, 0
#endif
#ifndef PS2_MOUSECLK
#define PS2_MOUSECLK          X, 0
#endif
#ifndef PS2_MOUSEDATA
#define PS2_MOUSEDATA         X, 0
#endif

//-----------------------------------------------------------------------------
/* Keyboard config */
#if (GPIOX_PORTNUM(PS2_KBDCLK) >= GPIOX_PORTNUM_A) && (GPIOX_PORTNUM(PS2_KBDDATA) >= GPIOX_PORTNUM_A)

#if KBDRBUF_SIZE < 8
#error KBDRBUF SIZE too small
#elif ((KBDRBUF_SIZE & (KBDRBUF_SIZE-1)) != 0)
#error KBDRBUF SIZE is not equal to 2 ^ n
#endif

#if KBDTBUF_SIZE < 8
#error KBDTBUF SIZE too small
#elif ((KBDTBUF_SIZE & (KBDTBUF_SIZE-1)) != 0)
#error KBDTBUF SIZE is not equal to 2 ^ n
#endif

#endif

// ----------------------------------------------------------------------------
/* Mouse config */
#if (GPIOX_PORTNUM(PS2_MOUSECLK) >= GPIOX_PORTNUM_A) && (GPIOX_PORTNUM(PS2_MOUSEDATA) >= GPIOX_PORTNUM_A)

#if MOUSERBUF_SIZE < 8
#error MOUSERBUF SIZE too small
#elif ((MOUSERBUF_SIZE & (MOUSERBUF_SIZE-1)) != 0)
#error MOUSERBUF SIZE is not equal to 2 ^ n
#endif

#if MOUSETBUF_SIZE < 8
#error MOUSETBUF SIZE too small
#elif ((MOUSETBUF_SIZE & (MOUSETBUF_SIZE-1)) != 0)
#error MOUSETBUF SIZE is not equal to 2 ^ n
#endif

#endif

// ----------------------------------------------------------------------------
/* Keyboard + Mouse */
#if (PS2_KBD_EXT_N >= 1) && (PS2_MOUSE_EXT_N >= 1)

#if (GPIOX_PIN(PS2_KBDCLK) == GPIOX_PIN(PS2_MOUSECLK))
#error "The keyboard and mouse CLK pin not possible in on the same pin number!"
#endif

#if PS2_KBD_EXT_N == PS2_MOUSE_EXT_N
#define PS2_KBD_MOUSE_COMMON_EXT_IRQ
#endif

#define PS2_TIM_VAR      uint8_t timermode
#define PS2_TIM_OFF      ps2s->timermode = 0
#define PS2_TIM_ON       ps2s->timermode = 1
#define PS2_KBDTIM_OFF   kbd.timermode = 0
#define PS2_KBDTIM_ON    kbd.timermode = 1
#define PS2_MOUSETIM_OFF mouse.timermode = 0
#define PS2_MOUSETIM_ON  mouse.timermode = 1

#else
/* Only keyboard or only mouse */

#define PS2_TIM_VAR
#define PS2_TIM_OFF
#define PS2_TIM_ON
#define PS2_KBDTIM_OFF
#define PS2_KBDTIM_ON
#define PS2_MOUSETIM_OFF
#define PS2_MOUSETIM_ON

#endif

void ps2_init(void);

// ============================================================================
/* PS2 status */
typedef enum
{
  PASSIVE = 0,  /* no sending, no receiving */
  REC,          /* receiving */
  SENDSTART,    /* start of sending */
  SEND,         /* sending */
  POST          /* end of sending or receiving */
} s_ps2s;

typedef struct
{
  void              (*cb_rx)(uint8_t, uint8_t); /* rx fifo write */
  uint8_t           (*cb_tx)(uint8_t *);/* tx fifo read */
  volatile s_ps2s   status;             /* ps2 status */
  GPIO_TypeDef *    clockport;          /* clock pin GPIO address */
  GPIO_TypeDef *    dataport;           /* data pin GPIO address */
  uint16_t          clockpinmask;       /* clock pin GPIO bitmask for BSRR, IDR */
  uint16_t          datapinmask;        /* data pin GPIO bitmask for BSRR, IDR */
  uint8_t           bitcount;
  uint8_t           data;
  uint8_t           databit;
  uint8_t           error;
  uint8_t           parity;
  PS2_TIM_VAR;                          /* 0: timer interrupt is not active, 1: active */
} t_Ps2;

#define FIFO_LEN(buf)                   (buf.in - buf.out)
#define FIFO_EMPTY(buf)                 (buf.in == buf.out)
#define FIFO_NOTEMPTY(buf)              (buf.in != buf.out)
#define FIFO_FULL(buf, bufsize)         (((buf.in - buf.out) & ~(bufsize - 1)) != 0)
#define FIFO_NOTFULL(buf, bufsize)      (((buf.in - buf.out) & ~(bufsize - 1)) == 0)
#define FIFO_WRITE(buf, bufsize, wd8)   buf.data[buf.in++ & (bufsize - 1)] = wd8
#define FIFO_READ(buf, bufsize, rd8)    rd8 = buf.data[buf.out++ & (bufsize - 1)]


// ----------------------------------------------------------------------------
uint8_t ps2_inited = 0;                 /* 0: not intited (must be called the ps2_init), 1: after init */

inline void ps2_initcheck(void)
{
  if(!ps2_inited)
  {
    ps2_init();
    ps2_inited = 1;
  }
}

// ============================================================================
/* Keyboard */
#if  PS2_KBD_EXT_N >= 1

volatile uint8_t  ps2_kbdlockstatus;    /* the lock and led bits: 0,0,0,0,0,capslock,numlock,scrollock */
volatile uint8_t  ps2_kbdctrlstatus;

void     cb_ps2_kbdrx(uint8_t rxdata, uint8_t error);
uint8_t  cb_ps2_kbdtx(uint8_t * txdata);

struct kbdbuf_r
{
  uint32_t in;                /* Next In Index */
  uint32_t out;               /* Next Out Index */
  char data[KBDRBUF_SIZE];    /* Buffer data */
};

struct kbdbuf_t
{
  uint32_t in;                /* Next In Index */
  uint32_t out;               /* Next Out Index */
  char data[KBDTBUF_SIZE];    /* Buffer data */
};

static struct kbdbuf_r kbdrbuf = {0, 0,};
static struct kbdbuf_t kbdtbuf = {0, 0,};

t_Ps2   kbd = {cb_ps2_kbdrx, cb_ps2_kbdtx, 0, 0, 0, 0, PASSIVE};
volatile uint8_t  kbd_rx_error = 0;

__weak  void ps2_kbd_cbrx(uint8_t rx_data) { }
__weak  void ps2_kbd_cbrxerror(uint32_t rx_errorcode) { }

// ----------------------------------------------------------------------------
uint8_t ps2_kbd_datawrite(uint8_t kbd_data)
{
  if(FIFO_FULL(kbdtbuf, KBDTBUF_SIZE))
    return 0;

  FIFO_WRITE(kbdtbuf, KBDTBUF_SIZE, kbd_data);  /* Add data to the transmit buffer. */
  ps2_printf("kt:%X\r\n", (unsigned int)kbd_data);

  if(kbd.status == PASSIVE)             /* can I send now? */
  {
    GPIOX_CLR(PS2_KBDCLK);              /* CLK = 0 */
    TIM_RESTART;
    PS2_KBDTIM_ON;                      /* Timer active */
    kbd.status = SENDSTART;
    ps2_printf("kts\r\n");
  }
  return 1;
}

// ----------------------------------------------------------------------------
uint8_t ps2_kbd_dataread(uint8_t * kbd_data)
{
  if(FIFO_NOTEMPTY(kbdrbuf))
  { /* not empty */
    FIFO_READ(kbdrbuf, KBDRBUF_SIZE, *kbd_data);
    ps2_printf("kr:%X\r\n", (unsigned int)*kbd_data);
    return 1;
  }
  else
  { /* empty */
    return 0;
  }
}

// ----------------------------------------------------------------------------
/* PS2 keyboard tx data get from tx fifo buffer (if txdata == NULL -> only tx buffer data info) */
uint8_t cb_ps2_kbdtx(uint8_t * txdata)
{
  if(FIFO_NOTEMPTY(kbdtbuf))
  {
    if(txdata)
    {
      FIFO_READ(kbdtbuf, KBDTBUF_SIZE, *txdata);
      ps2_printf("kct:%X\r\n", (unsigned int)*txdata);
    }
    return 1;
  }
  else
    return 0;
}

// ----------------------------------------------------------------------------
/* PS2 keyboard rx data store to rx fifo buffer */
void cb_ps2_kbdrx(uint8_t rxdata, uint8_t error)
{
  static uint8_t predata = 0;
  if(error)
  {
    kbd_rx_error = 1;
  }

  if(FIFO_NOTFULL(kbdrbuf, KBDRBUF_SIZE))
  {
    FIFO_WRITE(kbdrbuf, KBDRBUF_SIZE, rxdata);
    ps2_printf("kcr:%X\r\n", (unsigned int)rxdata);
  }
  else
  {
    ps2_kbd_cbrxerror(PS2_ERROR_OVF);
    ps2_printf("kcr:full!!\r\n");
  }

  if((predata != 0xF0) && (rxdata != 0xFA))
  { /* LED change */
    if(rxdata == SCN_CAPSLOCK)
    { /* capslock */
      ps2_kbdlockstatus ^= ST_KBDCAPSLOCK;
    }
    else if(rxdata == SCN_NUMLOCK)
    { /* numlock */
      ps2_kbdlockstatus ^= ST_KBDNUMLOCK;
    }
    else if(rxdata == SCN_SCRLOCK)
    { /* scrlock */
      ps2_kbdlockstatus ^= ST_KBDSCRLOCK;
    }
    else
    {
      predata = rxdata;
      return;
    }
    ps2_printf("key lock:%X\r\n", (unsigned int)ps2_kbdlockstatus);
    ps2_kbd_datawrite(0xED);
    ps2_kbd_datawrite(ps2_kbdlockstatus);
  }
  predata = rxdata;
  ps2_kbd_cbrx(rxdata);
}


// ----------------------------------------------------------------------------
/* low level ps2 keyboard init */
static inline void ps2_kbdinit(void)
{
  GPIOX_SET(PS2_KBDCLK);                /* CLK = 1 */
  GPIOX_ODOUT(PS2_KBDCLK);              /* CLK = OD out */

  GPIOX_SET(PS2_KBDDATA);               /* DATA = 1 */
  GPIOX_ODOUT(PS2_KBDDATA);             /* DATA = OD out */

  EXTI_INIT(PS2_KBDCLK);

  kbd.clockport = GPIOX(PS2_KBDCLK);
  kbd.dataport  = GPIOX(PS2_KBDDATA);
  kbd.clockpinmask = 1 << (GPIOX_PIN(PS2_KBDCLK));
  kbd.datapinmask = 1 << (GPIOX_PIN(PS2_KBDDATA));

  NVIC->ISER[(((uint32_t)(int32_t)PS2_KBD_EXT_IRQ) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)PS2_KBD_EXT_IRQ) & 0x1FUL));
  NVIC->IP[((uint32_t)(int32_t)PS2_KBD_EXT_IRQ)] = (uint8_t)((PS2_IRQPRIORITY << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
}

#endif  // #if  PS2_KBD_EXT_N >= 1

// ----------------------------------------------------------------------------
/* mouse low level */
#if  PS2_MOUSE_EXT_N >= 1

struct mousebuf_r
{
  volatile unsigned int in;             /* Next In Index */
  volatile unsigned int out;            /* Next Out Index */
  char data[MOUSERBUF_SIZE];            /* Buffer */
};

struct mousebuf_t
{
  volatile unsigned int in;             /* Next In Index */
  volatile unsigned int out;            /* Next Out Index */
  char data[MOUSETBUF_SIZE];            /* Buffer */
};

static struct mousebuf_r mouserbuf = {0, 0,};
static struct mousebuf_t mousetbuf = {0, 0,};
uint8_t       read_packet_size = 0;
volatile uint8_t mouse_rx_error = 0;

__weak  void ps2_mouse_cbrx(uint32_t rx_datanum) { }
__weak  void ps2_mouse_cbrxerror(uint32_t rx_errorcode) { }

// ----------------------------------------------------------------------------
/* ps2 mouse tx data read from tx fifo buffer */
uint8_t cb_ps2_mousetx(uint8_t * txdata)
{
  if(FIFO_NOTEMPTY(mousetbuf))
  { /* not empty */
    if(txdata)
    {
      FIFO_READ(mousetbuf, MOUSETBUF_SIZE, *txdata);
      ps2_printf("mct:%X\r\n", (unsigned int)*txdata);
    }
    return 1;
  }
  else
    return 0;
}

// ----------------------------------------------------------------------------
/* ps2 mouse rx data store to rx fifo buffer (callback) */
void cb_ps2_mouserx(uint8_t rxdata, uint8_t error)
{
  if(error)
  {
    mouse_rx_error = 1;
    ps2_mouse_cbrxerror(PS2_ERROR_PARITY);
    ps2_printf("mcr:parity!\r\n");
  }

  static uint8_t read_packet_cnt = 0;
  if(FIFO_NOTFULL(mouserbuf, MOUSERBUF_SIZE))
  {
    FIFO_WRITE(mouserbuf, MOUSERBUF_SIZE, rxdata);
    if(++read_packet_cnt >= read_packet_size)
    {
      ps2_mouse_cbrx(read_packet_cnt);
      read_packet_cnt = 0;
      ps2_printf("mcrp:%X\r\n", (unsigned int)rxdata);
    }
    else
      ps2_printf("mcr:%X\r\n", (unsigned int)rxdata);
  }
  else
  {
    ps2_mouse_cbrxerror(PS2_ERROR_OVF);
    ps2_printf("mcr:full!!\r\n");
  }
}

// ----------------------------------------------------------------------------

t_Ps2   mouse = { cb_ps2_mouserx, cb_ps2_mousetx, 0, 0, 0, 0, PASSIVE };

// ----------------------------------------------------------------------------
/* low level data read from ps2 mouse
   - param: pointer to 8 bits data
   - return: 0 = no data available, 1 = data received
   - note: if no data available -> the mouse_data does not change */
uint8_t ps2_mouse_dataread(uint8_t * mouse_data)
{
  if(FIFO_NOTEMPTY(mouserbuf))
  { /* not empty */
    FIFO_READ(mouserbuf, MOUSERBUF_SIZE, *mouse_data);
    ps2_printf("mr:%X\r\n", (unsigned int)*mouse_data);
    return 1;
  }
  else
  { /* empty */
    return 0;
  }
}

// ----------------------------------------------------------------------------
/* low level data write to ps2 mouse
   - param: 8 bits data
   - return: 0 = write buffer full, 1 = ok */
uint8_t ps2_mouse_datawrite(uint8_t mouse_data)
{
  if(FIFO_FULL(mousetbuf, MOUSETBUF_SIZE))
    return 0;

  FIFO_WRITE(mousetbuf, MOUSETBUF_SIZE, mouse_data);
  ps2_printf("mt:%X\r\n", (unsigned int)mouse_data);

  if(mouse.status == PASSIVE)           /* can I send now? */
  {
    GPIOX_CLR(PS2_MOUSECLK);            /* CLK = 0 */
    PS2_TIM->CNT = 0;                   /* Timer counter = 0 */
    PS2_TIM->CR1 |= TIM_CR1_CEN;
    PS2_MOUSETIM_ON;
    mouse.status = SENDSTART;
  }
  return 1;
}

// ----------------------------------------------------------------------------
/* low level ps2 mouse init */
static inline void ps2_mouseinit(void)
{
  GPIOX_SET(PS2_MOUSECLK);              /* CLK = 1 */
  GPIOX_ODOUT(PS2_MOUSECLK);
  GPIOX_SET(PS2_MOUSEDATA);             /* DATA = 1 */
  GPIOX_ODOUT(PS2_MOUSEDATA);

  EXTI_INIT(PS2_MOUSECLK);

  mouse.clockport = GPIOX(PS2_MOUSECLK);
  mouse.dataport  = GPIOX(PS2_MOUSEDATA);
  mouse.clockpinmask = 1 << (GPIOX_PIN(PS2_MOUSECLK));
  mouse.datapinmask = 1 << (GPIOX_PIN(PS2_MOUSEDATA));

  NVIC->ISER[(((uint32_t)(int32_t)PS2_MOUSE_EXT_IRQ) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)PS2_MOUSE_EXT_IRQ) & 0x1FUL));
  NVIC->IP[((uint32_t)(int32_t)PS2_MOUSE_EXT_IRQ)] = (uint8_t)((PS2_IRQPRIORITY << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
}

#endif  // if ((defined PS2_MOUSECLK) && defined PS2_MOUSEDATA))

// ============================================================================
/* common GPIO EXT interrupt (clock falling edge) */
static inline void ps2_ext_int(t_Ps2 * ps2s)
{
  #if PS2_PIN_DEBUG == 1
  GPIOX_SET(PS2_PIN_DEBUG_1);
  #endif

  #if PS2_CLOCKFILTER == 1
  if(GPIOX_IDR_PS2PIN(ps2s->clockport, ps2s->clockpinmask))
  {
    #if PS2_PIN_DEBUG == 1
    GPIOX_CLR(PS2_PIN_DEBUG_1);
    #endif
    return;
  }
  #endif

  /* databit = ps2 data pin */
  if(GPIOX_IDR_PS2PIN(ps2s->dataport, ps2s->datapinmask))
    ps2s->databit = 1;
  else
    ps2s->databit = 0;

  TIM_RESTART;

  ps2s->bitcount--;

  if(ps2s->status == PASSIVE)
  {                                     /* PASSIVE */
    PS2_TIM_ON;
    if(ps2s->databit == 0)
    {                                   /* PASSIVE startbit */
      ps2s->status = REC;
      ps2s->bitcount = 11;
    }
    else
      ps2s->error = 1;
    ps2s->parity = 0;
  }

  else if(ps2s->status == REC)
  {                                     /* REC */
    if(ps2s->bitcount == 2)
    {                                   /* REC parity */
      if(ps2s->databit == ps2s->parity)
        ps2s->error = 1;
    }
    else if(ps2s->bitcount == 1)
    {                                   /* REC stopbit */
      if(ps2s->databit == 1)
      {                                 /* if no error and stopbit == 1 -> scancode to rec buffer */
        ps2s->cb_rx(ps2s->data, ps2s->error);
      }
      ps2s->error = 0;
      ps2s->parity = 0;
      ps2s->data = 0;
      ps2s->status = POST;
    }
    else
    {                                   /* REC databits */
      ps2s->data >>= 1;
      if(ps2s->databit)
      {
        ps2s->data |= 0x80;
        ps2s->parity = 1 - ps2s->parity;
      }
    }
  }

  else if(ps2s->status == SEND)
  {                                     /* SEND */
    if(ps2s->bitcount == 2)
    {                                   /* SEND parity */
      if(!ps2s->parity)
        GPIOX_SET_PS2PIN(ps2s->dataport, ps2s->datapinmask); /* DATA = 1 */
      else
        GPIOX_CLR_PS2PIN(ps2s->dataport, ps2s->datapinmask); /* DATA = 0 */
    }
    else if(ps2s->bitcount == 1)
    {                                   /* SEND stopbit */
      GPIOX_SET_PS2PIN(ps2s->dataport, ps2s->datapinmask); /* DATA = 1 */
    }
    else if(ps2s->bitcount == 0)
    {                                   /* SEND ACK */
      ps2s->data = 0;
      ps2s->bitcount = 11;
      ps2s->parity = 0;
      ps2s->error = 0;
      ps2s->status = POST;
    }
    else
    {                                   /* SEND databits */
      if(ps2s->data & 0x01)
      {
        GPIOX_SET_PS2PIN(ps2s->dataport, ps2s->datapinmask); /* DATA = 1 */
        ps2s->parity = 1 - ps2s->parity;
      }
      else
        GPIOX_CLR_PS2PIN(ps2s->dataport, ps2s->datapinmask); /* DATA = 0 */
      ps2s->data >>= 1;
    }
  }

  else if(ps2s->status == POST)
  {
    ps2s->bitcount = 11;
    ps2s->status = REC;
  }
  #if PS2_PIN_DEBUG == 1
  GPIOX_CLR(PS2_PIN_DEBUG_1);
  #endif
}

// ----------------------------------------------------------------------------
static inline void ps2_timer_int(t_Ps2 * ps2s)
{
  uint8_t data8;
  #if PS2_PIN_DEBUG == 1
  GPIOX_SET(PS2_PIN_DEBUG_2);
  #endif
  if(ps2s->status == SENDSTART)
  {
    if(ps2s->cb_tx(&data8))
    {
      TIM_RESTART;                      /* timer counter reset + timer start */
      GPIOX_CLR_PS2PIN(ps2s->dataport, ps2s->datapinmask);   /* ps2 data pin = 0 */
      GPIOX_SET_PS2PIN(ps2s->clockport, ps2s->clockpinmask); /* ps2 clock pin = 1 */
      ps2s->bitcount = 11;
      ps2s->data = data8;
      ps2s->parity = 0;
      ps2s->status = SEND;
    }
    else
    {
      GPIOX_SET_PS2PIN(ps2s->dataport, ps2s->datapinmask);   /* ps2 data pin = 1 */
      GPIOX_SET_PS2PIN(ps2s->clockport, ps2s->clockpinmask); /* ps2 clock pin = 1 */
      ps2s->status = PASSIVE;
    }
  }
  else
  {
    #if 0
    if(ps2s->status == SEND)
    {
    }
    else if(ps2s->status == POST)
    {
    }
    else if(ps2s->status == REC)
    {
    }
    #endif

    GPIOX_SET_PS2PIN(ps2s->dataport, ps2s->datapinmask);  /* ps2 data pin = 1 */
    if(ps2s->cb_tx(NULL) && (GPIOX_IDR_PS2PIN(ps2s->dataport, ps2s->datapinmask)))
    {                                   /* tx buffer not empty and data pin is high */
      GPIOX_CLR_PS2PIN(ps2s->clockport, ps2s->clockpinmask); /* ps2 clock pin = 0 */
      PS2_TIM->CNT = 0;                 /* timer counter reset */
      PS2_TIM->CR1 |= TIM_CR1_CEN;
      PS2_TIM_ON;                       /* timer active */
      ps2s->status = SENDSTART;
    }
    else
    {
      GPIOX_SET_PS2PIN(ps2s->clockport, ps2s->clockpinmask); /* ps2 clock pin = 1 */
      ps2s->status = PASSIVE;
      PS2_TIM_OFF;
    }
  }
  #if PS2_PIN_DEBUG == 1
  GPIOX_CLR(PS2_PIN_DEBUG_2);
  #endif
}

// ----------------------------------------------------------------------------
/* common kbd and mouse clock EXT input (falling edge) interrupt */
#if (PS2_KBD_EXT_N >= 1) && (PS2_MOUSE_EXT_N >= 1) && (PS2_KBD_EXT_N == PS2_MOUSE_EXT_N)
void PS2_KBD_EXT_IRQHandler(void)
{
  if(EXTI_GET(PS2_KBDCLK))
  {
    EXTI_CLR(PS2_KBDCLK);
    ps2_ext_int(&kbd);
  }
  if(EXTI_GET(PS2_MOUSECLK))
  {
    EXTI_CLR(PS2_MOUSECLK);
    ps2_ext_int(&mouse);
  }
}
#endif

// ----------------------------------------------------------------------------
/* keyboard clock EXT input (falling edge) interrupt */
#if (PS2_KBD_EXT_N >= 1) && (PS2_KBD_EXT_N != PS2_MOUSE_EXT_N)
void PS2_KBD_EXT_IRQHandler(void)
{
  if(EXTI_GET(PS2_KBDCLK))
  {
    EXTI_CLR(PS2_KBDCLK);
    ps2_ext_int(&kbd);
  }
}
#endif

// ----------------------------------------------------------------------------
/* mouse clock EXT input (falling edge) interrupt */
#if (PS2_MOUSE_EXT_N >= 1) && (PS2_KBD_EXT_N != PS2_MOUSE_EXT_N)
void PS2_MOUSE_EXT_IRQHandler(void)
{
  if(EXTI_GET(PS2_MOUSECLK))
  {
    EXTI_CLR(PS2_MOUSECLK);
    ps2_ext_int(&mouse);
  }
}
#endif

// ----------------------------------------------------------------------------
/* timer interrupt */
void PS2_TIM_HANDLER(void)
{
  if(TIM_IRQ_GET)
  {
    #if (PS2_KBD_EXT_N >= 1) && (PS2_MOUSE_EXT_N >= 1)
    if(kbd.timermode)
    {
      ps2_timer_int(&kbd);
    }
    if(mouse.timermode)
    {
      ps2_timer_int(&mouse);
    }
    #elif PS2_KBD_EXT_N >= 1
    ps2_timer_int(&kbd);
    #elif PS2_MOUSE_EXT_N >= 1
    ps2_timer_int(&mouse);
    #endif
  }
  TIM_IRQ_CLR;
}

// ----------------------------------------------------------------------------
/* common ps2 low level init */
void ps2_init(void)
{
  #if (PS2_KBD_EXT_N >= 1) && (PS2_MOUSE_EXT_N >= 1)
  RCC_INIT;
  ps2_kbdinit();
  ps2_mouseinit();
  #elif PS2_KBD_EXT_N >= 1
  RCC_INIT;
  ps2_kbdinit();
  #elif PS2_MOUSE_EXT_N >= 1
  RCC_INIT;
  ps2_mouseinit();
  #endif

  TIM_INIT;

  NVIC->ISER[(((uint32_t)(int32_t)PS2_TIM_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)PS2_TIM_IRQn) & 0x1FUL));
  NVIC->IP[((uint32_t)(int32_t)PS2_TIM_IRQn)] = (uint8_t)((PS2_IRQPRIORITY << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);

  #if PS2_PIN_DEBUG > 0
  RCC->AHB4ENR |= GPIOX_CLOCK(PS2_PIN_DEBUG_1) | GPIOX_CLOCK(PS2_PIN_DEBUG_2);
  GPIOX_MODER(MODE_OUT, PS2_PIN_DEBUG_1);
  GPIOX_MODER(MODE_OUT, PS2_PIN_DEBUG_2);
  GPIOX_CLR(PS2_PIN_DEBUG_1);
  GPIOX_CLR(PS2_PIN_DEBUG_2);
  #endif
}

// ============================================================================
/* high level keyboard */
#if  PS2_KBD_EXT_N >= 1

/* get keyboard scan code
   - input
     *kbd_scan: scan code pointer (if NULL -> only return the key event information, and the RX fifo buffer not modified)
   - output
     return: 0 = not is key event, 1 = is key event
     *kbd_scan: keyboard scan code (if no key event occurred -> *kbd_scan not modified) */
uint8_t ps2_kbd_getscan(uint8_t * kbd_scan)
{
  ps2_initcheck();
  return ps2_kbd_dataread(kbd_scan);
}

// ----------------------------------------------------------------------------
/* Get keyboard asc code
   - input
     *kbd_key: asc code pointer (if NULL -> only return the key pressed information, and the RX fifo buffer not modified)
   - output
     return: 0 = no key pressed, 1 = key pressed
     *kbd_key: keyboard asc code (if no key event occurred -> *kbd_key not modified) */
uint8_t ps2_kbd_getkey(uint8_t * kbd_key)
{
  static uint8_t state = 0, ps2_kbd_c = 0, ps2_kbd_cs = 0;
  uint8_t ps2_kbd_s;

  #if PS2_PIN_DEBUG == 2
  GPIOX_SET(PS2_PIN_DEBUG_1);
  #endif

  ps2_initcheck();

  if(ps2_kbd_cs)
  { /* stored query (previous query: only key pressed information) */
    if(kbd_key)
    {
      *kbd_key = ps2_kbd_c;
      ps2_kbd_cs = 0;
    }
    #if PS2_PIN_DEBUG == 2
    GPIOX_CLR(PS2_PIN_DEBUG_1);
    #endif
    return 1;
  }

  while(1)
  {
    if(ps2_kbd_dataread(&ps2_kbd_s) == 0)
    {
      #if PS2_PIN_DEBUG == 2
      GPIOX_CLR(PS2_PIN_DEBUG_1);
      #endif
      return 0;                         /* nincs a billentyü pufferben semmi */
    }
    if(ps2_kbd_s == 0xE0)
    { /* két bájtos karakter */
      state |= ST_KBDMODIFIER;
      if(ps2_kbd_dataread(&ps2_kbd_s) == 0)
      {
        #if PS2_PIN_DEBUG == 2
        GPIOX_CLR(PS2_PIN_DEBUG_1);
        #endif
        return 0;
      }
    }
    if(ps2_kbd_s == 0xF0)
    { /* gomb felengedés */
      state |= ST_KBDBREAK;
      if(ps2_kbd_dataread(&ps2_kbd_s) == 0)
      {
        #if PS2_PIN_DEBUG == 2
        GPIOX_CLR(PS2_PIN_DEBUG_1);
        #endif
        return 0;
      }
    }

    if(state & ST_KBDBREAK)
    { /* gomb felengedés státusz */
      if(ps2_kbd_s == 0x12)
        state &= ~ST_KBDSHIFT_L;
      else if(ps2_kbd_s == 0x59)
        state &= ~ST_KBDSHIFT_R;
      else if(ps2_kbd_s == 0x14)
        state &= ~ST_KBDCTRL;
      else if(ps2_kbd_s == 0x11 && (state & ST_KBDMODIFIER))
        state &= ~ST_KBDALTGR;
      state &= ~(ST_KBDBREAK | ST_KBDMODIFIER); /* BREAK/MODIFIER státusz ki */
    }
    else
    {                                   /* gomb lenyomás vagy ismétlés */
      if(state & ST_KBDMODIFIER)
      {                                 /* két bájtos (MODIFIER) gombok (INS, DEL, 4 nyil stb.) */
        if(ps2_kbd_s == 0x11)
        {
          state |= ST_KBDALTGR;         /* E0 11 */
          continue;
        }
        else if(ps2_kbd_s == 0x14)
        {
          state |= ST_KBDCTRL;          /* E0 14 */
          continue;
        }
        switch(ps2_kbd_s)
        {
          case 0x70: ps2_kbd_c = PS2_INSERT;     break;
          case 0x6C: ps2_kbd_c = PS2_HOME;       break;
          case 0x7D: ps2_kbd_c = PS2_PAGEUP;     break;
          case 0x71: ps2_kbd_c = PS2_DELETE;     break;
          case 0x69: ps2_kbd_c = PS2_END;        break;
          case 0x7A: ps2_kbd_c = PS2_PAGEDOWN;   break;
          case 0x75: ps2_kbd_c = PS2_UPARROW;    break;
          case 0x6B: ps2_kbd_c = PS2_LEFTARROW;  break;
          case 0x72: ps2_kbd_c = PS2_DOWNARROW;  break;
          case 0x74: ps2_kbd_c = PS2_RIGHTARROW; break;
          case 0x4A: ps2_kbd_c = '/';            break;
          case 0x5A: ps2_kbd_c = PS2_ENTER;      break;
          default: break;
        }
        if(kbd_key)
        {
          *kbd_key = ps2_kbd_c;
        }
        else
        {
          ps2_kbd_cs = 1;
        }
        #if PS2_PIN_DEBUG == 2
        GPIOX_CLR(PS2_PIN_DEBUG_1);
        #endif
        return 1;
      }
      else
      {                            /* nincs MODIFIER (egy bájtos kód) */
        if(ps2_kbd_s == 0x12)
        {
          state |= ST_KBDSHIFT_L;
          continue;
        }
        else if(ps2_kbd_s == 0x59)
        {
          state |= ST_KBDSHIFT_R;
          continue;
        }
        else if(ps2_kbd_s == 0x14)
        {
          state |= ST_KBDCTRL;
          continue;
        }
        if(ps2_kbd_s <= PS2_MAINKEYMAP_SIZE)
        {
          if((state & ST_KBDALTGR) && keymap.uses_altgr) /* altgr */
            ps2_kbd_c = keymap.altgr[ps2_kbd_s];
          else if(state & (ST_KBDSHIFT_L | ST_KBDSHIFT_R))
          { /* shift */
            if(ps2_kbdlockstatus & ST_KBDCAPSLOCK)
              ps2_kbd_c = keymap.shiftcaps[ps2_kbd_s];
            else
              ps2_kbd_c = keymap.shift[ps2_kbd_s];
          }
          else
          {
            if(ps2_kbdlockstatus & ST_KBDCAPSLOCK)
              ps2_kbd_c = keymap.noshiftcaps[ps2_kbd_s];
            else
              ps2_kbd_c = keymap.noshift[ps2_kbd_s];
          }
        }
        else if(ps2_kbd_s <= (PS2_MAINKEYMAP_SIZE + PS2_NUMKEYMAP_SIZE))
        { /* numerikus */
          if(ps2_kbdlockstatus & ST_KBDNUMLOCK)
            ps2_kbd_c = keymap.numon[ps2_kbd_s - PS2_MAINKEYMAP_SIZE];
          else
            ps2_kbd_c = keymap.numoff[ps2_kbd_s - PS2_MAINKEYMAP_SIZE];
        }
        if(kbd_key)
        {
          *kbd_key = ps2_kbd_c;
        }
        else
        {
          ps2_kbd_cs = 1;
        }
        #if PS2_PIN_DEBUG == 2
        GPIOX_CLR(PS2_PIN_DEBUG_1);
        #endif
        return 1;
      } /* else (state & MODIFIER) */
    } /* else (state & BREAK) */
  } /*  while(1) */
}

// ----------------------------------------------------------------------------
uint8_t ps2_kbd_ledstatus(void)
{
  return ps2_kbdlockstatus;
}

// ----------------------------------------------------------------------------
uint8_t ps2_kbd_ctrlstatus(void)
{
  return ps2_kbdctrlstatus;
}

// ----------------------------------------------------------------------------
uint8_t ps2_kbd_setlocks(uint8_t kbd_locks)
{
  ps2_initcheck();
  ps2_kbdlockstatus = kbd_locks & 0x07;
  while(!ps2_kbd_datawrite(0xED));
  while(!ps2_kbd_datawrite(ps2_kbdlockstatus));
  return 1;
}

#else

uint8_t ps2_kbd_getscan(uint8_t * kbd_scan)  {return 0;}
uint8_t ps2_kbd_sendcmd(uint8_t kbd_command) {return 0;}
uint8_t ps2_kbd_getkey(uint8_t * kbd_key)    {return 0;}
uint8_t ps2_kbd_ledstatus(void)              {return 0;}

#endif

/* end of high level keyboard */
// ============================================================================
/* high level mouse */

#if PS2_MOUSE_EXT_N >= 1

uint32_t          time_data_packet;     /* packet start time */
#if ((MOUSE_METHOD == 2) || (MOUSE_METHOD == 3))
uint32_t          time_now;             /* now time */
#endif

#define  MOUSE_UNINITIALIZATION   0     /* no connection the mouse or before reset */
#define  MOUSE_RESET              1     /* reset  */
#define  MOUSE_GETID              2     /* mouse ID */
#define  MOUSE_SETRATE_200        3     /* set rate */
#define  MOUSE_RATE_200           4     /* set rate */
#define  MOUSE_SETRATE_100        5     /* set rate */
#define  MOUSE_RATE_100           6     /* set rate */
#define  MOUSE_SETRATE_80         7     /* set rate */
#define  MOUSE_RATE_80            8     /* set rate */
#define  MOUSE_GETMOVE            9     /* move data read */
#define  MOUSE_ONDATAREPORT      10     /* mouse data report on */

// ----------------------------------------------------------------------------
/* read any bytes from mouse, timeout handled
   - param1: pointer to data array
   - param2: data array size,
   - param3: timeout value (msec)
   - return: 0 = timeout, 1 = no timeout but not arrived the pack, 2 = arrived the pack */
uint8_t ps2_mouse_readdatapacket(uint8_t * data8, uint32_t dsize, uint32_t timeout)
{
  #if MOUSE_METHOD == 1

  while(FIFO_LEN(mouserbuf) < dsize)
  {
    if(PS2_GETTIME() - time_data_packet > timeout)
      return 0;
  }

  while(dsize--)
  {
    ps2_mouse_dataread(data8);
    data8++;
  }
  return 2;

  #elif ((MOUSE_METHOD == 2) || (MOUSE_METHOD == 3))
  uint8_t tmp8;
  if(FIFO_LEN(mouserbuf) >= dsize)
  {
    ps2_printf("mrx: %d, %d\r\n", (unsigned int)time_now, (unsigned int)dsize);
    while(dsize--)
    {
      ps2_mouse_dataread(data8);
      data8++;
    }
    return 2;
  }
  else
  {
    if(time_now - time_data_packet > timeout)
    { /* timeout */
      ps2_printf("mrx_tout: %d, %d\r\n", (unsigned int)time_now, (unsigned int)dsize);
      while(ps2_mouse_dataread(&tmp8)); /* receive buffer emptying */
      return 0;
    }
    else
      return 1;
  }
  #endif
}

// ----------------------------------------------------------------------------
/* get mouse move */
uint8_t ps2_mouse_getmove(ps2_MouseData * mouse_data)
{
  static uint8_t    mouse_status = MOUSE_UNINITIALIZATION;
  int16_t           tmp16;
  uint8_t           tmp8;

  #if PS2_PIN_DEBUG == 2
  GPIOX_SET(PS2_PIN_DEBUG_2);
  #endif

  ps2_initcheck();

  /* if not initialize a mouse */

  #if MOUSE_METHOD == 1
  uint8_t           data_packet[5];

  time_data_packet = PS2_GETTIME();
  if(mouse_status == MOUSE_GETMOVE)
  {
    ps2_mouse_datawrite(0xEB);            /* send the read data command */
    tmp8 = ps2_mouse_readdatapacket(data_packet, read_packet_size, PS2_MOUSE_READTIME);
    if(tmp8 == 2)
    { /* complett pack size */
      if(data_packet[0] != 0xFA)
      {
        #if PS2_PIN_DEBUG == 2
        GPIOX_CLR(PS2_PIN_DEBUG_2);
        #endif
        return 0;
      }

      /* x move */
      if(data_packet[1] & 0x40)
        tmp16 = 256;
      else
        tmp16 = 0;
      tmp16 += data_packet[2];
      if(data_packet[1] & 0x10)
        tmp16 -= 256;
      mouse_data->xmove = tmp16;

      /* y move */
      if(data_packet[1] & 0x80)
        tmp16 = 256;
      else
        tmp16 = 0;
      tmp16 += data_packet[3];
      if(data_packet[1] & 0x20)
        tmp16 -= 256;
      mouse_data->ymove = tmp16;

      /* z move and buttons */
      if(read_packet_size == 4)
      { /* 3 byte mouse data size */
        mouse_data->zmove = 0;
        mouse_data->btns = data_packet[1] & 0x07;
      }
      else
      { /* 4 byte mouse data size */
        mouse_data->zmove = (int8_t)data_packet[4];
        mouse_data->btns = data_packet[1] & 0x07;
      }

      #if PS2_PIN_DEBUG == 2
      GPIOX_CLR(PS2_PIN_DEBUG_2);
      #endif
      return 1;
    }
    else if(tmp8 == 0)
    {
      mouse_status = MOUSE_UNINITIALIZATION;
    }
  }

  else if(mouse_status == MOUSE_GETID)
  {
    ps2_mouse_datawrite(0xF2);      /* mouse ID */
    tmp8 = ps2_mouse_readdatapacket(data_packet, 2, PS2_MOUSE_IDTIME);
    if((tmp8 == 2) && (data_packet[0] == 0xFA) && ((data_packet[1] == 0x00) || (data_packet[1] == 0x03)))
    {
      mouse_status = MOUSE_GETMOVE;
      if(data_packet[1] == 0x00)
        read_packet_size = 4;
      else if(data_packet[1] == 0x03)
        read_packet_size = 5;
    }
  }

  else if((mouse_status >= MOUSE_SETRATE_200) && (mouse_status <= MOUSE_RATE_80))
  {
    if(mouse_status == MOUSE_RATE_200)
      ps2_mouse_datawrite(0xC8);        /* 200 */
    else if(mouse_status == MOUSE_RATE_100)
      ps2_mouse_datawrite(0x64);        /* 100 */
    else if(mouse_status == MOUSE_RATE_80)
      ps2_mouse_datawrite(0x50);        /* 80 */
    else
      ps2_mouse_datawrite(0xF3);        /* set sample rate */
    tmp8 = ps2_mouse_readdatapacket(data_packet, 1, PS2_MOUSE_RATETIME);
    if((tmp8 == 2) && (data_packet[0] == 0xFA))
    {
      if(mouse_status == MOUSE_RATE_80)
        mouse_status = MOUSE_GETID;
      else
        mouse_status++;
    }
    else
      mouse_status = MOUSE_UNINITIALIZATION;
  }

  else if(mouse_status == MOUSE_UNINITIALIZATION)
  {
    while(ps2_mouse_dataread(&tmp8));   /* receive buffer empty */
    ps2_mouse_datawrite(0xFF);          /* mouse reset */
    tmp8 = ps2_mouse_readdatapacket(data_packet, 3, PS2_MOUSE_RESETTIME);
    if((tmp8 == 2) && (data_packet[0] == 0xFA) && (data_packet[1] == 0xAA) && (data_packet[2] == 0x00))
      mouse_status = MOUSE_SETRATE_200;
  }

  #if PS2_PIN_DEBUG == 2
  GPIOX_CLR(PS2_PIN_DEBUG_2);
  #endif
  return 0;

  #elif MOUSE_METHOD == 2

  uint8_t           data_packet[5];

  time_now = PS2_GETTIME();
  if(mouse_status == MOUSE_GETMOVE)
  {
    tmp8 = ps2_mouse_readdatapacket(data_packet, read_packet_size, PS2_MOUSE_READTIME);
    if(tmp8 == 2)
    { /* complett pack size */
      if(data_packet[0] != 0xFA)
      {
        #if PS2_PIN_DEBUG == 2
        GPIOX_CLR(PS2_PIN_DEBUG_2);
        #endif
        return 0;
      }

      /* x move */
      if(data_packet[1] & 0x40)
        tmp16 = 256;
      else
        tmp16 = 0;
      tmp16 += data_packet[2];
      if(data_packet[1] & 0x10)
        tmp16 -= 256;
      mouse_data->xmove = tmp16;

      /* y move */
      if(data_packet[1] & 0x80)
        tmp16 = 256;
      else
        tmp16 = 0;
      tmp16 += data_packet[3];
      if(data_packet[1] & 0x20)
        tmp16 -= 256;
      mouse_data->ymove = tmp16;

      /* z move and buttons */
      if(read_packet_size == 4)
      { /* 3 byte mouse data size */
        mouse_data->zmove = 0;
        mouse_data->btns = data_packet[1] & 0x07;
      }
      else
      { /* 4 byte mouse data size */
        mouse_data->zmove = (int8_t)data_packet[4];
        mouse_data->btns = data_packet[1] & 0x07;
      }
      ps2_mouse_datawrite(0xEB);        /* send the read data command */
      time_data_packet = time_now;
      #if PS2_PIN_DEBUG == 2
      GPIOX_CLR(PS2_PIN_DEBUG_2);
      #endif
      return 1;
    }
    else if(tmp8 == 0)
    {
      mouse_status = MOUSE_UNINITIALIZATION;
    }
  }

  else if(mouse_status == MOUSE_GETID)
  {
    tmp8 = ps2_mouse_readdatapacket(data_packet, 2, PS2_MOUSE_IDTIME);
    if((tmp8 == 2) && (data_packet[0] == 0xFA) && ((data_packet[1] == 0x00) || (data_packet[1] == 0x03)))
    { /* FA 00 or FA 03 */
      ps2_mouse_datawrite(0xEB);            /* send the read data command */
      time_data_packet = time_now;
      mouse_status = MOUSE_GETMOVE;
      if(data_packet[1] == 0x00)
        read_packet_size = 4;
      else if(data_packet[1] == 0x03)
        read_packet_size = 5;
    }
    else if(tmp8 == 0)
      mouse_status = MOUSE_UNINITIALIZATION;
  }

  else if((mouse_status >= MOUSE_SETRATE_200) && (mouse_status <= MOUSE_RATE_80))
  {
    tmp8 = ps2_mouse_readdatapacket(data_packet, 1, PS2_MOUSE_RATETIME);
    if((tmp8 == 2) && (data_packet[0] == 0xFA))
    {
      if(mouse_status < MOUSE_RATE_80)
      {
        mouse_status++;
        if(mouse_status == MOUSE_RATE_200)
          ps2_mouse_datawrite(0xC8);    /* 200 */
        else if(mouse_status == MOUSE_RATE_100)
          ps2_mouse_datawrite(0x64);    /* 100 */
        else if(mouse_status == MOUSE_RATE_80)
          ps2_mouse_datawrite(0x50);    /* 80 */
        else
          ps2_mouse_datawrite(0xF3);    /* set sample rate */
      }
      else
      {
        ps2_mouse_datawrite(0xF2);      /* mouse ID */
        mouse_status = MOUSE_GETID;
      }
      time_data_packet = time_now;
    }
    else if(tmp8 == 0)
      mouse_status = MOUSE_UNINITIALIZATION;
  }

  else if(mouse_status == MOUSE_RESET)
  {
    tmp8 = ps2_mouse_readdatapacket(data_packet, 3, PS2_MOUSE_RESETTIME);
    if(tmp8 == 2)
    {
      if((data_packet[0] == 0xFA) && (data_packet[1] == 0xAA) && (data_packet[2] == 0x00))
      {
        ps2_mouse_datawrite(0xF3);      /* set sample rate */
        time_data_packet = time_now;
        mouse_status = MOUSE_SETRATE_200;
      }
    }
    else if(tmp8 == 0)
    {
      mouse_status = MOUSE_UNINITIALIZATION;
    }
  }

  else if(mouse_status == MOUSE_UNINITIALIZATION)
  {
    while(ps2_mouse_dataread(&tmp8));   /* receive buffer empty */
    ps2_mouse_datawrite(0xFF);          /* mouse reset */
    time_data_packet = time_now;
    mouse_status = MOUSE_RESET;
    ps2_printf("mouse reset\r\n");
  }
  #if PS2_PIN_DEBUG == 2
  GPIOX_CLR(PS2_PIN_DEBUG_2);
  #endif
  return 0;

  #elif MOUSE_METHOD == 3

  static uint8_t    pre_mouse_buttons, id;
  uint32_t          fifo_len;
  uint8_t           data_packet[4];

  #if PS2_PRINTF_DEBUG == 1
  static uint32_t   n = 0;
  n++;
  #endif

  time_now = PS2_GETTIME();

  if(mouse_rx_error)
  {
    mouse_status = MOUSE_UNINITIALIZATION;
    mouse_rx_error = 0;
    ps2_printf("mrx_parityerr: %d, %d\r\n", (unsigned int)time_now, (unsigned int)n);
  }

  if(mouse_status == MOUSE_GETMOVE)
  {
    fifo_len = FIFO_LEN(mouserbuf);
    if(fifo_len >= read_packet_size)
    { /* completed pack size */
      mouse_data->xmove = 0;
      mouse_data->ymove = 0;
      mouse_data->zmove = 0;
      mouse_data->btns  = 0;
      while(1)
      {
        ps2_mouse_dataread(&data_packet[0]);
        ps2_mouse_dataread(&data_packet[1]);
        ps2_mouse_dataread(&data_packet[2]);

        /* x move */
        if(data_packet[0] & 0x40)
          tmp16 = 256;
        else
          tmp16 = 0;
        tmp16 += data_packet[1];
        if(data_packet[0] & 0x10)
          tmp16 -= 256;
        mouse_data->xmove += tmp16;

        /* y move */
        if(data_packet[0] & 0x80)
          tmp16 = 256;
        else
          tmp16 = 0;
        tmp16 += data_packet[2];
        if(data_packet[0] & 0x20)
          tmp16 -= 256;
        mouse_data->ymove += tmp16;

        /* z move and buttons */
        if(read_packet_size == 3)
        { /* 3 byte mouse data size */
          mouse_data->zmove = 0;
          mouse_data->btns = data_packet[0] & 0x07;
        }
        else
        { /* 4 byte mouse data size */
          ps2_mouse_dataread(&data_packet[3]);
          mouse_data->zmove += (int8_t)data_packet[3];
          mouse_data->btns = data_packet[0] & 0x07;
        }

        if((FIFO_LEN(mouserbuf) < read_packet_size) || mouse_data->btns != pre_mouse_buttons)
        {
          pre_mouse_buttons = mouse_data->btns;
          time_data_packet = time_now;
          #if PS2_PIN_DEBUG == 2
          GPIOX_CLR(PS2_PIN_DEBUG_2);
          #endif
          return 1;
        }
      }
    }
    else if(fifo_len <= 1)
    {
      time_data_packet = time_now;
    }
    else
    {
      if(time_now - time_data_packet > PS2_MOUSE_READTIME)
        while(ps2_mouse_dataread(&tmp8)); /* if big time space -> receive buffer empty (sync repair) */
    }
  }

  else if(mouse_status == MOUSE_ONDATAREPORT)
  {
    tmp8 = ps2_mouse_readdatapacket((uint8_t *)&data_packet, 1, PS2_MOUSE_IDTIME);
    if((tmp8 == 2) && (data_packet[0] == 0xFA))
    {
      time_data_packet = time_now;
      mouse_status = MOUSE_GETMOVE;
      read_packet_size = id;
    }
    else if(tmp8 == 0)
      mouse_status = MOUSE_UNINITIALIZATION;
  }

  else if(mouse_status == MOUSE_GETID)
  {
    tmp8 = ps2_mouse_readdatapacket((uint8_t *)&data_packet, 2, PS2_MOUSE_IDTIME);
    if((tmp8 == 2) && (data_packet[0] == 0xFA) && ((data_packet[1] == 0x00) || (data_packet[1] == 0x03)))
    { /* FA 00 or FA 03 */
      ps2_mouse_datawrite(0xF4);            /* enable data reporting */
      ps2_printf("mtx:0xF4, %d, %d\r\n", (unsigned int)time_now, (unsigned int)n);
      time_data_packet = time_now;
      mouse_status = MOUSE_ONDATAREPORT;
      if(data_packet[1] == 0x00)
        id = 3;
      else if(data_packet[1] == 0x03)
        id = 4;
      read_packet_size = 1;
    }
    else if(tmp8 == 0)
      mouse_status = MOUSE_UNINITIALIZATION;
  }

  else if((mouse_status >= MOUSE_SETRATE_200) && (mouse_status <= MOUSE_RATE_80))
  {
    tmp8 = ps2_mouse_readdatapacket((uint8_t *)&data_packet, 1, PS2_MOUSE_RATETIME);
    if((tmp8 == 2) && (data_packet[0] == 0xFA))
    {
      if(mouse_status < MOUSE_RATE_80)
      {
        mouse_status++;
        if(mouse_status == MOUSE_RATE_200)
        {
          ps2_mouse_datawrite(0xC8);    /* 200 */
          ps2_printf("mtx:0xC8, %d, %d\r\n", (unsigned int)time_now, (unsigned int)n);
        }
        else if(mouse_status == MOUSE_RATE_100)
        {
          ps2_mouse_datawrite(0x64);    /* 100 */
          ps2_printf("mtx:0x64, %d, %d\r\n", (unsigned int)time_now, (unsigned int)n);
        }
        else if(mouse_status == MOUSE_RATE_80)
        {
          ps2_mouse_datawrite(0x50);    /* 80 */
          ps2_printf("mtx:0x50, %d, %d\r\n", (unsigned int)time_now, (unsigned int)n);

        }
        else
        {
          ps2_mouse_datawrite(0xF3);    /* set sample rate */
          ps2_printf("mtx:0xF3, %d, %d\r\n", (unsigned int)time_now, (unsigned int)n);
        }
      }
      else
      {
        ps2_mouse_datawrite(0xF2);      /* mouse ID */
        ps2_printf("mtx:0xF2, %d, %d\r\n", (unsigned int)time_now, (unsigned int)n);
        mouse_status = MOUSE_GETID;
        read_packet_size = 2;
      }
      time_data_packet = time_now;
    }
    else if(tmp8 == 0)
      mouse_status = MOUSE_UNINITIALIZATION;
  }

  else if(mouse_status == MOUSE_RESET)
  {
    tmp8 = ps2_mouse_readdatapacket((uint8_t *)&data_packet, 3, PS2_MOUSE_RESETTIME);
    if(tmp8 == 2)
    {
      if((data_packet[0] == 0xFA) && (data_packet[1] == 0xAA) && (data_packet[2] == 0x00))
      {
        ps2_mouse_datawrite(0xF3);      /* set sample rate */
        ps2_printf("mtx:0xF3, %d, %d\r\n", (unsigned int)time_now, (unsigned int)n);
        time_data_packet = time_now;
        mouse_status = MOUSE_SETRATE_200;
        read_packet_size = 1;
      }
    }
    else if(tmp8 == 0)
    {
      mouse_status = MOUSE_UNINITIALIZATION;
    }
  }

  else if(mouse_status == MOUSE_UNINITIALIZATION)
  {
    while(ps2_mouse_dataread(&tmp8));   /* receive buffer empty */
    ps2_mouse_datawrite(0xFF);          /* mouse reset */
    time_data_packet = time_now;
    mouse_status = MOUSE_RESET;
    ps2_printf("mtx:0xFF, %d, %d\r\n", (unsigned int)time_now, (unsigned int)n);
    read_packet_size = 1;
  }
  #if PS2_PIN_DEBUG == 2
  GPIOX_CLR(PS2_PIN_DEBUG_2);
  #endif
  return 0;

  #endif
}

#else

uint8_t ps2_mouse_getmove(ps2_MouseData * mouse_data) {return 0;}

#endif
