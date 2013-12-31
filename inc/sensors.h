/** @file    sensors.c
 *  @author  Lukas Zurschmiede <lukas@ranta.ch>
 *  @email   <lukas@ranta.ch>
 *  @version 0.0.1
 *  @date    2014-01-01
 *  @brief   This file contains all needed functions for the sensors which are used
 *           in this project for the Quadrocopter.
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
#ifndef SENSORS_H
#define SENSORS_H

#include "../lib/inc/stm32f4xx.h"
#include "../lib/inc/peripherals/misc.h"
#include "../lib/inc/peripherals/stm32f4xx_rcc.h"
#include "../lib/inc/peripherals/stm32f4xx_tim.h"
#include "../lib/inc/peripherals/stm32f4xx_gpio.h"
#include "../lib/inc/peripherals/stm32f4xx_spi.h"

#include "lis302dl.h"

// SPI Interface
#define SENSORS_SPI                       SPI1
#define SENSORS_SPI_CLK                   RCC_APB2Periph_SPI1

// Serial Clock
#define SENSORS_SPI_SCK_PIN               GPIO_Pin_5
#define SENSORS_SPI_SCK_GPIO_PORT         GPIOA
#define SENSORS_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define SENSORS_SPI_SCK_SOURCE            GPIO_PinSource5
#define SENSORS_SPI_SCK_AF                GPIO_AF_SPI1

// Serial Data Out: Master in, Slave out
#define SENSORS_SPI_MISO_PIN              GPIO_Pin_6
#define SENSORS_SPI_MISO_GPIO_PORT        GPIOA
#define SENSORS_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define SENSORS_SPI_MISO_SOURCE           GPIO_PinSource6
#define SENSORS_SPI_MISO_AF               GPIO_AF_SPI1

// Serial Data In: Master out, Slave in
#define SENSORS_SPI_MOSI_PIN              GPIO_Pin_7
#define SENSORS_SPI_MOSI_GPIO_PORT        GPIOA
#define SENSORS_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define SENSORS_SPI_MOSI_SOURCE           GPIO_PinSource7
#define SENSORS_SPI_MOSI_AF               GPIO_AF_SPI1

// Chip select pin
#define SENSORS_SPI_CS_PIN                GPIO_Pin_3
#define SENSORS_SPI_CS_GPIO_PORT          GPIOE
#define SENSORS_SPI_CS_GPIO_CLK           RCC_AHB1Periph_GPIOE

// Interrupt configuration
#define SENSORS_SPI_INT1_PIN              GPIO_Pin_0
#define SENSORS_SPI_INT1_GPIO_PORT        GPIOE
#define SENSORS_SPI_INT1_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define SENSORS_SPI_INT1_EXTI_LINE        EXTI_Line0
#define SENSORS_SPI_INT1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define SENSORS_SPI_INT1_EXTI_PIN_SOURCE  EXTI_PinSource0
#define SENSORS_SPI_INT1_EXTI_IRQn        EXTI0_IRQn

#define SENSORS_SPI_INT2_PIN              GPIO_Pin_1
#define SENSORS_SPI_INT2_GPIO_PORT        GPIOE
#define SENSORS_SPI_INT2_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define SENSORS_SPI_INT2_EXTI_LINE        EXTI_Line1
#define SENSORS_SPI_INT2_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define SENSORS_SPI_INT2_EXTI_PIN_SOURCE  EXTI_PinSource1
#define SENSORS_SPI_INT2_EXTI_IRQn        EXTI1_IRQn

/**
 * @brief  Initialize all IO ports, timers and interrupts, etc. for all sensors
 * @param  None
 * @retval None
 */
void sensors_init_gpio();

#endif // SENSORS_H
