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
#ifndef LIS302DL_H
#define LIS302DL_H

#include "../lib/inc/stm32f4xx.h"
#include "../lib/inc/peripherals/misc.h"
#include "../lib/inc/peripherals/stm32f4xx_gpio.h"
#include "../lib/inc/peripherals/stm32f4xx_spi.h"

// TODO: Make this configurabe on each command through a different API
// use sensors.h for this
#define LIS302DL_SPI  SPI1

// see http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00135460.pdf

/**
 * @def LIS302DL_MAX_TIMEOUT
 * @brief Maximum Timeout for waiting for flags in loops.
 * 
 * These timeout is not based on accurate values, it just guarantee that the
 * application will not remain stuck if the SPI communication is corrupted.
 * You can redefine this in your global configuration file.
 */
#ifndef LIS302DL_MAX_TIMEOUT
#define LIS302DL_MAX_TIMEOUT              4096
#endif // LIS302DL_MAX_TIMEOUT

/**
 * @brief This function is called whenever a timeout occure during communication.
 * 
 * This function is called whenever a timeout condition occure during communication
 * (waiting for transmit data register empty flag(TXE) or waitig for receive an empty
 * data register flag(RXNE)). The default implementation just stops all further communication
 * and proceeds an empty loop so the whole application stops working.
 * Typically a user implementation of this callback should reset the peripheral memory
 * and re-initialize communication or in worst case reset the complete application.
 * In case of a user implementation, just define LIS302DL_USE_CUSTOM_TIMEOUT_CALLBACK
 * and implement the function u32 LIS302DL_TIMEOUT_UserCallback(void).
 * 
 * @param void
 * @retval void
 */
uint32_t LIS302DL_TIMEOUT_UserCallback(void);
/* #define LIS302DL_USE_CUSTOM_TIMEOUT_CALLBACK */

/**** BEGIN: LIS302DL-Registers ****/

/**
 * @def LIS302DL_WHO_AM_I_ADDR
 * @brief Device Identification Register
 * 
 *  Read only register
 *  Default value: 0x3B
 * 
 *  This register contains the device identifier and is set to 0x3B for the LIS302DL
 */
#define LIS302DL_WHO_AM_I_ADDR             0x0F

/**
 * @def LIS302DL_CTRL_REG1_ADDR
 * @brief Control Register 1 from LIS302DL
 * 
 *  Read Write register
 *  Default value: 0x07 (0000 0111)
 * 
 *  7 DR: Data Rate selection
 *        0: 100 Hz output data rate (ODR) (default)
 *        1: 400 Hz output data rate (ODR)
 * 
 *  6 PD: Power Down control
 *        0: Power down mode (default)
 *        1: Active mode
 * 
 *  5 FS: Full Scale selection
 *        0: Typical measurement range ±2.3 (default)
 *        1: Typical measurement range ±9.2
 * 
 *  4 STP: Self Test power supply
 *        0: Disable (default)
 *        1: Enable 
 * 
 *  3 STM: Self Test values change in sign for all axis
 *        0: Disable (default)
 *        1: Enable
 * 
 *  2 Zen: Z axis enable
 *        0: Z axis disabled
 *        1: Z axis enabled (default)
 * 
 *  1 Yen: Y axis enable
 *        0: Y axis disabled
 *        1: Y axis enabled (default)
 * 
 *  0 Xen: X axis enable
 *        0: X axis disabled
 *        1: X axis enabled (default)
 */
#define LIS302DL_CTRL_REG1_ADDR            0x20

