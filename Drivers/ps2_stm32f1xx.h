/* stm32f1xx processor family dependent things */

#ifndef __PS2_STM32F1XX_H__
#define __PS2_STM32F1XX_H__

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
#define BITBAND_ACCESS(a, b)  *(volatile uint32_t*)(((uint32_t)&a & 0xF0000000) + 0x2000000 + (((uint32_t)&a & 0x000FFFFF) << 5) + (b << 2))

//-----------------------------------------------------------------------------
/* GPIO config (PP: push-pull, OD: open drain, FF: input floating) */
#define MODE_ANALOG_INPUT     0x0
#define MODE_PP_OUT_10MHZ     0x1
#define MODE_PP_OUT_2MHZ      0x2
#define MODE_PP_OUT_50MHZ     0x3
#define MODE_FF_DIGITAL_INPUT 0x4
#define MODE_OD_OUT_10MHZ     0x5
#define MODE_OD_OUT_2MHZ      0x6
#define MODE_OD_OUT_50MHZ     0x7
#define MODE_PU_DIGITAL_INPUT 0x8
#define MODE_PP_ALTER_10MHZ   0x9
#define MODE_PP_ALTER_2MHZ    0xA
#define MODE_PP_ALTER_50MHZ   0xB
#define MODE_RESERVED         0xC
#define MODE_OD_ALTER_10MHZ   0xD
#define MODE_OD_ALTER_2MHZ    0xE
#define MODE_OD_ALTER_50MHZ   0xF

#define GPIOX_(a, b)          GPIO ## a
#define GPIOX(a)              GPIOX_(a)

#define GPIOX_PIN_(a, b)      b
#define GPIOX_PIN(a)          GPIOX_PIN_(a)

#define GPIOX_MODE_(a,b,c)    ((GPIO_TypeDef*)(((c & 8) >> 1) + GPIO ## b ## _BASE))->CRL = (((GPIO_TypeDef*)(((c & 8) >> 1) + GPIO ## b ## _BASE))->CRL & ~(0xF << ((c & 7) << 2))) | (a << ((c & 7) << 2))
#define GPIOX_MODE(a, b)      GPIOX_MODE_(a, b)

#define GPIOX_ODR_(a, b)      BITBAND_ACCESS(GPIO ## a ->ODR, b)
#define GPIOX_ODR(a)          GPIOX_ODR_(a)

#define GPIOX_IDR_(a, b)      BITBAND_ACCESS(GPIO ## a ->IDR, b)
#define GPIOX_IDR(a)          GPIOX_IDR_(a)

#define GPIOX_SET_(a, b)      GPIO ## a ->BSRR = 1 << b
#define GPIOX_SET(a)          GPIOX_SET_(a)

#define GPIOX_CLR_(a, b)      GPIO ## a ->BSRR = 1 << (b + 16)
#define GPIOX_CLR(a)          GPIOX_CLR_(a)

#define GPIOX_LINE_(a, b)     EXTI_Line ## b
#define GPIOX_LINE(a)         GPIOX_LINE_(a)

#define GPIOX_PORTSRC_(a, b)  GPIO_PortSourceGPIO ## a
#define GPIOX_PORTSRC(a)      GPIOX_PORTSRC_(a)

#define GPIOX_PINSRC_(a, b)   GPIO_PinSource ## b
#define GPIOX_PINSRC(a)       GPIOX_PINSRC_(a)

#define GPIOX_CLOCK_(a, b)    RCC_APB2ENR_IOP ## a ## EN
#define GPIOX_CLOCK(a)        GPIOX_CLOCK_(a)

#define GPIOX_PORTNUM_A       1
#define GPIOX_PORTNUM_B       2
#define GPIOX_PORTNUM_C       3
#define GPIOX_PORTNUM_D       4
#define GPIOX_PORTNUM_E       5
#define GPIOX_PORTNUM_F       6
#define GPIOX_PORTNUM_G       7
#define GPIOX_PORTNUM_H       8
#define GPIOX_PORTNUM_I       9
#define GPIOX_PORTNUM_J       10
#define GPIOX_PORTNUM_K       11
#define GPIOX_PORTNUM_(a, b)  GPIOX_PORTNUM_ ## a
#define GPIOX_PORTNUM(a)      GPIOX_PORTNUM_(a)

