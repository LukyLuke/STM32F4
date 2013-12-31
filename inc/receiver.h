/** @file    receiver.c
 *  @author  Lukas Zurschmiede <lukas@ranta.ch>
 *  @email   <lukas@ranta.ch>
 *  @version 0.0.1
 *  @date    2014-01-01
 *  @brief   This file contains all needed functions for reading out commands from
 *           a remote controll system used in scale modelling.
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
#ifndef RECEIVER_H
#define RECEIVER_H

// Include STM32F4x libraries we need here
#include "../lib/inc/stm32f4xx.h"
#include "../lib/inc/system_stm32f4xx.h"
#include "../lib/inc/peripherals/stm32f4xx_gpio.h"
#include "../lib/inc/peripherals/stm32f4xx_rcc.h"
#include "../lib/inc/peripherals/stm32f4xx_tim.h"
#include "../lib/inc/peripherals/misc.h" // High level functions for NVIC and SysTick (add-on to CMSIS functions)

// Receiver ports
#define RECEIVER1 GPIO_Pin_7
#define RECEIVER2 GPIO_Pin_8
#define RECEIVER3 GPIO_Pin_9
#define RECEIVER4 GPIO_Pin_10
#define RECEIVER5 GPIO_Pin_11
#define RECEIVER6 GPIO_Pin_12
#define RECEIVER7 GPIO_Pin_13
#define RECEIVER8 GPIO_Pin_14
#define RECEIVER_REGISTER GPIOE
#define RECEIVER_PORTS (RECEIVER1 | RECEIVER2 | RECEIVER3 | RECEIVER4 | RECEIVER5 | RECEIVER6 | RECEIVER7 | RECEIVER8)

// Receiver resolution:
// A servo needs a signal from 1-2 ms and a beginning high of 1ms
// The timer is running on 1MHz; a persiod of 20000 is a tick every 20µs
#define RECEIVER_TIM_PRESCALE 42
#define RECEIVER_TIM_PERIOD 20
#define RECEIVER_TIM_COUNTER 1000
#define RECEIVER_TIM_MICROSECOND 100

extern volatile u16 receiver_position[8];
extern volatile u16 receiver_position_read[8];
extern volatile u16 receiver_count;

void receiver_gpio_init();
void receiver_init();
u16 receiver_get_pos(u16 num);
void TIM2_IRQHandler(void);

#endif // RECEIVER_H