/**
 * @def LIS302DL_CTRL_REG2_ADDR
 * @brief Control Register 2
 * 
 *  Read Write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7 SIM: SPI Serial Interface Mode Selection
 *        0: 4 wire interface (default)
 *        1: 3 wire interface
 * 
 *  6 BOOT: Reboot memory content
 *        0: Normal mode (default)
 *        1: Reboot memory content
 * 
 *  5 --: Reserved
 * 
 *  4 FDS: Filtered data selection
 *        0: Internal filter bypassed (default)
 *        1: Data from internal filter sent to output register
 * 
 *  3 HP FF_WU2: High pass filter enabled for FreeFall/WakeUp#2
 *        0: Filter bypassed (default)
 *        1: Filter enabled
 * 
 *  2 HP FF_WU1: High pass filter enabled for FreeFall/WakeUp#1
 *        0: Filter bypassed (default)
 *        1: Filter enabled
 * 
 *  1-0 HP_coeff2,1: High pass filter cut-off frequency (ft) configuration
 *                 ft = ODR[hz] / 6 * HP coeff
 * 
 *         HP coeff2 | HP coeff1 |   HP coeff
 *        ------------------------------------
 *             0     |     0     |      8      (default)
 *             0     |     1     |     16
 *             1     |     0     |     32
 *             1     |     1     |     64
 * 
 *                    |  ft[hz]   |  ft[hz]   
 *        HP coeff2,1 | ODR 100Hz | ODR 400Hz  
 *       -------------+-----------+-----------
 *             00     |    2.00   |     8      (default)
 *             01     |    1.00   |     4     
 *             10     |    0.50   |     2     
 *             11     |    0.25   |     1     
 */
#define LIS302DL_CTRL_REG2_ADDR            0x21

/**
 * @def LIS302DL_CTRL_REG3_ADDR
 * @brief Interrupt Control Register
 * 
 *  Read Write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7 IHL: Interrupt active high/low
 *        0: Active high (default)
 *        1: Active low
 * 
 *  6 PP_OD: push-pull/open-drain
 *        0: Push-pull (default)
 *        1: Open-drain
 * 
 *  5-3 I2_CFG2 - I2_CFG0: Data signal on INT2 pad control bits
 *        000: See table below from I1CFGn (default)
 * 
 *  2-0 I1_CFG2 - I1_CFG0: Data signal on INT1 pad control bits
 *        I1(2)_CFG2  |  I1(2)_CFG1  |  I1(2)_CFG0  | INT1(2) Pad
 *        ------------+--------------+--------------+---------------
 *              0     |      0       |       0      | GND            (default)
 *              0     |      0       |       1      | FreeFall/WakeUp#1
 *              0     |      1       |       0      | FreeFall/WakeUp#2
 *              0     |      1       |       1      | FreeFall/WakeUp#1 or FreeFall/WakeUp#2
 *              1     |      0       |       0      | Data ready
 *              1     |      1       |       1      | Click interrupt
 */
#define LIS302DL_CTRL_REG3_ADDR            0x22

/**
 * @def LIS302DL_HP_FILTER_RESET_REG_ADDR
 * @brief Dummy register
 * 
 *  Read only register
 *  Default value: Dummy
 * 
 *  Reading at this address zeroes instantaneously the content of the internal
 *  high pass filter. If the high pass filter is enabled all three axes are
 *  instantaneously set to 0g.
 *  This allows to overcome the settling time of the high pass filter.
 *  Read only register.
 */
#define LIS302DL_HP_FILTER_RESET_REG_ADDR  0x23

/**
 * @def LIS302DL_STATUS_REG_ADDR
 * @brief Status Register
 * 
 *  Read only register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7 ZYXOR: X, Y and Z axis data overrun
 *        0: no overrun has occurred (default)
 *        1: new data has overwritten the previous one before it was read
 * 
 *  6 ZOR: Z axis data overrun
 *        0: no overrun has occurred (default)
 *        1: new data for Z-axis has overwritten the previous one before it was read
 * 
 *  5 yOR: y axis data overrun
 *        0: no overrun has occurred (default)
 *        1: new data for y-axis has overwritten the previous one before it was read
 * 
 *  4 XOR: X axis data overrun
 *        0: no overrun has occurred (default)
 *        1: new data for X-axis has overwritten the previous one before it was read
 * 
 *  3 ZYXDA: X, Y and Z axis new data available
 *        0: a new set of data is not yet available (default)
 *        1: a new set of data is available
 * 
 *  2 ZDA: Z axis new data available
 *        0: a new set of data is not yet available (default)
 *        1: a new data for Z axis is available
 * 
 *  1 YDA: Y axis new data available
 *        0: a new set of data is not yet available (default)
 *        1: a new data for Y axis is available
 * 
 *  0 XDA: X axis new data available
 *        0: a new set of data is not yet available (default)
 *        1: a new data for X axis is available
 */
