// Main includes
#include <stdio.h>
#include <math.h>

// Project includes
#include "servo.h"
#include "receiver.h"

// Include all needed SMF32F4 libraries
#include "../lib/inc/stm32f4xx.h"
#include "../lib/inc/system_stm32f4xx.h"
#include "../lib/inc/peripherals/stm32f4xx_gpio.h"
#include "../lib/inc/peripherals/stm32f4xx_i2c.h"
#include "../lib/inc/peripherals/stm32f4xx_rcc.h"
#include "../lib/inc/peripherals/stm32f4xx_sdio.h"
#include "../lib/inc/peripherals/stm32f4xx_spi.h"
#include "../lib/inc/peripherals/stm32f4xx_syscfg.h"
#include "../lib/inc/peripherals/stm32f4xx_tim.h"
#include "../lib/inc/peripherals/misc.h" // High level functions for NVIC and SysTick (add-on to CMSIS functions)

// LED Configuration
#define LED1 GPIO_Pin_12 
#define LED2 GPIO_Pin_13
#define LED3 GPIO_Pin_14
#define LED4 GPIO_Pin_15
#define LED_REGISTER GPIOD
#define LED_PORTS (LED1 | LED2 | LED3 | LED4)

// Redefine the HSE value; it's equal to 8 MHz on the STM32F4-DISCOVERY Kit
#if defined (HSE_VALUE)
#undef HSE_VALUE
#define HSE_VALUE ((uint32_t)8000000) 
#endif // HSE_VALUE


// ---------- Main methods ---------- //
void init_gpio();
void delay(__IO u32 nCount);
void wait(u32 micro);
