
#ifndef SERVO_H
#define SERVO_H

// Include STM32F4x libraries we need here
#include "../lib/inc/stm32f4xx.h"
#include "../lib/inc/system_stm32f4xx.h"
#include "../lib/inc/peripherals/stm32f4xx_gpio.h"
#include "../lib/inc/peripherals/stm32f4xx_rcc.h"
#include "../lib/inc/peripherals/stm32f4xx_tim.h"
#include "../lib/inc/peripherals/misc.h" // High level functions for NVIC and SysTick (add-on to CMSIS functions)

// Servo ports
#define SERVO1 GPIO_Pin_0
#define SERVO2 GPIO_Pin_1
#define SERVO3 GPIO_Pin_2
#define SERVO4 GPIO_Pin_3
#define SERVO_REGISTER GPIOD
#define SERVO_PORTS (SERVO1 | SERVO2 | SERVO3 | SERVO4)

// Servo resolution:
// The servo needs a signal from 1-2 ms and a beginning high of 1ms
// The timer is running on 1MHz; a persiod of 20000 is a tick every 20µs
#define SERVO_TIM_PRESCALE 42
#define SERVO_TIM_PERIOD 20
#define SERVO_TIM_COUNTER 1000
#define SERVO_TIM_MICROSECOND 100

extern __IO float servo_angle[4];
extern __IO u16 servo_period[4];
extern __IO u16 servo_count;

void servo_gpio_init();
void servo_init();
void servo_set_pos(u16 num, u16 position);
void TIM3_IRQHandler(void);

#endif // SERVO_H