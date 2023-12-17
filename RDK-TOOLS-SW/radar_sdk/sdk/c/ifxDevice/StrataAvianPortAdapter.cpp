/* ===========================================================================
** Copyright (C) 2021 Infineon Technologies AG
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

// ---------------------------------------------------------------------------- includes
#include "StrataAvianPortAdapter.hpp"
#include <platform/BoardManager.hpp>
#include <platform/exception/EProtocolFunction.hpp>
#include <universal/error_definitions.h>
#include <universal/types/DataAvianSettings.h>
#include <common/NarrowCast.hpp>
#include <thread>


// ---------------------------------------------------------------------------- constants
#define PROCESSED_DATA_CHANNEL      (0x02)

// ---------------------------------------------------------------------------- namespaces
namespace Infineon
{
namespace Communication
{

// ---------------------------------------------------------------------------- StrataAvianPortAdapter
StrataAvianPortAdapter::StrataAvianPortAdapter(BoardInstance *board):
    m_bridgeData{ board->getIBridge()->getIBridgeData() }
{
    // for now, we only support MCU7 with these properties.
    // refactoring needed if more boards are supported:
    // either lookup of properties here, or better add a universal interafce to ISpi to query there parameters universally
    m_properties = {
        "Avian",
        false,
        2,
    };

    IRadarAvian *avian;
    try
    {
        m_id = 0;
        avian = board->getComponent<IRadarAvian>(m_id);
        // after getting the remote instance, we need to call one function to see if it actually exists or not
        m_dataIndex = avian->getDataIndex();
    }
    catch (EProtocolFunction &e)
    {
        if (e.code() == STATUS_COMMAND_ID_INVALID)
        {
            m_id = 1;
            avian = board->getComponent<IRadarAvian>(m_id);
            // after getting the remote instance, we need to call one function to see if it actually exists or not
            m_dataIndex = avian->getDataIndex();
        }
        else
        {
            throw;
        }
    }

    m_data = board->getIBridge()->getIBridgeControl()->getIData();
    m_cmd = avian->getICommandsAvian();
    m_pins = avian->getIPinsAvian();
    m_bridgeData->registerListener(this);

    // stop any ongoing device measurement and clear fifo
    try
    {
        m_cmd->setBits(ICommandsAvian::Write(0x00, 0x00000C));
    }
    catch (EProtocolFunction &e)  // TODO: remove once Radar GUI is deprecated
    {
        if (e.code() == STATUS_COMMAND_FUNCTION_INVALID)
        {
            // ensure backwards compatibility with older FW (needed by Radar GUI)
            auto regs = avian->getIRegisters();
            regs->setBits(0x00, 0x00000C);
        }
        else
        {
            throw;
        }
    }
}

// ---------------------------------------------------------------------------- ~StrataAvianPortAdapter
StrataAvianPortAdapter::~StrataAvianPortAdapter() = default;

// ---------------------------------------------------------------------------- get_properties
const StrataAvianPortAdapter::Properties& StrataAvianPortAdapter::get_properties() const
{
    return m_properties;
}

// ---------------------------------------------------------------------------- send_commands
void StrataAvianPortAdapter::send_commands(const Avian::HW::Spi_Command_t* commands,
                                           size_t num_words,
                                           Avian::HW::Spi_Response_t* response)
{
    m_cmd->execute(reinterpret_cast<const ICommandsAvian::Command *>(commands), narrow_cast<uint32_t>(num_words), response);
}

// ---------------------------------------------------------------------------- generate_reset_sequence
void StrataAvianPortAdapter::generate_reset_sequence()
{
    try
    {
        m_pins->reset();
    }
    catch (...)  // TODO: remove once Radar GUI is deprecated
    {
        // ensure backwards compatibility with older FW (needed by Radar GUI)
        m_pins->setResetPin(false);
        std::this_thread::sleep_for(std::chrono::microseconds(1));  // T_reset
        m_pins->setResetPin(true);
        std::this_thread::sleep_for(std::chrono::microseconds(1));  // T_reset
    }
}

// ---------------------------------------------------------------------------- read_irq_level
bool StrataAvianPortAdapter::read_irq_level()
{
    return m_pins->getIrqPin();
}

// ---------------------------------------------------------------------------- start_reader
void StrataAvianPortAdapter::start_reader(Avian::HW::Spi_Command_t burst_command,
                                          size_t burst_size,
                                          Data_Ready_Callback_t callback)
{
    /*
     * Before changing the reader configuration, any ongoing
     * acquisition must be stopped.
     */
    stop_reader();

    /*
     * Apply new reader configuration. 
     * This will throw an exception in case configuration is invalid.
     */
    const uint8_t flags   = 0;
    const uint8_t address = (burst_command & 0xFF);
    m_dataSize            = (burst_size & 0xFFFF);
    DataAvianSettings_t settings(flags, address, m_dataSize);

    IDataProperties_t properties = {};
    properties.format            = DataFormat_Auto;   // let device choose suitable data format (i.e. packed or unpacked)
    const size_t sampleSize      = sizeof(uint16_t);  // 16-bit word suits both packed and unpacked data samples

    m_data->configure(m_dataIndex, &properties, &settings);

    /*
     * Turn on the callback, and allocate memory to queue
     * the for incoming data stream.
     */
    m_dataReadyCallback = callback;

    const uint32_t bufferSize = m_dataSize * sampleSize;
    const uint16_t poolSize = 32;
    m_bridgeData->setFrameBufferSize(bufferSize);
    m_bridgeData->setFrameQueueSize(poolSize);
    m_bridgeData->startStreaming();

    /*
     * Enable data transmission on the device.
     */
    m_data->start(m_dataIndex);
}

