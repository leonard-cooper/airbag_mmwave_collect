/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "protocol_definitions.h"

enum PsHeaderMode
{
  PsHeaderMode_HeaderWithPayload = 0x00,
  PsHeaderMode_HeaderOnly = 0x01,
  PsHeaderMode_PayloadOnly = 0x02
};

enum CtrxDmuxPins
{
  CtrxDmuxPins_DMUX1 = 1,
  CtrxDmuxPins_DMUX2,
  CtrxDmuxPins_DMUX3
};

enum
{
  CtrxSystemInfo_Length = 0U,
  CtrxSystemInfo_String = 1U
};

/* write - configureReceiver
        wLength: 6 byte
        payload:
            - 8bit use LVDS receiver (true=1, false=0)
            - 8bit header options (0x0...send data with header, 0x1...send only header no data, 0x2...send only data no header)
            - 16bit sequence count
            - 16bit number of ramps
    */
#define CMD_CTRX_CONFIGURE_RECEIVER    0x01

/* write - Activate JTAG
        wIndex: Mode
            - 0: Normal (no JTAG)
            - 1: Full JTAG via SPI (async)
            - 2: Full JTAG via SPI (sync)
            - 3: cJTAG and SPI
	        - 4: cJTAG via SPI
	        - 5: cJTAG via DMUX1&2
	        - 6: cJTAG via DMUX2&3
	        - 7: cJTAG via DMUX1&3
        wLength: 16
        payload: JTAG activation sequence
    */
#define CMD_CTRX_ACTIVATE_JTAG         0x02

/* write - Enable JTAG test mode to be able to access the Test Register Chain (TRX) via JTAG
        wIndex: 0
        wLength: 16
        payload: The password needed to activate the test mode
*/
#define CMD_CTRX_ENABLE_JTAG_TESTMODE  0x03

/* write - Write memory using the JTAG interface
        wIndex: 0
        wLength: 4 + (4 * number of values to write)
        Payload: Address (32bit) and values (each 32bit)
*/
#define CMD_CTRX_WRITE_MEMORY_VIA_JTAG 0x04

/* transfer - Read memory using the JTAG interface
        Must be a different ID than write since the firmware cannot distinguish between
        write and transfer calls
        wIndex: 0
        wLength In: 4 + 4
        payload In: Address (32bit) and number of values to read (32bit)
        wLength Out: 4 * number of values read
        payload Out: read values (each 32bit)
*/
#define CMD_CTRX_READ_MEMORY_VIA_JTAG  0x05

/* read - System Info
           Retrieves information on the setup of the board, including versions of the different components.
           The information is to be provided as JSON string.
        wIndex:
            0 (CtrxSystemInfo_Length) = retrieve the length of the string, wLength = 2 then
            1 (CtrxSystemInfo_String) = retrieve the string, without \0, wLength is what was retrieved before via wIndex=0
   */
#define CMD_CTRX_SYSTEM_INFO 0x06