#define LIS302DL_STATUS_REG_ADDR           0x27

/**
 * @def LIS302DL_OUT_X_ADDR
 * @brief X-axis output data register
 * 
 *  Read only register
 *  Default value: output
 * 
 *  7-0 XD7-XD0: X-axis output Data
 */
#define LIS302DL_OUT_X_ADDR                0x29

/**
 * @def LIS302DL_OUT_Y_ADDR
 * @brief Y-axis output data register
 * 
 *  Read only register
 *  Default value: output
 * 
 *  7-0 YD7-YD0: Y-axis output Data
 */
#define LIS302DL_OUT_Y_ADDR                0x2B

/**
 * @def LIS302DL_OUT_Z_ADDR
 * @brief Z-axis output data register
 * 
 *  Read only register
 *  Default value: output
 * 
 *  7-0 ZD7-ZD0: Z-axis output Data
 */
#define LIS302DL_OUT_Z_ADDR                0x2D

/**
 * @def LIS302DL_FF_WU_CFG1_REG_ADDR
 * @brief Configuration register for Interrupt 1 source
 * 
 *  Read write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7 AOI: AND or OR combination of Interrupt events
 *        0: OR combination of interrupt events (default)
 *        1: AND combination of interrupt events 
 * 
 *  6 LIR: Latch/not latch interrupt request
 *        0: Interrupt request not latched (default)
 *        1: Interrupt request latched
 * 
 *  5 ZHIE: Enable interrupt generation on «Z high event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value higher than preset threshold
 * 
 *  4 ZLIE: Enable interrupt generation on «Z low event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value lower than preset threshold
 * 
 *  3 YHIE: Enable interrupt generation on «Y high event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value higher than preset threshold
 * 
 *  2 YLIE: Enable interrupt generation on «Y low event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value lower than preset threshold
 * 
 *  1 XHIE: Enable interrupt generation on «X high event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value higher than preset threshold
 * 
 *  0 XLIE: Enable interrupt generation on «X low event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value lower than preset threshold
 */
#define LIS302DL_FF_WU_CFG1_REG_ADDR       0x30

/**
 * @def LIS302DL_FF_WU_SRC1_REG_ADDR
 * @brief Interrupt 1 source register (Free-fall and wake-up source register)
 * 
 *  Reading at this address clears FF_WU_SRC_1 register and the FF_WU_1 interrupt
 *  and allow the refreshment of data in the FF_WU_SRC_1 register if the latched option was chosen.
 *  Read only register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7 X: Reserved
 * 
 *  6 IA: Interrupt active
 *        0: no interrupt has been generated (default)
 *        1: one or more interrupts have been generated
 * 
 *  5 ZH: Z high
 *        0: no interrupt (default)
 *        1: ZH event has occurred 
 * 
 *  4 ZL: Z low
 *        0: no interrupt (default)
 *        1: ZL event has occurred
 * 
 *  3 YH: Y high
 *        0: no interrupt (default)
 *        1: YH event has occurred 
 * 
 *  2 YL: Y low
 *        0: no interrupt (default)
 *        1: YL event has occurred
 * 
 *  1 YH: X high
 *        0: no interrupt (default)
 *        1: XH event has occurred 
 * 
 *  0 YL: X low
 *        0: no interrupt (default)
 *        1: XL event has occurred
 */
#define LIS302DL_FF_WU_SRC1_REG_ADDR       0x31

/**
 * @def LIS302DL_FF_WU_THS1_REG_ADDR
 * @brief Threshold register
 * 
 *  Most significant bit (DCRM) is used to select the resetting mode of the duration counter.
 *  If DCRM is 0, the counter is resetted when the interrupt is no more active, else if DCRM is 1,
 *  the duration counter is decremented instead.
 *  Read Write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7 DCRM: Reset mode selection.
 *        0: counter resetted (default)
 *        1: counter decremented
 * 
 *  6 THS6-THS0: Free-fall/wake-up threshold value (7bit) (default: 000 000x)
 */
