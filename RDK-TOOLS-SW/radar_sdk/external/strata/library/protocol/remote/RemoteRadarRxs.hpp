/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "RemotePinsRxs.hpp"
#include "RemoteRadar.hpp"
#include "RemoteRegisters.hpp"
#include "RemoteVendorCommands.hpp"
#include <components/radar/rxs/RadarRxsFirmwareCalls.hpp>
#include <platform/interfaces/IVendorCommands.hpp>

class RemoteRadarRxs :
    public RadarRxsFirmwareCalls,
    public RemoteRadar
{
public:
    RemoteRadarRxs(IVendorCommands *commands, uint8_t id);

    //IRadar
    void reset(bool softReset) override;

    //IRadarRxs
    IRegisters<uint16_t> *getIRegisters() override;
    IPinsRxs *getIPinsRxs() override;

    double getInternalSamplingRate();

    void setDefaults() override;
    bool getStatus() override;
    void configureDmuxMap(const uint8_t map[I_PINS_RXS_DMUX_COUNT]) override;
    void enableDividerOutput(bool enable) override;
    void enableDataCrc(bool enable) override;
    void enableTxRx() override;
    void setTriggerSource(uint8_t src) override;


    void startFirmwareFunction(uint16_t callcode, const uint16_t params[], uint16_t paramCount) override;
    void checkFirmwareFunctionStatus(uint16_t &status, uint16_t &count) override;
    void getFirmwareFunctionResult(uint16_t retVals[], uint16_t retCount) override;

    void Get_firmware_version(uint8_t &chipVersion, uint32_t &fwRevision) override;
    float Measure_temperature() override;
    void Enable_TX(bool TX1_enable, bool TX2_enable, bool TX3_enable, uint8_t operationMode) override;
    uint16_t Calibration(int32_t nf_min, int32_t nf_calib, uint8_t tx_phase_cal_iqm_mask1, uint8_t tx_phase_cal_iqm_mask2, uint8_t ref_sel, bool cal_ref, uint8_t tx_iqm_verif, uint16_t run_flags) override;
    void Start_rmp_scenario(uint16_t startAddress, uint8_t triggerSource, uint8_t monitoring) override;
    void Set_phase(uint8_t config_index, uint16_t phase_angle) override;

    void setChargePumpCurrentA(uint16_t icpA) override;
    void setChargePumpCurrentC(uint16_t icpC) override;
    void setChargePumpOffset(uint16_t mode) override;
    void calculateParameterCrc() override;

    void Set_LVDS_frame_delay(double &rise, double &fall) override;
    void Set_TX_power(uint8_t TX1_DAC_value, uint8_t TX2_DAC_value, uint8_t TX3_DAC_value) override;
    void Enable_RX(bool RX1_enable, bool RX2_enable, bool RX3_enable, bool RX4_enable, uint8_t LP_gain, uint8_t Mixer_gain, uint8_t DCOC_shift, uint8_t DCOC_enable, uint8_t RX_settings) override;
    void LVDS_calib_start() override;
    void LVDS_calib_stop() override;
    void LVDS_calib_fake() override;
    void Static_frequency(int32_t nf) override;
    void Goto_low_power() override;
    void Set_clk_out(bool Enable_25_Clk_Out, bool Enable_50_Clk_Out) override;

private:
    RemoteVendorCommands m_vendorCommands;

    RemoteRegisters<uint16_t> m_registers;
    RemotePinsRxs m_pinsRadar;
};
