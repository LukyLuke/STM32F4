/** @file    sensors.c
 *  @author  Lukas Zurschmiede <lukas@ranta.ch>
 *  @email   <lukas@ranta.ch>
 *  @version 0.0.1
 *  @date    2014-01-01
 *  @brief   This file contains all needed functions for working with a LIS302DL sensor
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
#include "../inc/lis302dl.h"

/**** Private declarations ****/

static uint8_t _LIS302DL_SendByte(SPI_TypeDef* spi, uint8_t byte);


/**** Public implementations ****/

void LIS302DL_Init(SPI_TypeDef* spi, const LIS302DL_Config* pConfig) {
	// Map the configuration to be always BigEndian
	uint8_t cnf = 0x00;
	cnf |= pConfig->DataRate ? LIS302DL_BIT0 : 0x00;
	cnf |= pConfig->PowerDown ? LIS302DL_BIT1 : 0x00;
	cnf |= pConfig->FullScale ? LIS302DL_BIT2 : 0x00;
	cnf |= pConfig->SelfTest_P ? LIS302DL_BIT3 : 0x00;
	cnf |= pConfig->SelfTest_M ? LIS302DL_BIT4 : 0x00;
	cnf |= pConfig->ZAxisEnabled ? LIS302DL_BIT5 : 0x00;
	cnf |= pConfig->YAxisEnabled ? LIS302DL_BIT6 : 0x00;
	cnf |= pConfig->XAxisEnabled ? LIS302DL_BIT7 : 0x00;
	
	LIS302DL_Write(&cnf, LIS302DL_CTRL_REG1_ADDR, 1);
}


void LIS302DL_GetConfiguration(SPI_TypeDef* spi, volatile LIS302DL_Config* pConfig) {
	uint8_t cnf = 0x00;
	LIS302DL_Read(&cnf, LIS302DL_CTRL_REG1_ADDR, 1);
	
	// Map received data into the configuration; Data is delivered in BigEndian
	pConfig->DataRate = cnf & LIS302DL_BIT0 ? 0x01 : 0x00;
	pConfig->PowerDown = cnf & LIS302DL_BIT1 ? 0x01 : 0x00;
	pConfig->FullScale = cnf & LIS302DL_BIT2 ? 0x01 : 0x00;
	pConfig->SelfTest_P = cnf & LIS302DL_BIT3 ? 0x01 : 0x00;
	pConfig->SelfTest_M = cnf & LIS302DL_BIT4 ? 0x01 : 0x00;
	pConfig->ZAxisEnabled = cnf & LIS302DL_BIT5 ? 0x01 : 0x00;
	pConfig->YAxisEnabled = cnf & LIS302DL_BIT6 ? 0x01 : 0x00;
	pConfig->XAxisEnabled = cnf & LIS302DL_BIT7 ? 0x01 : 0x00;
}


void LIS302DL_Read(SPI_TypeDef* spi, uint8_t* pBuffer, uint8_t readAddr, uint16_t numByteToRead) {
	// For reading multiple bytes we need to set bit 0 (RW) and 1 (MS)
	if(numByteToRead > 1) {
		readAddr |= (uint8_t)(LIS302DL_BIT0 | LIS302DL_BIT1);
	} else {
		readAddr |= (uint8_t)LIS302DL_BIT0;
	}
	
	// Set chip select Low at the start of the transmission
	LIS302DL_CS_LOW();
	
	// Send the address of the indexed register
	_LIS302DL_SendByte(spi, readAddr);
	
	// Receive the data that will be read from the device (MSB First)
	// Send a dummy byte (0x00) to generate the SPI clock to LIS302DL (Slave device) and receive data.
	while (numByteToRead > 0) {
		*pBuffer = _LIS302DL_SendByte(spi, (uint8_t)LIS302DL_DUMMY_BYTE);
		numByteToRead--;
		pBuffer++;
	}
	
	// Set chip select High at the end of the transmission
	LIS302DL_CS_HIGH();
}


void LIS302DL_Write(SPI_TypeDef* spi, uint8_t* pBuffer, uint8_t writeAddr, uint16_t numByteToWrite) {
	// For writing multiple bytes we need to set bit 1 (MS)
	if (numByteToWrite > 1) {
		writeAddr |= (uint8_t)LIS302DL_BIT1;
	}
	
	// Set chip select Low at the start of the transmission
	LIS302DL_CS_LOW();
	
	LIS302DL_Read(spi);
	
	// Send the Address of the indexed register
	_LIS302DL_SendByte(spi, writeAddr);
	
	// Send the data that will be written into the device (MSB First)
	while (numByteToWrite >= 1) {
		_LIS302DL_SendByte(spi, *pBuffer);
		numByteToWrite--;
		pBuffer++;
	}
	
	// Set chip select High at the end of the transmission
	LIS302DL_CS_HIGH();
}