#define LIS302DL_FF_WU_THS1_REG_ADDR       0x32

/**
 * @def LIS302DL_FF_WU_DURATION1_REG_ADDR
 * @brief Duration Register for Free-Faöö / wake-up interrupt 1
 * 
 *  Duration step and maximum value depend on the ODR choosen. A step is 2.5 msec (from 0 to 637.5 msec)
 *  if ODR is 400Hz, else a step is 10 msec (from 0 to 2'550 msec) when ODR is 100Hz.
 *  The counter used to implement the duration fuction is blocked when LIR is set to 1
 *  in the configuration register and the interrupt event is verified.
 *  Read Write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7-0 D7-D0: Duration value
 */
#define LIS302DL_FF_WU_DURATION1_REG_ADDR  0x33

/**
 * @def LIS302DL_FF_WU_CFG2_REG_ADDR
 * @brief Configuration register for Interrupt 2 source
 * 
 *  Read write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7 AOI: AND or OR combination of Interrupt events
 *        0: OR combination of interrupt events (default)
 *        1: AND combination of interrupt events 
 * 
 *  6 LIR: Latch/not latch interrupt request
 *        0: Interrupt request not latched (default)
 *        1: Interrupt request latched
 * 
 *  5 ZHIE: Enable interrupt generation on «Z high event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value higher than preset threshold
 * 
 *  4 ZLIE: Enable interrupt generation on «Z low event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value lower than preset threshold
 * 
 *  3 YHIE: Enable interrupt generation on «Y high event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value higher than preset threshold
 * 
 *  2 YLIE: Enable interrupt generation on «Y low event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value lower than preset threshold
 * 
 *  1 XHIE: Enable interrupt generation on «X high event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value higher than preset threshold
 * 
 *  0 XLIE: Enable interrupt generation on «X low event»
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request on measured accel. value lower than preset threshold
 */
#define LIS302DL_FF_WU_CFG2_REG_ADDR       0x34

/**
 * @def LIS302DL_FF_WU_SRC2_REG_ADDR
 * @brief Interrupt 2 source register
 * 
 *  Reading at this address clears FF_WU_SRC_2 register and the FF_WU_2 interrupt
 *  and allow the refreshment of data in the FF_WU_SRC_2 register if the latched option was chosen.
 *  Read only register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7 X: Reserved
 * 
 *  6 IA: Interrupt active
 *        0: No interrupt has been generated (default)
 *        1: One or more interrupts have been generated
 * 
 *  5 ZH: Z high
 *        0: No interrupt (default)
 *        1: ZH event has occurred 
 * 
 *  4 ZL: Z low
 *        0: No interrupt (default)
 *        1: ZL event has occurred
 * 
 *  3 YH: Y high
 *        0: No interrupt (default)
 *        1: YH event has occurred 
 * 
 *  2 YL: Y low
 *        0: No interrupt (default)
 *        1: YL event has occurred
 * 
 *  1 YH: X high
 *        0: No interrupt (default)
 *        1: XH event has occurred 
 * 
 *  0 YL: X low
 *        0: No interrupt (default)
 *        1: XL event has occurred
 */
#define LIS302DL_FF_WU_SRC2_REG_ADDR       0x35

/**
 * @def LIS302DL_FF_WU_THS2_REG_ADDR
 * @brief Threshold register
 * 
 *  Most significant bit (DCRM) is used to select the resetting mode of the duration counter.
 *  If DCRM is 0, the counter is resetted when the interrupt is no more active, else if DCRM is 1,
 *  the duration counter is decremented instead.
 *  Read Write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7 DCRM: Reset mode selection.
 *        0: Counter resetted (default)
 *        1: Counter decremented
 * 
 *  6 THS6-THS0: Free-fall/wake-up threshold value (7bit) (default: 000 000x)
 */
#define LIS302DL_FF_WU_THS2_REG_ADDR       0x36

