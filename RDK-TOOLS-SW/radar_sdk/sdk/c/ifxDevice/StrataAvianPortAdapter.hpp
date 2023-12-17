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

#ifndef STRATA_AVIAN_PORT_ADAPTER_H_INCLUDED
#define STRATA_AVIAN_PORT_ADAPTER_H_INCLUDED

// ---------------------------------------------------------------------------- includes
#include "ifxAvian_IPort.hpp"
#include <components/interfaces/IRadarAvian.hpp>
#include <components/interfaces/IProcessingRadar.hpp>
#include <platform/BoardInstance.hpp>
#include <memory>
#include <mutex>

// ---------------------------------------------------------------------------- namespaces
namespace Infineon
{
namespace Communication
{

// ----------------------------------------------------------------------------
constexpr uint32_t ERR_FIFO_OVERFLOW = static_cast<uint32_t>(0x012E);

// forward declaration
class StrataAvianPortAdapter;

/**
 * \brief This type defines a callback function for notification when
 *        an error occured.
 *
 * The error callback is called whenever an error occured.
 *
 * The first argument is a pointer to the instance of
 * \ref StrataAvianPortAdapter.
 *
 * The error code is given as the second argument to the callback. The
 * error code corresponds to IFrame::getStatusCode. On a FIFO overflow
 * the value is given by \ref ERR_FIFO_OVERFLOW. All other values correspond
 * to communcation problems (typically data loss between board and host).
 */
typedef std::function<void(StrataAvianPortAdapter*, uint32_t)> Error_Callback_t;

// ---------------------------------------------------------------------------- StrataAvianPortAdapter
class StrataAvianPortAdapter :
    public Avian::HW::IPort<Avian::HW::Packed_Raw_Data_t>,
    public IFrameListener<>
{
public:
    StrataAvianPortAdapter(BoardInstance* board);
    ~StrataAvianPortAdapter() override;

    const Properties& get_properties() const override;

    void send_commands(const Avian::HW::Spi_Command_t* commands, size_t num_words,
                       Avian::HW::Spi_Response_t* response = nullptr) override;

    void generate_reset_sequence() override;

    bool read_irq_level() override;

    void start_reader(Avian::HW::Spi_Command_t burst_command, size_t burst_size,
                      Data_Ready_Callback_t callback) override;

    void stop_reader() override;

    void set_buffer(Avian::HW::Packed_Raw_Data_t* buffer) override;

    void onNewFrame(IFrame* frame) override;

    /**
     * \brief Register error callback.
     *
     * Register an error callback function. The function is called when
     * an error occurs (FIFO overflow or communication error).
     *
     * To unregister the error callback you pass a nullpointer for
     * callback.
     *
     * \param [in]  callback    error callback or nullptr
     */
    void register_error_callback(Error_Callback_t callback);

    /**
     * @brief get the id of the board FW's component that is used by this instance
     *        by definition, this corresponst to Spi devId, Data index, and I2C bus id
     *
     * @return the component id
     */
    uint8_t getComponentId() const;

private:
    IBridgeData *m_bridgeData;
    Properties m_properties;
    Error_Callback_t m_errorCallback = nullptr;
    std::mutex m_stop_guard;

protected:
    uint8_t m_id;
    uint8_t m_dataIndex;

    Avian::HW::Packed_Raw_Data_t* m_buffer = nullptr;
    Data_Ready_Callback_t m_dataReadyCallback = nullptr;
    uint16_t m_dataSize = 0;

    IData* m_data;
    ICommandsAvian *m_cmd;
    IPinsAvian *m_pins;

    virtual void onNewProcessedFrame(IFrame* frame);
};
/* ------------------------------------------------------------------------ */

} // namespace Communication
} // namespace Infineon

#endif /* STRATA_AVIAN_PORT_ADAPTER_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