// ---------------------------------------------------------------------------- stop_reader
void StrataAvianPortAdapter::stop_reader()
{
    /*
     * First the acquisition itself is stopped to make sure no more data is
     * produced. Stopping data forwarding is the second step. The lock before
     * turning off the callback guarantees that no more callback is in progress
     * when this method returns.
     */
    m_data->stop(m_dataIndex);
    m_bridgeData->stopStreaming();
    std::lock_guard<std::mutex> lock(m_stop_guard);
    m_dataReadyCallback = nullptr;
}

// ---------------------------------------------------------------------------- set_buffer
void StrataAvianPortAdapter::set_buffer(Avian::HW::Packed_Raw_Data_t* buffer)
{
    m_buffer = buffer;
}

// ---------------------------------------------------------------------------- register_error_callback
void StrataAvianPortAdapter::register_error_callback(Error_Callback_t callback)
{
    m_errorCallback = callback;
}

uint8_t StrataAvianPortAdapter::getComponentId() const
{
    return m_id;
}

// ---------------------------------------------------------------------------- onNewFrame
void StrataAvianPortAdapter::onNewFrame(IFrame* frame)
{
    const uint32_t status_code = frame->getStatusCode();

    if (status_code != 0)
    {
        /*
         * Emergency break!
         * Typically reasons for error frames are connection problems or a FIFO
         * overflow. In both cases it does not make sense to stream more data,
         * because this will only result in more error frames, but no more data.
         */
        m_bridgeData->stopStreaming();

        if (m_errorCallback)
        {
            m_errorCallback(this, status_code);
        }
        frame->release();
        return;
    }

    // handle processed data
    if (frame->getVirtualChannel() == PROCESSED_DATA_CHANNEL)
    {
        onNewProcessedFrame(frame);
        frame->release();
        return;
    }

    // handle raw data
    const uint32_t unpackedSampleSize = sizeof(uint16_t);
    const uint32_t unpackedSize = m_dataSize * unpackedSampleSize;
    if (frame->getDataSize() == unpackedSize)
    {
        // re-pack data (for the time being) to stay compatible with legacy GUI endpoint
        for (uint32_t i = 0, j = 0; i < frame->getDataSize(); i += 4, j += 3)
        {
            m_buffer[j] = (((frame->getData()[i + 1]) << 4) & 0xF0) | ((frame->getData()[i] >> 4) & 0x0F);
            m_buffer[j + 1] = (((frame->getData()[i]) << 4) & 0xF0) | ((frame->getData()[i + 3]) & 0x0F);
            m_buffer[j + 2] = (frame->getData()[i + 2]);
        }
    }
    else
    {
        std::copy(frame->getData(), frame->getData() + frame->getDataSize(), m_buffer);
    }

    frame->release();

    // This lock helps the stop_reader method to wait for the end of a callback.
    std::lock_guard<std::mutex> lock(m_stop_guard);
    if (m_dataReadyCallback)
    {
        m_dataReadyCallback(0);
    }
}

// ---------------------------------------------------------------------------- onNewProcessedFrame
void StrataAvianPortAdapter::onNewProcessedFrame(IFrame* /*frame*/)
{
    // do nothing. This is for derived classes to be implemented
}

// ----------------------------------------------------------------------------
}
}
