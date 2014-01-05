/** @file    sensors.c
 *  @author  Lukas Zurschmiede <lukas@ranta.ch>
 *  @email   <lukas@ranta.ch>
 *  @version 0.0.1
 *  @date    2014-01-05
 *  @see     http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00265057.pdf
 *  @brief   This file contains all needed functions for working with a L3G4200D sensor
 *           The L3G4200D is a three-axis angular rate sensor
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
#ifndef L3G4200D_H
#define L3G4200D_H

// TODO: Decouple this from the STM32F4 processor and discovery board
#include "../../lib/inc/stm32f4xx.h"
#include "../../lib/inc/peripherals/misc.h"
#include "../../lib/inc/peripherals/stm32f4xx_gpio.h"
#include "../../lib/inc/peripherals/stm32f4xx_spi.h"



/**** BEGIN: L3G4200D-Registers ****/

/**
 * @def L3G4200D_WHO_AM_I_ADDR
 * @brief Device Identification Register
 * 
 *  Read only register
 *  Default value: 0xD3
 * 
 *  This register contains the device identifier and is set to 0xD3 for the L3G4200D
 */
#define L3G4200D_WHO_AM_I_ADDR             0x0F

/**
 * @def L3G4200D_CTRL_REG1_ADDR
 * @brief Control register 1 from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x07 (0000 0111)
 * 
 *  7-6 DR: Data Rate selection in Hz
 *          DR | BW | ODR | Cut-Off
 *         ----+----+-----+---------
 *          00 | 00 | 100 |  12.5    (default)
 *          00 | 01 | 100 |  25.0
 *          00 | 10 | 100 |  25.0
 *          00 | 11 | 100 |  25.0
 *          01 | 00 | 200 |  12.5
 *          01 | 01 | 200 |  25.0
 *          01 | 10 | 200 |  50.0
 *          01 | 11 | 200 |  70.0
 *          10 | 00 | 400 |  20.0
 *          10 | 01 | 400 |  25.0
 *          10 | 10 | 400 |  50.0
 *          10 | 11 | 400 | 110.0
 *          11 | 00 | 800 |  30.0
 *          11 | 01 | 800 |  35.0
 *          11 | 10 | 800 |  50.0
 *          11 | 11 | 800 | 110.0
 * 
 *  5-4 BW: Bandwidth selection - see table above, this defines the Cut-Off
 * 
 *  3 PD: Power Down control
 *          0: Power down mode (default)
 *          1: Active mode
 * 
 *  2 Zen: Z axis enable
 *          0: Z axis disabled
 *          1: Z axis enabled (default)
 * 
 *  1 Yen: Y axis enable
 *          0: Y axis disabled
 *          1: Y axis enabled (default)
 * 
 *  0 Xen: X axis enable
 *          0: X axis disabled
 *          1: X axis enabled (default)
 */
#define L3G4200D_CTRL_REG1_ADDR             0x20

/**
 * @def L3G4200D_CTRL_REG2_ADDR
 * @brief Control register 2 from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7-6 --: Unused, loaded at boot time and should not be set
 * 
 *  5-4 HPM: High Pass filter Mode Selection
 *          00: Normal mode, reset reading HP_RESET FILTER (default)
 *          01: Reference signal for filtering
 *          10: Normal mode, no reset of HP_RESET_FILTER
 *          11: Auto-Reset on interrupt event
 *  
 *  3-0 HPCF: High Pass Cut Off frequency selection in Hz
 *               |  ODR  |  ODR  |  ODR  |  ODR
 *          HPCF | 100Hz | 200Hz | 400Hz | 800Hz
 *         ------+-------+-------+-------+-------
 *          0000 |   8Hz |  15Hz |  30Hz |  56Hz
 *          0001 |   4Hz |   8Hz |  15Hz |  30Hz
 *          0010 |   2Hz |   4Hz |   8Hz |  15Hz
 *          0011 |   1Hz |   2Hz |   4Hz |   8Hz
 *          0100 |  .5Hz |   1Hz |   2Hz |   4Hz
 *          0101 |  .2Hz |  .5Hz |   1Hz |   2Hz
 *          0110 |  .1Hz |  .2Hz |  .5Hz |   1Hz
 *          0111 | .05Hz |  .1Hz |  .2Hz |  .5Hz
 *          1000 | .02Hz | .05Hz |  .1Hz |  .2Hz
 *          1001 | .01Hz | .02Hz | .05Hz |  .1Hz
 */