#define GPIOX_PORTNAME_(a, b) a
#define GPIOX_PORTNAME(a)     GPIOX_PORTNAME_(a)

//-----------------------------------------------------------------------------
/* Timer config */
#if PS2_TIM == 1
#undef  PS2_TIM
#define PS2_TIM               TIM1
#define PS2_TIM_CLKON         RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
#define PS2_TIM_IRQn          TIM1_UP_IRQn
#define PS2_TIM_HANDLER       TIM1_UP_IRQHandler
#elif PS2_TIM == 2
#undef  PS2_TIM
#define PS2_TIM               TIM2
#define PS2_TIM_CLKON         RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
#define PS2_TIM_IRQn          TIM2_IRQn
#define PS2_TIM_HANDLER       TIM2_IRQHandler
#elif PS2_TIM == 3
#undef  PS2_TIM
#define PS2_TIM               TIM3
#define PS2_TIM_CLKON         RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
#define PS2_TIM_IRQn          TIM3_IRQn
#define PS2_TIM_HANDLER       TIM3_IRQHandler
#elif PS2_TIM == 4
#undef  PS2_TIM
#define PS2_TIM               TIM4
#define PS2_TIM_CLKON         RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
#define PS2_TIM_IRQn          TIM4_IRQn
#define PS2_TIM_HANDLER       TIM4_IRQHandler
#elif PS2_TIM == 5
#undef  PS2_TIM
#define PS2_TIM               TIM5
#define PS2_TIM_CLKON         RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
#define PS2_TIM_IRQn          TIM5_IRQn
#define PS2_TIM_HANDLER       TIM5_IRQHandler
#elif PS2_TIM == 6
#undef  PS2_TIM
#define PS2_TIM               TIM6
#define PS2_TIM_CLKON         RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
#define PS2_TIM_IRQn          TIM6_IRQn
#define PS2_TIM_HANDLER       TIM6_IRQHandler
#elif PS2_TIM == 7
#undef  PS2_TIM
#define PS2_TIM               TIM7
#define PS2_TIM_CLKON         RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
#define PS2_TIM_IRQn          TIM7_IRQn
#define PS2_TIM_HANDLER       TIM7_IRQHandler
#elif PS2_TIM == 8
#undef  PS2_TIM
#define PS2_TIM               TIM8
#define PS2_TIM_CLKON         RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
#define PS2_TIM_IRQn          TIM8_UP_IRQn
#define PS2_TIM_HANDLER       TIM8_UP_IRQHandler
#elif PS2_TIM == 9
#undef  PS2_TIM
#define PS2_TIM               TIM9
#define PS2_TIM_CLKON         RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
#define PS2_TIM_IRQn          TIM9_IRQn
#define PS2_TIM_HANDLER       TIM9_IRQHandler
#elif PS2_TIM == 10
#undef  PS2_TIM
#define PS2_TIM               TIM10
#define PS2_TIM_CLKON         RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
#define PS2_TIM_IRQn          TIM10_IRQn
#define PS2_TIM_HANDLER       TIM10_IRQHandler
#elif PS2_TIM == 11
#undef  PS2_TIM
#define PS2_TIM               TIM11
#define PS2_TIM_CLKON         RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
#define PS2_TIM_IRQn          TIM11_IRQn
#define PS2_TIM_HANDLER       TIM11_IRQHandler
#elif PS2_TIM == 12
#undef  PS2_TIM
#define PS2_TIM               TIM12
#define PS2_TIM_CLKON         RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;
#define PS2_TIM_IRQn          TIM12_IRQn
#define PS2_TIM_HANDLER       TIM12_IRQHandler
#elif PS2_TIM == 13
#undef  PS2_TIM
#define PS2_TIM               TIM13
#define PS2_TIM_CLKON         RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;
#define PS2_TIM_IRQn          TIM13_IRQn
#define PS2_TIM_HANDLER       TIM13_IRQHandler
#elif PS2_TIM == 14
#undef  PS2_TIM
#define PS2_TIM               TIM14
#define PS2_TIM_CLKON         RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
#define PS2_TIM_IRQn          TIM14_IRQn
#define PS2_TIM_HANDLER       TIM14_IRQHandler
#else
#error  PS2 TIM unknown	
#endif

