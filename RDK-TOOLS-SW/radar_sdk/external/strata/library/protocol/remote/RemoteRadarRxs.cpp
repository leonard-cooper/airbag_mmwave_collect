/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteRadarRxs.hpp"

#include <common/Buffer.hpp>
#include <common/Serialization.hpp>
#include <common/exception/ENotImplemented.hpp>
#include <components/exception/ERadar.hpp>
#include <platform/exception/EProtocol.hpp>
#include <universal/components/implementations/radar/iradarrxs.h>
#include <universal/components/subinterfaces.h>
#include <universal/protocol/protocol_definitions.h>


RemoteRadarRxs::RemoteRadarRxs(IVendorCommands *commands, uint8_t id) :
    RadarRxsFirmwareCalls(),
    RemoteRadar(commands, id),
    m_vendorCommands {commands, CMD_COMPONENT, getType(), getImplementation(), id, SUBIF_DEFAULT},
    m_registers(2, commands, getType(), getImplementation(), id),
    m_pinsRadar(commands, id)
{
}

void RemoteRadarRxs::reset(bool softReset)
{
    if (softReset)
    {
        RemoteRadar::reset(softReset);
    }
    else
    {
        //special treatment for RXS, use pins interface to do the reset
        m_pinsRadar.reset();
    }
}

IRegisters<uint16_t> *RemoteRadarRxs::getIRegisters()
{
    return &m_registers;
}

IPinsRxs *RemoteRadarRxs::getIPinsRxs()
{
    return &m_pinsRadar;
}

double RemoteRadarRxs::getInternalSamplingRate()
{
    throw ENotImplemented();
}

void RemoteRadarRxs::setDefaults()
{
    throw ENotImplemented();
}

bool RemoteRadarRxs::getStatus()
{
    uint8_t state;
    m_vendorCommands.vendorRead(FN_RADAR_RXS_GET_STATUS, sizeof(state), &state);
    return state;
}
void RemoteRadarRxs::configureDmuxMap(const uint8_t map[I_PINS_RXS_DMUX_COUNT])
{
    m_vendorCommands.vendorWrite(FN_RADAR_RXS_CONFIGURE_DMUX_MAP, I_PINS_RXS_DMUX_COUNT, map);
}

void RemoteRadarRxs::enableDividerOutput(bool enable)
{
    const uint8_t value = enable ? 1 : 0;
    m_vendorCommands.vendorWrite(FN_RADAR_RXS_SET_DIV_OUTPUT, sizeof(value), &value);
}

void RemoteRadarRxs::enableDataCrc(bool enable)
{
    const uint8_t value = enable ? 1 : 0;
    m_vendorCommands.vendorWrite(FN_RADAR_RXS_SET_DATA_CRC, sizeof(value), &value);
}

void RemoteRadarRxs::enableTxRx()
{
    throw ENotImplemented();
}

void RemoteRadarRxs::setTriggerSource(uint8_t src)
{
    m_vendorCommands.vendorWrite(FN_RADAR_RXS_SET_TRIGGER_SRC, sizeof(src), &src);
}

void RemoteRadarRxs::startFirmwareFunction(uint16_t callcode, const uint16_t params[], uint16_t paramCount)
{
    constexpr uint16_t argSize = sizeof(callcode);
    const uint16_t dataSize    = paramCount * sizeof(params[0]);
    const uint16_t wLength     = argSize + dataSize;
    stdext::buffer<uint8_t> payload(wLength);
    const uint8_t *data = reinterpret_cast<const uint8_t *>(params);
    uint8_t *it         = payload.data();
    it                  = hostToSerial(it, callcode);
    it                  = std::copy(data, data + dataSize, it);
    m_vendorCommands.vendorWrite(FN_RADAR_RXS_START_FW_FUNCTION, wLength, payload.data());
}

void RemoteRadarRxs::checkFirmwareFunctionStatus(uint16_t &status, uint16_t &count)
{
    constexpr uint16_t argSize = sizeof(status) + sizeof(count);
    uint8_t buf[argSize];
    m_vendorCommands.vendorRead(FN_RADAR_RXS_CHECK_FW_FUNCTION_STATUS, argSize, buf);

    const uint8_t *it = buf;
    it                = serialToHost(it, status);
    it                = serialToHost(it, count);
}

void RemoteRadarRxs::getFirmwareFunctionResult(uint16_t retVals[], uint16_t retCount)
{
    m_vendorCommands.vendorRead(FN_RADAR_RXS_GET_FW_FUNCTION_RESULT, retCount * sizeof(retVals[0]), reinterpret_cast<uint8_t *>(retVals));
}

void RemoteRadarRxs::Get_firmware_version(uint8_t &, uint32_t &)
{
    throw ENotImplemented();
}

float RemoteRadarRxs::Measure_temperature()
{
    throw ENotImplemented();
}

void RemoteRadarRxs::Enable_TX(bool, bool, bool, uint8_t)
{
    throw ENotImplemented();
}

uint16_t RemoteRadarRxs::Calibration(int32_t, int32_t, uint8_t, uint8_t, uint8_t, bool, uint8_t, uint16_t)
{
    throw ENotImplemented();
}

void RemoteRadarRxs::Start_rmp_scenario(uint16_t, uint8_t, uint8_t)
{
    throw ENotImplemented();
}

void RemoteRadarRxs::Set_phase(uint8_t, uint16_t)
{
    throw ENotImplemented();
}

void RemoteRadarRxs::setChargePumpCurrentA(uint16_t)
{
    throw ENotImplemented();
}

void RemoteRadarRxs::setChargePumpCurrentC(uint16_t)
{
    throw ENotImplemented();
}

void RemoteRadarRxs::setChargePumpOffset(uint16_t)
{
    throw ENotImplemented();
}

void RemoteRadarRxs::calculateParameterCrc()
{
    throw ENotImplemented();
}

void RemoteRadarRxs::Set_LVDS_frame_delay(double &, double &)
{
    throw ENotImplemented();
}

void RemoteRadarRxs::Set_TX_power(uint8_t, uint8_t, uint8_t)
{
    throw ENotImplemented();
}

void RemoteRadarRxs::Enable_RX(bool, bool, bool, bool, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)
{
    throw ENotImplemented();
}

void RemoteRadarRxs::LVDS_calib_start()
{
    throw ENotImplemented();
}

void RemoteRadarRxs::LVDS_calib_stop()
{
    throw ENotImplemented();
}

void RemoteRadarRxs::LVDS_calib_fake()
{
    throw ENotImplemented();
}

void RemoteRadarRxs::Static_frequency(int32_t)
{
    throw ENotImplemented();
}

void RemoteRadarRxs::Goto_low_power()
{
    throw ENotImplemented();
}

void RemoteRadarRxs::Set_clk_out(bool, bool)
{
    throw ENotImplemented();
}