#define L3G4200D_CTRL_REG2_ADDR             0x21

/**
 * @def L3G4200D_CTRL_REG3_ADDR
 * @brief Control register 3 from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7 I1_Int1: Interrupt enable
 *          0: Disable Interrupts (default)
 *          1: Enable Interrupts
 * 
 *  6 I1_Boot: Boot staus enabled on Interrupt 1
 *          0: Disable (default)
 *          1: Enable
 * 
 *  5 H_Lactive: Interrupt 1 active configuration
 *          0: High active (default)
 *          1: Low active
 * 
 *  4 PP_OD: Push-Pull / Open drain
 *          0: Push-Pull (default)
 *          1: Open drain
 * 
 *  3 I2_DRDY: Data ready interrupt on INT2
 *          0: Disable (default)
 *          1: Enable
 * 
 *  2 I2_WTM: FIFO Watermark interrupt on INT2
 *          0: Disable (default)
 *          1: Enable
 * 
 *  1 I2_ORun: FIFO Overrun interrupt on INT2
 *          0: Disable (default)
 *          1: Enable
 * 
 *  0 I2_Empty: FIFO Empty interrupt on INT2
 *          0: Disable (default)
 *          1: Enable
 */
#define L3G4200D_CTRL_REG3_ADDR             0x22

/**
 * @def L3G4200D_CTRL_REG4_ADDR
 * @brief Control register 4 from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7 BDU: Block Data Update
 *          0: Continous update (default)
 *          1: Output register are not updated until MSB and LSB reading
 * 
 *  6 BLE: Big/Little endian data selection
 *          0: Data in Big-Endian (default)
 *          1: Data in Little-Endian
 * 
 *  5-4 FS: Full Scale selection
 *          00: 250 dps (default)
 *          01: 500 dps
 *          10: 2000 dps
 *          11: 2000 dps
 * 
 *  3 --: not used
 * 
 *  2-1 ST: Self-Test enable
 *          00: Normal mode (default)
 *          01: Self test 0 (+ DST sign)
 *          10: -- nothing
 *          11: Self test 0 (- DST sign)
 * 
 *  0 SIM: SPI Serial Interface Mode selection
 *          0: 4-wire interface (default)
 *          1: 3-wire interface
 */
#define L3G4200D_CTRL_REG4_ADDR             0x23

/**
 * @def L3G4200D_CTRL_REG5_ADDR
 * @brief Control register 5 from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7 BOOT: Reboot memory content
 *          0: Normal mode (default)
 *          1: Reboot the memory content
 * 
 *  6 FIFO_EN: Enable FIFO
 *          0: Disable (default)
 *          1: Enable
 * 
 *  5 --: Not used
 * 
 *  4 HPen: High Pass filter enable
 *          0: HPF Disabled (default)
 *          1: HPF Enabled
 * 
 *  3-2 INT1_Sel: Interrupt 1 selection configuration
 *          HPen | INT1_Sel | Data used for interrupts
 *         ------+----------+----------------------------
 *            x  |    00    | Non-High-pass-filtered
 *            x  |    00    | High-pass-filtered
 *            0  |    1x    | Low-pass-filtered
 *            1  |    1x    | Low-/High-pass-filtered
 * 
 *  1-0 OUT_Sel: Output selection configuration
 *          HPen | OUT_Sel | Data in DataReg and FIFO
 *         ------+---------+----------------------------
 *            x  |    00   | Non-High-pass-filtere
 *            x  |    00   | High-pass-filtered
 *            0  |    1x   | Low-apss-filtered
 *            1  |    1x   | Low-/High-pass-filtered
 */