/**
 * @def LIS302DL_FF_WU_DURATION2_REG_ADDR
 * @brief Duration Register for Free-Faöö / wake-up interrupt 1
 * 
 *  Duration step and maximum value depend on the ODR choosen. A step is 2.5 msec (from 0 to 637.5 msec)
 *  if ODR is 400Hz, else a step is 10 msec (from 0 to 2'550 msec) when ODR is 100Hz.
 *  The counter used to implement the duration fuction is blocked when LIR is set to 1
 *  in the configuration register and the interrupt event is verified.
 *  Read Write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7-0 D7-D0: Duration value
 */
#define LIS302DL_FF_WU_DURATION2_REG_ADDR  0x37

/**
 * @def LIS302DL_CLICK_CFG_REG_ADDR
 * @brief Click Register
 *        Enables/Disables single and double click interrupts
 * 
 *  Read Write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7 --: Reserved
 * 
 *  6 LIR: Latch Interrupt request
 *        0: Interrupt request not latched (default)
 *        1: Interrupt request latched
 * 
 *  5 Double_Z: Enable interrupt generation on double click event on Z axis
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request
 * 
 *  4 Single_Z: Enable interrupt generation on single click event on Z axis
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request
 * 
 *  3 Double_Y: Enable interrupt generation on double click event on Y axis
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request
 * 
 *  2 Single_Y: Enable interrupt generation on single click event on Y axis
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request
 * 
 *  1 Double_X: Enable interrupt generation on double click event on X axis
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request
 * 
 *  0 Single_y: Enable interrupt generation on single click event on X axis
 *        0: Disable interrupt request (default)
 *        1: Enable interrupt request
 */
#define LIS302DL_CLICK_CFG_REG_ADDR        0x38

/**
 * @def LIS302DL_CLICK_SRC_REG_ADDR
 * @brief Click status Register
 *        This register holds the status of the click events when an interrupt has been generated
 * 
 *  Read only register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7 --: Reserved
 * 
 *  6 IA: Interrupt active
 *        0: No interrupt has been generated (default)
 *        1: one or more interrupts have been generated
 * 
 *  5 Double_Z: Double click on Z axis event
 *        0: No interrupt (default)
 *        1: Double click Z event has occurred 
 * 
 *  4 Single_Z: Single click in Z axis event
 *        0: No interrupt (default)
 *        1: Single click Z event has occurred 
 * 
 *  3 Double_Y: Double click on Y axis event
 *        0: No interrupt (default)
 *        1: Double click Y event has occurred 
 * 
 *  2 Single_Y: Single click in Y axis event
 *        0: No interrupt (default)
 *        1: Single click Y event has occurred 
 * 
 *  1 Double_X: Double click on X axis event
 *        0: No interrupt (default)
 *        1: Double click X event has occurred 
 * 
 *  0 Single_X: Single click in X axis event
 *        0: No interrupt (default)
 *        1: Single click X event has occurred 
 */
#define LIS302DL_CLICK_SRC_REG_ADDR        0x39

/**
 * @def LIS302DL_CLICK_THSY_X_REG_ADDR
 * @brief Click threshold Y and X register
 *        Configuration for the threashold for the click events on X and Y axis. Defined from 0.0g up to 7.5g with steps of 0.5g.
 *        A threashod of 0.5g is therefore defined as 0001 and 7.5g as 1111.
 * 
 *  Read Write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7-4 THSy3-THSy0: Click threshold on Y axis
 * 
 *  3-0 THSx3-THSx0: Click threshold on X axis
 */
#define LIS302DL_CLICK_THSY_X_REG_ADDR     0x3B

/**
 * @def LIS302DL_CLICK_THSZ_REG_ADDR
 * @brief Click threshold Z register
 *        Configuration for the threashold for the click events on Z axis. Defined from 0.0g up to 7.5g with steps of 0.5g.
 *        A threashod of 0.5g is therefore defined as 0001 and 7.5g as 1111.
 * 
 *  Read Write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7-4 --: Reserved
 * 
 *  3-0 THSz3-THSz0: Click threshold on Z axis
 */
#define LIS302DL_CLICK_THSZ_REG_ADDR       0x3C

