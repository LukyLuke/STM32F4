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
static u8 _LIS302DL_SendByte(u8 byte);


/**** Public methods ****/

void LIS302DL_Configure(const LIS302DL_Config *pConfig) {
	
}

void LIS302DL_GetConfiguration(volatile LIS302DL_Config *pConfig) {
	
}

void LIS302DL_Read(u8 *pBuffer, u8 readAddr, u16 numByteToRead) {
	// For reading multiple bytes we need to set bit 0 (RW) and 1 (MS)
	if(numByteToRead > 1) {
		readAddr |= (u8)(0x40 | 0x80);
	} else {
		readAddr |= (u8)0x40;
	}
	
	// Set chip select Low at the start of the transmission
	LIS302DL_CS_LOW();
	
	// Send the address of the indexed register
	_LIS302DL_SendByte(readAddr);
	
	// Receive the data that will be read from the device (MSB First)
	// Send a dummy byte (0x00) to generate the SPI clock to LIS302DL (Slave device) and receive data.
	while (numByteToRead > 0) {
		*pBuffer = _LIS302DL_SendByte(LIS302DL_DUMMY_BYTE);
		numByteToRead--;
		pBuffer++;
	}
	
	// Set chip select High at the end of the transmission
	LIS302DL_CS_HIGH();
}

void LIS302DL_Write(u8 *pBuffer, u8 writeAddr, u16 numByteToWrite) {
	// For writing multiple bytes we need to set bit 1 (MS)
	if(numByteToWrite > 0x01) {
		writeAddr |= (u8)0x40;
	}
	// Set chip select Low at the start of the transmission
	LIS302DL_CS_LOW();
	
	LIS302DL_Read();
	
	// Send the Address of the indexed register
	_LIS302DL_SendByte(writeAddr);
	
	// Send the data that will be written into the device (MSB First)
	while(numByteToWrite >= 0x01) {
		_LIS302DL_SendByte(*pBuffer);
		numByteToWrite--;
		pBuffer++;
	}
	
	// Set chip select High at the end of the transmission
	LIS302DL_CS_HIGH();
}

void LIS302DL_Acceleration(i32 *out) {
	u8 buffer[6];
	u8 crtl, i = 0;
	
	// Read out the Control-Register-1 to get the FS value
	LIS302DL_Read(&crtl, LIS302DL_CTRL_REG1_ADDR, 1);
	
	// Read out the X,Y,Z output data. Between them is one byte each which is not used, therefore 6 bytes
	LIS302DL_Read(buffer, LIS302DL_OUT_X_ADDR, 6);
	
	// Check for the FS bit is set or not.
	// Multiply axis values wtith the sensitivity and all these values with each other.
	// Only use the first, third and fifth byte, the second and third is not used.
	// OUT_X (29), OUT_Y (2B), OUT_Z (2D) --> 2A and 2C is not used --> buffer[2 * i]
	switch (crtl & 0x20) {
		// FS bit is 0 ==> Sensitivity typical value = 18 milligals/digit
		case 0x00:
			for (i = 0; i < 3; i++) {
				*out = (i32)(18 * (i8)buffer[2 * i]);
				out++;
			}
			break;
		
		// FS bit is 1 ==> Sensitivity typical value = 72 milligals/digit
		case 0x20:
			for(i = 0; i < 3; i++) {
				*out = (i32)(72 * (i8)buffer[2 * i]);
				out++;
			}         
			break;
		default:
			break;
	}
}

/**
 * @brief  Send one Byte through the SPI interface and return the received Byte
 * @param  u8 byte  The byte to send
 * @retval u8 The received byte value
 */
static u8 _LIS302DL_SendByte(u8 byte) {
	// TODO: Make this configurable so more than only one SPI port/sensor can be used.
	volatile u32 _LIS302DL_Timeout = LIS302DL_MAX_TIMEOUT;
	
	// Loop while DR register in not empty; or we ran into a timeout
	_LIS302DL_Timeout = LIS302DL_MAX_TIMEOUT;
	while (SPI_I2S_GetFlagStatus(LIS302DL_SPI, SPI_I2S_FLAG_TXE) == RESET) {
		if ((_LIS302DL_Timeout--) == 0) {
			return LIS302DL_TIMEOUT_UserCallback();
		}
	}
	
	// Send a Byte through the SPI peripheral
	SPI_I2S_SendData(LIS302DL_SPI, byte);
	
	// Wait to receive a Byte; or we ran into a timeout
	_LIS302DL_Timeout = LIS302DL_MAX_TIMEOUT;
	while (SPI_I2S_GetFlagStatus(LIS302DL_SPI, SPI_I2S_FLAG_RXNE) == RESET) {
		if ((_LIS302DL_Timeout--) == 0) {
			return LIS302DL_TIMEOUT_UserCallback();
		}
	}
	
	/* Return the Byte read from the SPI bus */
	return (u8)SPI_I2S_ReceiveData(LIS302DL_SPI);
}

#ifndef LIS302DL_USE_CUSTOM_TIMEOUT_CALLBACK
/**
 * @brief  Basic management for timeouts blocks all further communications by default and therefore halt the application
 * @param  None
 * @retval None
 */
u32 LIS302DL_TIMEOUT_UserCallback(void) {
	while (1);
}
#endif // LIS302DL_USE_CUSTOM_TIMEOUT_CALLBACK