void LIS302DL_Acceleration(SPI_TypeDef* spi, int32_t* out) {
	uint8_t buffer[6];
	uint8_t crtl, i = 0;
	
	// Read out the Control-Register-1 to get the FS value
	LIS302DL_Read(spi, &crtl, LIS302DL_CTRL_REG1_ADDR, 1);
	
	// Read out the X,Y,Z output data. Between them is one byte each which is not used, therefore 6 bytes
	LIS302DL_Read(spi, buffer, LIS302DL_OUT_X_ADDR, 6);
	
	// Check for the FS bit is set or not.
	// Multiply axis values wtith the sensitivity and all these values with each other.
	// Only use the first, third and fifth byte, the second and third is not used.
	// OUT_X (29), OUT_Y (2B), OUT_Z (2D) --> 2A and 2C is not used --> buffer[2 * i]
	switch (crtl & (uint8_t)LIS302DL_BIT2) {
		// FS bit is 0 ==> Sensitivity typical value = 18 milligals/digit
		case 0:
			for (i = 0; i < 3; i++) {
				*out = (int32_t)(18 * (i8)buffer[2 * i]);
				out++;
			}
			break;
		
		// FS bit is 1 ==> Sensitivity typical value = 72 milligals/digit
		case (uint8_t)LIS302DL_BIT2:
			for(i = 0; i < 3; i++) {
				*out = (int32_t)(72 * (i8)buffer[2 * i]);
				out++;
			}         
			break;
		default:
			break;
	}
}


void LIS302DL_Reboot(SPI_TypeDef* spi) {
	uint8_t tmpreg;
	
	// Read the CTRL_REG2, enable the reboot memory flag and write it back
	LIS302DL_Read(spi, &tmpreg, LIS302DL_CTRL_REG2_ADDR, 1);
	tmpreg |= (uint8_t)LIS302DL_BIT1;
	LIS302DL_Write(spi, &tmpreg, LIS302DL_CTRL_REG2_ADDR, 1);
}


void LIS302DL_ChangeScaleMode(SPI_TypeDef* spi, bool fullScaleEnable) {
	uint8_t tmpreg;
	
	// Read the CTRL_REG1, enable/disable the FS memory flag and write it back
	LIS302DL_Read(spi, &tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
	tmpreg &= (uint8_t)~LIS302DL_BIT2; // Unset the FS bit
	if (!fullScaleEnable) { // If the bit is not set, we are in ±2.3g mode (full scale)
		tmpreg |= (uint8_t)LIS302DL_BIT2;
	}
	LIS302DL_Write(spi, &tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
}


void LIS302D_ChangeDataRate(SPI_TypeDef* spi, bool enableHighSpeed) {
	uint8_t tmpreg;
	
	// Read the CTRL_REG1, enable/disable the DR memory flag and write it back
	LIS302DL_Read(spi, &tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
	tmpreg &= (uint8_t)~LIS302DL_BIT0; // Unset the FS bit
	if (!enableHighSpeed) { // If the bit is set, we are on 400Hz, otherwise on 100Hz
		tmpreg |= (uint8_t)LIS302DL_BIT0;
	}
	LIS302DL_Write(spi, &tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
}


void LIS302D_ChangePowerControl(SPI_TypeDef* spi, bool enableActiveMode) {
	uint8_t tmpreg;
	
	// Read the CTRL_REG1, enable/disable the PD memory flag and write it back
	LIS302DL_Read(spi, &tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
	tmpreg &= (uint8_t)~LIS302DL_BIT1; // Unset the FS bit
	if (enableActiveMode) { // If the bit is set, we are in active mode, else in power down control
		tmpreg |= (uint8_t)LIS302DL_BIT1;
	}
	LIS302DL_Write(spi, &tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
}

/**** Private implementations ****/

/**
 * @brief  Send one Byte through the SPI interface and return the received Byte
 * @param  uint8_t byte  The byte to send
 * @retval uint8_t The received byte value
 */
static uint8_t _LIS302DL_SendByte(SPI_TypeDef* spi, uint8_t byte) {
	// TODO: Make this configurable so more than only one SPI port/sensor can be used.
	volatile uint32_t _LIS302DL_Timeout = LIS302DL_MAX_TIMEOUT;
	
	// Loop while DR register in not empty; or we ran into a timeout
	_LIS302DL_Timeout = LIS302DL_MAX_TIMEOUT;
	while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET) {
		if ((_LIS302DL_Timeout--) == 0) {
			return LIS302DL_TIMEOUT_UserCallback();
		}
	}
	
	// Send a Byte through the SPI peripheral
	SPI_I2S_SendData(spi, byte);
	
	// Wait to receive a Byte; or we ran into a timeout
	_LIS302DL_Timeout = LIS302DL_MAX_TIMEOUT;
	while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET) {
		if ((_LIS302DL_Timeout--) == 0) {
			return LIS302DL_TIMEOUT_UserCallback();
		}
	}
	
	/* Return the Byte read from the SPI bus */
	return (uint8_t)SPI_I2S_ReceiveData(spi);
}

#ifndef LIS302DL_USE_CUSTOM_TIMEOUT_CALLBACK
/**
 * @brief  Basic management for timeouts blocks all further communications by default and therefore halt the application
 * @param  None
 * @retval None
 */
uint32_t LIS302DL_TIMEOUT_UserCallback(void) {
	while (1);
}
#endif // LIS302DL_USE_CUSTOM_TIMEOUT_CALLBACK