/**
 * @def LIS302DL_CLICK_TIMELIMIT_REG_ADDR
 * @brief Time Limit register
 *        Time limit in which a click/doubleclick must occur defined from 0 msec up to 127.5 msec in steps of 0.5 msec.
 *        A timelimit of 0.5 msec is therefore defined 00000001 and 127.5 msec as 11111111
 * 
 *  Read Write register
 *  Default value: 0x00 (0000 0000)
 * 
 *  7-0 Dur7-Dur0: Time Limit value
 */
#define LIS302DL_CLICK_TIMELIMIT_REG_ADDR  0x3D

/**
 * @def LIS302DL_CLICK_LATENCY_REG_ADDR
 * @brief Latency register
 *        Click latency from 0 to 255 msec with steps of 1 msec.
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7-0 Lat7-Lat0: Latency value, step 1msec
 */
#define LIS302DL_CLICK_LATENCY_REG_ADDR    0x3E

/**
 * @def LIS302DL_CLICK_WINDOW_REG_ADDR
 * @brief Window register
 *        Click window from 0 to 255 msec with steps of 1 msec.
 * 
 *  Read Write register
 *  Default value: 0x00
 * 
 *  7-0 Win7-Win0: Window value, step 1msec
 */
#define LIS302DL_CLICK_WINDOW_REG_ADDR     0x3F

/**** END: LIS302DL-Registers ****/

/**
 * @def LIS302DL_DUMMY_BYTE
 * @brief A Dummy Byte to send to generate a clock.
 * 
 *  Sent by the SPI Master device in order to generate the clock to the slave device
 */
#define LIS302DL_DUMMY_BYTE                ((uint8_t)0x00)

/**
 * @def LIS302DL_BIT0
 * @brief Bit at position 0 from left side
 */
#define LIS302DL_BIT0                     0x80

/**
 * @def LIS302DL_BIT1
 * @brief Bit at position 1 from left side
 */
#define LIS302DL_BIT1                     0x40

/**
 * @def LIS302DL_BIT2
 * @brief Bit at position 2 from left side
 */
#define LIS302DL_BIT2                     0x20

/**
 * @def LIS302DL_BIT3
 * @brief Bit at position 3 from left side
 */
#define LIS302DL_BIT3                     0x10

/**
 * @def LIS302DL_BIT4
 * @brief Bit at position 4 from left side
 */
#define LIS302DL_BIT4                     0x08

/**
 * @def LIS302DL_BIT5
 * @brief Bit at position 5 from left side
 */
#define LIS302DL_BIT5                     0x04

/**
 * @def LIS302DL_BIT6
 * @brief Bit at position 6 from left side
 */
#define LIS302DL_BIT6                     0x02

/**
 * @def LIS302DL_BIT7
 * @brief Bit at position 7 from left side
 */
#define LIS302DL_BIT7                     0x01

/**
 * @typedef LIS302DL_Config
 * @brief Used for the configuration of a LIS302DL sensor (see CTRL_REG1)
 * 
 *  Use this to read and write the configuration on a LIS302DL sensor.
 */
typedef struct {
	uint8_t DataRate : 1;       ///< Data rate selection (0: 100Hz, 1: 400Hz)
	uint8_t PowerDown : 1;      ///< Power mode selection (0: power down mode, 1: active mode)
	uint8_t FullScale : 1;      ///< Measurement rate selection (0: Measurement rate to 2.3, 1: Measurement rate to 9.2)
	uint8_t SelfTest_P : 1;     ///< SelfTest P-Mode (0: SelfTest disabled, 1: Various output values on all axis)
	uint8_t SelfTest_M : 1;     ///< SelfTest M-Mode (0: SelfTest disabled, 1: Invert the sign on all axis values)
	uint8_t ZAxisEnabled : 1;   ///< Z-Axis (0: Disabled, 1: Enabled)
	uint8_t YAxisEnabled : 1;   ///< Y-Axis (0: Disabled, 1: Enabled)
	uint8_t XAxisEnabled : 1;   ///< X-Axis (0: Disabled, 1: Enabled)
} LIS302DL_Config;