//-----------------------------------------------------------------------------
/* Keyboard EXTI config */
#if (GPIOX_PORTNUM(PS2_KBDCLK) >= GPIOX_PORTNUM_A) && (GPIOX_PORTNUM(PS2_KBDDATA) >= GPIOX_PORTNUM_A)

#if (GPIOX_PIN(PS2_KBDCLK)) == 0
#define PS2_KBD_EXT_N    1
#define PS2_KBD_EXT_IRQ  EXTI0_IRQn
#define PS2_KBD_EXT_IRQHandler  EXTI0_IRQHandler
#elif (GPIOX_PIN(PS2_KBDCLK)) == 1
#define PS2_KBD_EXT_N    2
#define PS2_KBD_EXT_IRQ  EXTI1_IRQn
#define PS2_KBD_EXT_IRQHandler  EXTI1_IRQHandler
#elif (GPIOX_PIN(PS2_KBDCLK)) == 2
#define PS2_KBD_EXT_N    3
#define PS2_KBD_EXT_IRQ  EXTI2_IRQn
#define PS2_KBD_EXT_IRQHandler  EXTI2_IRQHandler
#elif (GPIOX_PIN(PS2_KBDCLK)) == 3
#define PS2_KBD_EXT_N    4
#define PS2_KBD_EXT_IRQ  EXTI3_IRQn
#define PS2_KBD_EXT_IRQHandler  EXTI3_IRQHandler
#elif (GPIOX_PIN(PS2_KBDCLK)) == 4
#define PS2_KBD_EXT_N    5
#define PS2_KBD_EXT_IRQ  EXTI4_IRQn
#define PS2_KBD_EXT_IRQHandler  EXTI4_IRQHandler
#elif (GPIOX_PIN(PS2_KBDCLK)) <= 9
#define PS2_KBD_EXT_N    6
#define PS2_KBD_EXT_IRQ  EXTI9_5_IRQn
#define PS2_KBD_EXT_IRQHandler  EXTI9_5_IRQHandler
#elif (GPIOX_PIN(PS2_KBDCLK)) <= 15
#define PS2_KBD_EXT_N    7
#define PS2_KBD_EXT_IRQ  EXTI15_10_IRQn
#define PS2_KBD_EXT_IRQHandler  EXTI15_10_IRQHandler
#endif

#endif

// ----------------------------------------------------------------------------
/* Mouse EXTI config */
#if (GPIOX_PORTNUM(PS2_MOUSECLK) >= GPIOX_PORTNUM_A) && (GPIOX_PORTNUM(PS2_MOUSEDATA) >= GPIOX_PORTNUM_A)

#if (GPIOX_PIN(PS2_MOUSECLK)) == 0
#define PS2_MOUSE_EXT_N    1
#define PS2_MOUSE_EXT_IRQ  EXTI0_IRQn
#define PS2_MOUSE_EXT_IRQHandler  EXTI0_IRQHandler
#elif (GPIOX_PIN(PS2_MOUSECLK)) == 1
#define PS2_MOUSE_EXT_N    2
#define PS2_MOUSE_EXT_IRQ  EXTI1_IRQn
#define PS2_MOUSE_EXT_IRQHandler  EXTI1_IRQHandler
#elif (GPIOX_PIN(PS2_MOUSECLK)) == 2
#define PS2_MOUSE_EXT_N    3
#define PS2_MOUSE_EXT_IRQ  EXTI2_IRQn
#define PS2_MOUSE_EXT_IRQHandler  EXTI2_IRQHandler
#elif (GPIOX_PIN(PS2_MOUSECLK)) == 3
#define PS2_MOUSE_EXT_N    4
#define PS2_MOUSE_EXT_IRQ  EXTI3_IRQn
#define PS2_MOUSE_EXT_IRQHandler  EXTI3_IRQHandler
#elif (GPIOX_PIN(PS2_MOUSECLK)) == 4
#define PS2_MOUSE_EXT_N    5
#define PS2_MOUSE_EXT_IRQ  EXTI4_IRQn
#define PS2_MOUSE_EXT_IRQHandler  EXTI4_IRQHandler
#elif (GPIOX_PIN(PS2_MOUSECLK)) <= 9
#define PS2_MOUSE_EXT_N    6
#define PS2_MOUSE_EXT_IRQ  EXTI9_5_IRQn
#define PS2_MOUSE_EXT_IRQHandler  EXTI9_5_IRQHandler
#elif (GPIOX_PIN(PS2_MOUSECLK)) <= 15
#define PS2_MOUSE_EXT_N    7
#define PS2_MOUSE_EXT_IRQ  EXTI15_10_IRQn
#define PS2_MOUSE_EXT_IRQHandler  EXTI15_10_IRQHandler
#endif

