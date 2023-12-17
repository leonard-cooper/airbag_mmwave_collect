/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "IPinsRxs.hpp"
#include "IRadar.hpp"
#include "IRegisters.hpp"
#include <universal/types/Types_IRadarRxs.h>

#include <cstdint>
#include <vector>


class IRadarRxs :
    virtual public IRadar
{
public:
    static constexpr uint8_t getImplementation()
    {
        return COMPONENT_IMPL_RADAR_RXS;
    }

    virtual ~IRadarRxs() = default;

    virtual IPinsRxs *getIPinsRxs()               = 0;
    virtual IRegisters<uint16_t> *getIRegisters() = 0;

    virtual void setDefaults()                                              = 0;
    virtual bool getStatus()                                                = 0;
    virtual void configureDmuxMap(const uint8_t map[I_PINS_RXS_DMUX_COUNT]) = 0;
    virtual void enableDividerOutput(bool enable)                           = 0;
    virtual void enableDataCrc(bool enable)                                 = 0;
    virtual void setTriggerSource(uint8_t src)                              = 0;
    virtual void enableTxRx()                                               = 0;

    virtual void startFirmwareFunction(uint16_t callcode, const uint16_t params[] = nullptr, uint16_t paramCount = 0) = 0;
    virtual void checkFirmwareFunctionStatus(uint16_t &status, uint16_t &count)                                       = 0;
    virtual void getFirmwareFunctionResult(uint16_t retVals[], uint16_t retCount)                                     = 0;

    virtual uint16_t executeFirmwareFunction(uint16_t callcode, const uint16_t params[] = nullptr, uint16_t paramCount = 0, uint16_t retVals[] = nullptr, uint16_t maxRetCount = 0, uint16_t *retCount = nullptr) = 0;
    virtual uint16_t executeFirmwareFunction(uint16_t callcode, const std::vector<uint16_t> &params, std::vector<uint16_t> *retVals = nullptr)                                                                    = 0;


    virtual void Get_firmware_version(uint8_t &chipVersion, uint32_t &fwRevision)                                                                                                                           = 0;
    virtual float Measure_temperature()                                                                                                                                                                     = 0;
    virtual void Set_LVDS_frame_delay(double &rise, double &fall)                                                                                                                                           = 0;
    virtual void Set_TX_power(uint8_t TX1_DAC_value, uint8_t TX2_DAC_value, uint8_t TX3_DAC_value)                                                                                                          = 0;
    virtual void Enable_RX(bool RX1_enable, bool RX2_enable, bool RX3_enable, bool RX4_enable, uint8_t LP_gain, uint8_t HP_gain, uint8_t DCOC_shift, uint8_t DCOC_enable, uint8_t RX_settings)              = 0;
    virtual void Enable_TX(bool TX1_enable, bool TX2_enable, bool TX3_enable, uint8_t operationMode)                                                                                                        = 0;
    virtual void LVDS_calib_start()                                                                                                                                                                         = 0;
    virtual void LVDS_calib_stop()                                                                                                                                                                          = 0;
    virtual void LVDS_calib_fake()                                                                                                                                                                          = 0;
    virtual uint16_t Calibration(int32_t nf_min, int32_t nf_calib, uint8_t tx_phase_cal_iqm_mask1, uint8_t tx_phase_cal_iqm_mask2, uint8_t ref_sel, bool cal_ref, uint8_t tx_iqm_verif, uint16_t run_flags) = 0;
    virtual void Static_frequency(int32_t nf)                                                                                                                                                               = 0;
    virtual void Start_rmp_scenario(uint16_t startAddress, uint8_t triggerSource, uint8_t monitoring)                                                                                                       = 0;
    virtual void Goto_low_power()                                                                                                                                                                           = 0;
    virtual void Set_clk_out(bool Enable_25_Clk_Out, bool Enable_50_Clk_Out)                                                                                                                                = 0;
    virtual void Set_phase(uint8_t config_index, uint16_t phase_angle)                                                                                                                                      = 0;
    virtual void setChargePumpCurrentA(uint16_t icpA)                                                                                                                                                       = 0;
    virtual void setChargePumpCurrentC(uint16_t icpC)                                                                                                                                                       = 0;
    virtual void setChargePumpOffset(uint16_t mode)                                                                                                                                                         = 0;
    virtual void calculateParameterCrc()                                                                                                                                                                    = 0;
};
