/****************************************************************************\
* Copyright (C) 2019 Infineon Technologies
*
* THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
\****************************************************************************/
#pragma once

#include "protocol_definitions.h"

enum ResetDevices
{
  ResetDevices_DphyController = 1U
};

enum
{
  MiraSetupInfo_Length = 0U,
  MiraSetupInfo_String = 1U
};

/* 0x00 was previously used for CMD_MIRA_SETUP and should be treated as reserved */

/* write - setCaptureState
        wLength: 4
        payload: - 8bit supress super frames (true=1, false=0)
                 - 16bit frame count
                 - 8bit send only headers (true=1, false=0)
   read  - (not implemented)
   */
#define CMD_MIRA_CAPTURE_STATE 0x01

/* 0x02 was previously used for CMD_MIRA_PREPARE_NEXT_FRAME and should be treated as reserved */

/* write - reset
           Performs reset of the device specified with wIndex.

        wLength: 0
        wIndex: Device to reset
            0 = reserved
            1 = DPHY controller
   read  - (not implemented)
   */
#define CMD_MIRA_RESET 0x03

/* write - Benchmark
           Starts benchmark mode, i.e. sends frame count frames of width x height of random data.
           Does not depend on an imager.

        wIndex: 0
        wLength: 6
        payload:  - 16bit frame count
                  - 16bit frame width
                  - 16bit frame height
   read  - (not implemented)
   */
#define CMD_MIRA_BENCHMARK 0x04

/* read - Setup Info
           Retrieves information on the setup of the board, including versions of the different components.
           The information is to be provided as JSON string.

        wIndex:
            0 (MiraSetupInfo_Length) = retrieve the length of the string, wLength = 2 then
            1 (MiraSetupInfo_String) = retrieve the string, without \0, wLength is what was retrieved before via wIndex=0
   */
#define CMD_MIRA_SETUP_INFO 0x05