#endif

// ----------------------------------------------------------------------------
/* RCC processor family dependent things */
#define RCC_PIN_DEBUG_INIT  RCC->AHBENR |= GPIOX_CLOCK(PS2_PIN_DEBUG_1) | GPIOX_CLOCK(PS2_PIN_DEBUG_2)

#if (PS2_KBD_EXT_N >= 1) && (PS2_MOUSE_EXT_N >= 1)
#define RCC_INIT  { \
  RCC->AHBENR |= GPIOX_CLOCK(PS2_KBDCLK) | GPIOX_CLOCK(PS2_KBDDATA) |    \
                 GPIOX_CLOCK(PS2_MOUSECLK) | GPIOX_CLOCK(PS2_MOUSEDATA); \
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                                    }
#elif PS2_KBD_EXT_N >= 1
#define RCC_INIT  { \
  RCC->AHBENR |= GPIOX_CLOCK(PS2_KBDCLK) | GPIOX_CLOCK(PS2_KBDDATA) | RCC_APB2ENR_AFIOEN; \
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; }
#elif PS2_MOUSE_EXT_N >= 1
#define RCC_INIT  { \
  RCC->AHBENR |= GPIOX_CLOCK(PS2_MOUSECLK) | GPIOX_CLOCK(PS2_MOUSEDATA); \
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; }
#endif

// ----------------------------------------------------------------------------
/* GPIO processor family dependent things */
#define GPIOX_PPOUT(a)          GPIOX_MODE_(MODE_PP_OUT_2MHZ, a)
#define GPIOX_ODOUT(a)          GPIOX_MODE_(MODE_OD_OUT_2MHZ, a)
#define GPIOX_SET_PS2PIN(a, b)  a->BSRR = b
#define GPIOX_CLR_PS2PIN(a, b)  a->BSRR = b << 16
#define GPIOX_IDR_PS2PIN(a, b)  a->IDR & b

// ----------------------------------------------------------------------------
/* TIMER processor family dependent things */
#define TIM_RESTART             { PS2_TIM->CNT = 0;  PS2_TIM->CR1 |= TIM_CR1_CEN; }
#define TIM_IRQ_GET             PS2_TIM->SR & TIM_SR_UIF
#define TIM_IRQ_CLR             PS2_TIM->SR = 0
#define TIM_INIT {                              \
  PS2_TIM_CLKON;                                \
  PS2_TIM->PSC = (PS2_TIM_CLK) / 1000000 - 1;   \
  PS2_TIM->ARR = PS2_STARTIMPULSEWIDTH - 1;     \
  PS2_TIM->CR1 |= TIM_CR1_OPM;                  \
  PS2_TIM->DIER |= TIM_DIER_UIE;                }

// ----------------------------------------------------------------------------
/* EXTI processor family dependent things */
#define EXTI_GET(a)             EXTI->PR & (1 << GPIOX_PIN_(a))
#define EXTI_CLR(a)             EXTI->PR = 1 << GPIOX_PIN_(a)
#define EXTI_INIT(a) {                \
  AFIO->EXTICR[GPIOX_PIN_(a) / 4] |= (GPIOX_PORTNUM_(a) - 1) << ((GPIOX_PIN_(a) % 4) * 4); \
  EXTI->FTSR |= 1 << (GPIOX_PIN_(a)); \
  EXTI->IMR |= 1 << (GPIOX_PIN_(a));  }

#ifdef __cplusplus
}
#endif

#endif  /* __PS2_STM32F1XX_H__ */