#define L3G4200D_CTRL_REG5_ADDR             0x24

/**
 * @def L3G4200D_REFERENCE_ADDR
 * @brief Reference Register from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  Reference value for interrupt generation.
 * TODO: Find out what this is for?!?
 */
#define L3G4200D_REFERENCE_ADDR             0x25

/**
 * @def L3G4200D_OUT_TEMP_ADDR
 * @brief Reading temparature register from L3G4200D
 * 
 *  Read only register
 *  Default value: output
 * 
 *  Temperature data.
 */
#define L3G4200D_OUT_TEMP_ADDR              0x26

/**
 * @def L3G4200D_STATUS_REG_ADDR
 * @brief Status register from L3G4200D
 * 
 *  Read only register
 *  Default value: output
 * 
 *  7 ZYXOR: X, Y, Z-axis data overrun
 *          0: No overrun has occurred (default)
 *          1: New data has overritten the previous one before it was read
 * 
 *  6 ZOR: Z-axis overrun
 *          0: No overrun has occurred (default)
 *          1: New data has overritten the previous one before it was read
 * 
 *  5 YOR: Y-axis overrun
 *          0: No overrun has occurred (default)
 *          1: New data has overritten the previous one before it was read
 * 
 *  4 XOR: X-axis overrun
 *          0: No overrun has occurred (default)
 *          1: New data has overritten the previous one before it was read
 * 
 *  3 ZYXDA: X, Y, Z-axis new data available
 *          0: No new data available (default)
 *          1: New data available
 * 
 *  2 ZDA: Z-axis new data available
 *          0: No new data available (default)
 *          1: New data available
 * 
 *  1 YDA: Y-axis new data available
 *          0: No new data available (default)
 *          1: New data available
 * 
 *  0 XDA: X-axis new data available
 *          0: No new data available (default)
 *          1: New data available
 */
#define L3G4200D_STATUS_REG_ADDR            0x27

/**
 * @def L3G4200D_OUT_X_L_ADDR
 * @brief X Low data register from L3G4200D
 * 
 *  Read only register
 *  Default value: output
 * 
 *  X-Axis angular rate data. The value is expressed as two's complement.
 *  See OUT_X_H for the high complement.
 */
#define L3G4200D_OUT_X_L_ADDR               0x28

/**
 * @def L3G4200D_OUT_X_H_ADDR
 * @brief X High data register from L3G4200D
 * 
 *  Read only register
 *  Default value: output
 * 
 *  X-Axis angular rate data. The value is expressed as two's complement.
 *  See OUT_X_L for the low complement.
 */
#define L3G4200D_OUT_X_H_ADDR               0x29

/**
 * @def L3G4200D_OUT_Y_L_ADDR
 * @brief Y Low data register from L3G4200D
 * 
 *  Read only register
 *  Default value: output
 * 
 *  Y-Axis angular rate data. The value is expressed as two's complement.
 *  See OUT_Y_H for the high complement.
 */
#define L3G4200D_OUT_Y_L_ADDR               0x2A

/**
 * @def L3G4200D_OUT_Y_H_ADDR
 * @brief Y High data register from L3G4200D
 * 
 *  Read only register
 *  Default value: output
 * 
 *  Y-Axis angular rate data. The value is expressed as two's complement.
 *  See OUT_Y_L for the low complement.
 */
#define L3G4200D_OUT_Y_H_ADDR               0x2B

/**
 * @def L3G4200D_OUT_Z_L_ADDR
 * @brief Z Low data register from L3G4200D
 * 
 *  Read only register
 *  Default value: output
 * 
 *  Z-Axis angular rate data. The value is expressed as two's complement.
 *  See OUT_Z_H for the high complement.
 */
#define L3G4200D_OUT_Z_L_ADDR               0x2C