/**** Macros ****/
// TODO: Create functions from these to be able to use more that only one sensor on different SPI ports
#define LIS302DL_CS_LOW()       GPIO_ResetBits(LIS302DL_SPI_CS_GPIO_PORT, LIS302DL_SPI_CS_PIN)
#define LIS302DL_CS_HIGH()      GPIO_SetBits(LIS302DL_SPI_CS_GPIO_PORT, LIS302DL_SPI_CS_PIN)

/**
 * @brief  Write the configuration to the LIS302DL device
 * @param  spi  Pointer to the SPI on which the data should be sent/set/received
 * @param  pConfig  Pointer to the configuration to write
 * @retval void
 */
void LIS302DL_Init(SPI_TypeDef* spi, const LIS302DL_Config* config);

/**
 * @brief  Read out the configuration form a LIS302DL
 * @param  spi  Pointer to the SPI on which the data should be sent/set/received
 * @param  pConfig  Pointer to write the configuration to
 * @retval void
 */
void LIS302DL_GetConfiguration(SPI_TypeDef* spi, volatile LIS302DL_Config* config);

/**
 * @brief  Read data from a LIS302DL.
 * @param  spi  Pointer to the SPI on which the data should be sent/set/received
 * @param  pBuffer  Pointer to the buffer for the received data
 * @param  readAddr  LIS302DL Internal address from the register to read from
 * @param  numByteToRead  Number of bytes to read from the LIS302DL
 * @retval void
 */
void LIS302DL_Read(SPI_TypeDef* spi, uint8_t* pBuffer, uint8_t readAddr, uint16_t numByteToRead);

/**
 * @brief  Writes one byte to the LIS302DL.
 * @param  spi  Pointer to the SPI on which the data should be sent/set/received
 * @param  pBuffer  Pointer to the buffer  containing the data to be written to the LIS302DL.
 * @param  writeAddr  LIS302DL's internal address to write to.
 * @param  NumByteToWrite  Number of bytes to write.
 * @retval void
 */
void LIS302DL_Write(SPI_TypeDef* spi, uint8_t* pBuffer, uint8_t writeAddr, uint16_t numByteToWrite);

/**
 * @brief  Read the LIS302DL output register and calculate the acceleration based like:
 *         ACC[mg] = SENSITIVITY * (out_h * 256 + out_l) / 16 (12 bit rappresentation)
 * @param  spi  Pointer to the SPI on which the data should be sent/set/received
 * @param out  Pointer to the buffer to store the received data
 * @retval void
 */
void LIS302DL_Acceleration(SPI_TypeDef* spi, int32_t* out);

/**
 * @brief  Rebot the memory content of a LIS302DL device
 *         This is done by setting BOOT in CTRL_REG2
 * @param  spi  Pointer to the SPI on which the data should be sent/set/received
 * @retval None
 */
void LIS302DL_Reboot(SPI_TypeDef* spi);

/**
 * @brief  Change the sensor scale to full/halve scale
 *         In full scale the measurement rate is ±2.3g (default), otherwise it is ±9.2g
 * @param  spi  Pointer to the SPI on which the data should be sent/set/received
 * @param  fullScaleEnable Set to true to change to ±2.3g mode or false if you want ±9.2g
 * @retval None
 */
void LIS302DL_ChangeScaleMode(SPI_TypeDef* spi, uint8_t fullScaleEnable);

/**
 * @brief  Change the sensor data rate to 100Hz or 400Hz
 *         For full speed 400Hz (defalut), set enableHighSpeed to true, otherwise to false
 * @param  spi  Pointer to the SPI on which the data should be sent/set/received
 * @param  enableHighSpeed Enable for 400Hz, disable for 100Hz data rate
 * @retval None
 */
void LIS302D_ChangeDataRate(SPI_TypeDef* spi, uint8_t enableHighSpeed);

/**
 * @brief  Change the sensors PowerDown control
 * @param  spi  Pointer to the SPI on which the data should be sent/set/received
 * @param  enableActiveMode Set to true to enable the sensors active mode, else the sensor is in power down control
 * @retval None
 */
void LIS302D_ChangePowerControl(SPI_TypeDef* spi, uint8_t enableActiveMode);





#endif // LIS302DL_H