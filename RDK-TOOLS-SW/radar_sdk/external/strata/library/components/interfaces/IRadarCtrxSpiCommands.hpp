/**
 * @copyright 2021 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/interfaces/IRadarCtrxSpiStructs.hpp>

/**
 * @brief This class provides SPI commands for the CTRX component
 */
class ISpiCommandsCtrx
{
public:
    /**
     * @brief Directly execute an arbitrary firmware command. The command is sent and then the response is read.
     * @param commandId The ID of the firmware command to execute
     * @param params An array containing the firmware command parameters. Please note that the first parameter only has 16bit. If the value is too big, an exception will be thrown.
     * @param paramsCount Number of firmware command parameters in the params array
     * @param reserved Optional value to set the Reserved byte, otherwise it will be 0
     * @param readCount The number of words to read. This number has to include the header and the trailing CRC32, if used.
     *                  The readCount parameter will be ignored if the value is <0 and the amount of words to read will be set automatically.
     * @return A structure containing information regarding the received response
     */
    virtual CtrxSpiStructs::ExecuteDirectlyFwCmdResult Execute_Directly_FW_CMD(uint8_t commandId, const uint32_t params[], uint8_t paramsCount, uint8_t reserved = 0, int32_t readCount = -1) = 0;

    /**
     * @brief abort the sequencer
     * @note If Finish_Ramp_scenario command is running the sequencer is aborted and Finish_Ramp_Scenario response is returned
     * @param param0 ignored (don't care)
     * @param readCount The number of words to read. This number has to include the header and the trailing CRC32, if used.
     *                  The readCount parameter will be ignored if the value is <0 and the amount of words to read will be set automatically.
     * @return A structure containing information regarding the received response
     */
    virtual CtrxSpiStructs::AbortRampScenarioResult Abort_Ramp_Scenario(uint8_t param0 = 0, int32_t readCount = -1) = 0;

    /**
     * @brief Validate and store the set of parameters (command configuration).
     * @param handleOrCommandId Handle if configuration should be updated, Command ID if new configuration should be created
     * @param actionType 0 - Update the existing command configuration, 1 - Create new command configuration.
     * @param params An array containing the firmware command parameters. Please note that the first parameter only has 16bit. If the value is too big, an exception will be thrown.
     * @param paramsCount Number of firmware command parameters in the params array
     * @param readCount The number of words to read. This number has to include the header and the trailing CRC32, if used.
     *                  The readCount parameter will be ignored if the value is <0 and the amount of words to read will be set automatically.
     * @return A structure containing information regarding the received response
     */
    virtual CtrxSpiStructs::ConfigureHandleResult Configure_Handle(uint8_t handleOrCommandId, uint8_t actionType, const uint32_t params[], uint8_t paramsCount, int32_t readCount = -1) = 0;

    /**
     * @brief Reset the firmware to a defined state.
     * @param resetType Optional Type of Reset, otherwise a Application Reset 0xAE is triggered
     * @param readCount The number of words to read. This number has to include the header and the trailing CRC32, if used.
     *                  The readCount parameter will be ignored if the value is <0 and the amount of words to read will be set automatically.
     * @return A structure containing information regarding the received response
     */
    virtual CtrxSpiStructs::TriggerResetResult Trigger_Reset(uint8_t resetType = 0xAE, int32_t readCount = -1) = 0;

    /**
     * @brief Execute a firmware command using the already stored configuration as specified by the handle
     * @param handle Handle of the stored command configuration to be executed
     * @param readCount The number of words to read. This number has to include the header and the trailing CRC32, if used.
     *                  The readCount parameter will be ignored if the value is <0 and the amount of words to read will be set automatically.
     * @return A structure containing information regarding the received response
     */
    virtual CtrxSpiStructs::ExecuteHandleResult Execute_Handle(uint8_t handle, int32_t readCount = -1) = 0;

    /**
     * @brief Perform Configure_Handle and, if successful, use the Handle to perform Execute_Handle
     * @param handleOrCommandId Handle if configuration should be updated, Command ID if new configuration should be created
     * @param actionType 0 - Update the existing command configuration, 1 - Create new command configuration.
     * @param params An array containing the firmware command parameters. Please note that the first parameter only has 16bit. If the value is too big, an exception will be thrown.
     * @param paramsCount Number of firmware command parameters in the params array
     * @param readCount The number of words to read. This number has to include the header and the trailing CRC32, if used.
     *                  The readCount parameter will be ignored if the value is <0 and the amount of words to read will be set automatically.
     * @return A structure containing information regarding the received response
     */
    virtual CtrxSpiStructs::ExecuteHandleResult Configure_And_Execute_Handle(uint8_t handleOrCommandId, uint8_t actionType, const uint32_t params[], uint8_t paramsCount, int32_t readCount = -1) = 0;

    /**
     * @brief Send a Do_NOP command to retrieve the last response
     * @param param0 ignored (don't care)
     * @param readCount The number of words to read. This number has to include the header and the trailing CRC32, if used.
     *                  The readCount parameter will be ignored if the value is <0 and the amount of words to read will be set automatically.
     * @return A structure containing information regarding the received response.
     *         Note that the single elements will only be filled if the received SPI message is valid. Otherwise only the raw data member is filled.
     */
    virtual CtrxSpiStructs::NopResult Do_NOP(uint8_t param0 = 0, int32_t readCount = -1) = 0;
};