/**
 * @def L3G4200D_OUT_Z_H_ADDR
 * @brief Z High reading register from L3G4200D
 * 
 *  Read only register
 *  Default value: output
 * 
 *  Z-Axis angular rate data. The value is expressed as two's complement.
 *  See OUT_Z_L for the low complement.
 */
#define L3G4200D_OUT_Z_H_ADDR               0x2D

/**
 * @def L3G4200D_FIFO_CTRL_REG_ADDR
 * @brief The FIFO Control register from L3G4200D
 * 
 *  Read Write register
 *  Default value: output
 * 
 *  7-5 FM: FIFO Mode selection
 *          000: Bypass mode (default)
 *          001: FIFO mode
 *          010: Stream mode
 *          011: Stream-to-FIFO mode
 *          100: Bypass-to-Stream mode
 * 
 *  4-0 WTM: FIFO threshold watermark setting.
 */
#define L3G4200D_FIFO_CTRL_REG_ADDR         0x2E

/**
 * @def L3G4200D_FIFO_SRC_REG_ADDR
 * @brief The FIFO Source register from L3G4200D
 * 
 *  Read only register
 *  Default value: 0x00
 * 
 *  7 WTM: Watermark status
 *          0: FIFO filling lower than WTM-Level (default)
 *          1: FIFO filling is equal or higher than WTM-Level
 * 
 *  6 OVRN: FIFO Overrun status
 *          0: FIFO is not completely filled (default)
 *          1: FIFO is full
 * 
 *  5 EMPTY:FIFO Empty status
 *          0: FIFO is not empty (default)
 *          1: FIFO is empty
 * 
 *  4-0 FSS: FIFO stored data level
 */
#define L3G4200D_FIFO_SRC_REG_ADDR          0x2F

/**
 * @def L3G4200D_INT1_CFG_ADDR
 * @brief Interrupt 1 configuration register from L3G4200D
 * 
 *  Read Write register
 *  Default value: output
 * 
 *  7 ANDOR: AND/OR combination of interrupts.
 *          0: OR combination (each interrupt event is fired solely) (default)
 *          1: AND combination (more than one interrupt data change can occur in one interrupt)
 * 
 *  6 LIR: Latch Interrupt Request
 *          0: Interrupt request is not latched (default)
 *          1: Interrupt request is latched
 * 
 *  5 ZHIE: Enable Interrupt generation on Z high
 *          0: Disable interrupt request (default)
 *          1: Enable interrupt request on measured accellerated value higher than the preset threshold
 * 
 *  4 ZLIE: Enable Interrupt generation on Z low
 *          0: Disable interrupt request (default)
 *          1: Enable interrupt request on measured accellerated value lower than the preset threshold
 * 
 *  3 YHIE: Enable Interrupt generation on Y high
 *          0: Disable interrupt request (default)
 *          1: Enable interrupt request on measured accellerated value higher than the preset threshold
 * 
 *  2 YLIE: Enable Interrupt generation on Y low
 *          0: Disable interrupt request (default)
 *          1: Enable interrupt request on measured accellerated value lower than the preset threshold
 * 
 *  1 XHIE: Enable Interrupt generation on X high
 *          0: Disable interrupt request (default)
 *          1: Enable interrupt request on measured accellerated value higher than the preset threshold
 * 
 *  0 XLIE: Enable Interrupt generation on X low
 *          0: Disable interrupt request (default)
 *          1: Enable interrupt request on measured accellerated value lower than the preset threshold
 */
#define L3G4200D_INT1_CFG_ADDR              0x30

