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
#include "../inc/sensors.h"


void sensors_init_gpio() {
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	// Enable the SPI periphery for LIS203DL (Discovery-Kit OnBoard gyro)
	RCC_APB2PeriphClockCmd(SENSORS_SPI_CLK, ENABLE);
	
	// Enable SCK-, MOSI- and MISO-GPIO clocks (on register GPIOA)
	RCC_AHB1PeriphClockCmd(SENSORS_SPI_SCK_GPIO_CLK | SENSORS_SPI_MISO_GPIO_CLK | SENSORS_SPI_MOSI_GPIO_CLK, ENABLE);
	
	// Enable CS-, INT1- and INT2-GPIO clocks (on register GPIOE)
	RCC_AHB1PeriphClockCmd(SENSORS_SPI_CS_GPIO_CLK | SENSORS_SPI_INT1_GPIO_CLK | SENSORS_SPI_INT2_GPIO_CLK, ENABLE);
	
	// Configure alternate functions for SCK, MISO and MOIS ports
	GPIO_PinAFConfig(SENSORS_SPI_SCK_GPIO_PORT,  SENSORS_SPI_SCK_SOURCE,  SENSORS_SPI_SCK_AF);
	GPIO_PinAFConfig(SENSORS_SPI_MISO_GPIO_PORT, SENSORS_SPI_MISO_SOURCE, SENSORS_SPI_MISO_AF);
	GPIO_PinAFConfig(SENSORS_SPI_MOSI_GPIO_PORT, SENSORS_SPI_MOSI_SOURCE, SENSORS_SPI_MOSI_AF);
	
	// Initialize all needed GPIO pins and registers in alternate function mode
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	// SPI SCK pin configuration (Serial Clock)
	GPIO_InitStructure.GPIO_Pin = SENSORS_SPI_SCK_PIN;
	GPIO_Init(SENSORS_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
	
	// SPI MOSI pin configuration (Master Out, Slave In)
	GPIO_InitStructure.GPIO_Pin = SENSORS_SPI_MOSI_PIN;
	GPIO_Init(SENSORS_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
	
	// SPI MISO pin configuration (Master In, Slave Out)
	GPIO_InitStructure.GPIO_Pin = SENSORS_SPI_MISO_PIN;
	GPIO_Init(SENSORS_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
	
	// Configure SPI for the LIS203DL sensor on SPI-1 to send/receive bytes in fullduplex mode (4-wire)
	SPI_I2S_DeInit(SENSORS_SPI);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SENSORS_SPI, &SPI_InitStructure);
	
	// Enable the SPI-1 bus
	SPI_Cmd(SENSORS_SPI, ENABLE);
	
	// Configure GPIO PIN for LIS203DL Chip select
	GPIO_InitStructure.GPIO_Pin   = SENSORS_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SENSORS_SPI_CS_GPIO_PORT, &GPIO_InitStructure);
	
	// Bring the chip up / enable
	GPIO_SetBits(SENSORS_SPI_CS_GPIO_PORT, SENSORS_SPI_CS_PIN);
	
	// Configure GPIO pins to detect Interrupts
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	// Interrupt 1 from LIS203DL
	GPIO_InitStructure.GPIO_Pin = SENSORS_SPI_INT1_PIN;
	GPIO_Init(SENSORS_SPI_INT1_GPIO_PORT, &GPIO_InitStructure);
	
	// Interrupt 2 from LIS203DL
	GPIO_InitStructure.GPIO_Pin = SENSORS_SPI_INT2_PIN;
	GPIO_Init(SENSORS_SPI_INT2_GPIO_PORT, &GPIO_InitStructure);
}
