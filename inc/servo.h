/** @file    servo.c
 *  @author  Lukas Zurschmiede <lukas@ranta.ch>
 *  @email   <lukas@ranta.ch>
 *  @version 0.0.1
 *  @date    2014-01-01
 *  @brief   This file contains all needed functions for regulate a servo with a STM32F4
 * 
 *  Copyright (C) 2013-2014 @em Lukas @em Zurschmiede <lukas@ranta.ch>
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
// SystemCoreClock is set to 168 MHz for STM32F4xx Devices Revision A.
#define SERVO_TIM_PRESCALE 168
#define SERVO_TIM_PERIOD 20
#define SERVO_TIM_COUNTER 1000
#define SERVO_TIM_MICROSECOND 100

extern volatile float servo_angle[4];
extern volatile u16 servo_period[4];
extern volatile u16 servo_count;

void servo_gpio_init();
void servo_init();
void servo_set_pos(u16 num, u16 position);
void TIM3_IRQHandler(void);

#endif // SERVO_H