/**
 * @def L3G4200D_INT1_SRC_ADDR
 * @brief Interrupt 1 source register from L3G4200D
 * 
 *  Read only register
 *  Default value: output
 * 
 *  Reading at this address clears INT1_SRC IA bit (and eventually the interrupt signal on INT1 pin)
 *  and allows the refreshment of data in the INT1_SRC register if the latched option was chosen
 * 
 *  7 --: Not used, default to 0
 * 
 *  6 IA: Interrupt active
 *          0: No interrupt has been generated (default)
 *          1: One or more interrupts have been generated
 * 
 *  5 ZH: Interrupt on Z-High event
 *          0: No interrupt generated (default)
 *          1: Interrupt generated
 * 
 *  4 ZL: Interrupt on Z-Low event
 *          0: No interrupt generated (default)
 *          1: Interrupt generated
 * 
 *  3 YH: Interrupt on Y-High event
 *          0: No interrupt generated (default)
 *          1: Interrupt generated
 * 
 *  2 YL: Interrupt on Y-Low event
 *          0: No interrupt generated (default)
 *          1: Interrupt generated
 * 
 *  1 XH: Interrupt on X-High event
 *          0: No interrupt generated (default)
 *          1: Interrupt generated
 * 
 *  0 XL: Interrupt on X-Low event
 *          0: No interrupt generated (default)
 *          1: Interrupt generated
 */
#define L3G4200D_INT1_SRC_ADDR              0x31

/**
 * @def L3G4200D_INT1_TSH_XH_ADDR
 * @brief Interrupt 1 Threshold for X-High from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7 --: Not used
 * 
 *  6-0 THSX: Interrupt threshold high bits 8-14,
 *            see INT1_TSH_XL for the low bits 0-7
 */
#define L3G4200D_INT1_TSH_XH_ADDR           0x32

/**
 * @def L3G4200D_INT1_TSH_XL_ADDR
 * @brief Interrupt 1 Threshold for X-Low from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7-0 THSX: Interrupt threshold low bits 0-7,
 *            see INT1_TSH_XL for the high bits 8-14
 */
#define L3G4200D_INT1_TSH_XL_ADDR           0x33

/**
 * @def L3G4200D_INT1_TSH_YH_ADDR
 * @brief Interrupt 1 Threshold for Y-High from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7 --: Not used
 * 
 *  6-0 THSY: Interrupt threshold high bits 8-14,
 *            see INT1_TSH_YL for the low bits 0-7
 */
#define L3G4200D_INT1_TSH_YH_ADDR           0x34

/**
 * @def L3G4200D_INT1_TSH_YL_ADDR
 * @brief Interrupt 1 Threshold for Y-Low from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7-0 THSY: Interrupt threshold low bits 0-7,
 *            see INT1_TSH_YL for the high bits 8-14
 */
#define L3G4200D_INT1_TSH_YL_ADDR           0x35

/**
 * @def L3G4200D_INT1_TSH_ZH_ADDR
 * @brief Interrupt 1 threshold for Z-High from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7 --: Not used
 * 
 *  6-0 THSZ: Interrupt threshold high bits 8-14,
 *            see INT1_TSH_ZL for the low bits 0-7
 */
#define L3G4200D_INT1_TSH_ZH_ADDR           0x36

/**
 * @def L3G4200D_INT1_TSH_ZL_ADDR
 * @brief Interrupt 1 Threshold for Z-Low from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7-0 THSZ: Interrupt threshold low bits 0-7,
 *            see INT1_TSH_ZH for the high bits 8-14
 */
#define L3G4200D_INT1_TSH_ZL_ADDR           0x37

/**
 * @def L3G4200D_DURATION_ADDR
 * @brief Interrupt duration configuration register from L3G4200D
 * 
 *  Read Write register
 *  Default value: 0x00
 *
 *  An Interrupt is always fired after the defined wait duration if the signal crosses
 *  the selected threshold. If the value falls below/above the threshold before
 *  the defined wait duration ends, no interrupt event is fired.
 * 
 *  If the WAIT bit is disabled (default), the interrupt is disabled immediately if
 *  the signal falls below/above the selected threshold again.
 *  
 *  If the WAIT bit is enabled, the interrupt is disabled after the defined duration
 *  if the signal falls below/above the selected threshold again.
 * 
 *  7 WAIT: WAIT enabled
 *          0: Disabled (default)
 *          1: Enabled
 * 
 *  6-0 D: Wait Duration value
 */
#define L3G4200D_DURATION                   0x38

#endif // L3G4200D_H
