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

/**
 * @internal
 * @file RadarDevice.hpp
 *
 * @brief Defines the structure for the Radar Device Controller Module.
*/

#ifndef IFX_RADAR_INTERNAL_RADAR_DEVICE_HPP
#define IFX_RADAR_INTERNAL_RADAR_DEVICE_HPP

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <memory>
#include <atomic>
#include <chrono>
#include <iterator>

#include "ifxBase/internal/NonCopyable.hpp"
#include "ifxDevice/internal/DeviceControlAvian.hpp"
#include "ifxDevice/internal/RawDataFifo.hpp"

#include "ifxBase/Defines.h"

#include "ifxDevice/internal/RadarDeviceBase.hpp"

// libAvian
#include <ifxAvian_Driver.hpp>
#include <ifxAvian_CwController.hpp>

// Strata
#include <platform/BoardManager.hpp>

using namespace Infineon;


/*
==============================================================================
   1. FORWARD DECLARATIONS
==============================================================================
*/

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

/*
==============================================================================
   3. TYPES
==============================================================================
*/

/**
 * @brief The structure for eeprom header object.
 */
typedef struct {
    uint8_t magic[8];      /**< Magic string identifier. owned by object*/
    uint8_t version_major; /**< Major part of version number (major.minor), currently 1 */
    uint8_t version_minor; /**< Minor part of version number (major.minor), currently 0 */
    uint16_t board_type;   /**< Shield identifier using enum Board_Type_t */
    uint16_t nvm_type;     /**< node version manager identifier using enum EP_BSF_Nvm_Type_t */
    uint16_t reserved;     /**< reserved bytes, set to 0 */
    uint8_t uuid[16];      /**< 16 byte uuid */
} eeprom_header_t;

/**
 * @brief A handle for an instance of DeviceControl module, see DeviceControl.h.
 */
typedef struct RadarDeviceBase* ifx_Device_Handle_t;

/**
 * @brief Base board type
 */
enum class BoardType {
    Unknown = 0,
    RadarBaseboardAurix = 1,
    RadarBaseboardMCU7 = 2,
    V9 = 3
};

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/
struct ifx_Radar_Device_s : public RadarDeviceBase
{
    NONCOPYABLE(ifx_Radar_Device_s);

    ifx_Radar_Device_s(std::unique_ptr<BoardInstance> board);
    virtual ~ifx_Radar_Device_s();

    void get_temperature(ifx_Float_t* temperature_celsius) override;

    ifx_Error_t get_next_frame(ifx_Frame_R_t* frame, uint16_t timeout_ms) override;

    ifx_Float_t get_tx_power(uint8_t tx_antenna) override;

    BoardInstance* get_strata_avian_board() const override
    {
        return m_board.get();
    };

    Communication::StrataAvianPortAdapter* get_strata_avian_port() const override
    {
        return m_avian_port.get();
    };

    BoardType get_board_type_from_pid() const;

    const eeprom_header_t *get_eeprom_header() const;

    bool start_acquisition() override;
    bool stop_acquisition() override;

    static bool is_unknown_boardtype_from_pid(const uint16_t pid);

    Avian::Constant_Wave_Controller* get_constant_wave_controller() override;

protected:
    ifx_Radar_Device_s() = default;

private:
    ifx_Error_t send_to_device() override;

    bool read_eeprom_header();

    bool m_eeprom_header_valid;
    eeprom_header_t m_eeprom_header;

    std::chrono::steady_clock::time_point m_temperature_expiration_time; // timestamp until the cached temperature value is valid
    ifx_Float_t m_temperature_value = 0; // cached temperature value in degrees Celsius

protected:
    // attributes
    std::unique_ptr<BoardInstance> m_board;
    std::unique_ptr<Communication::StrataAvianPortAdapter> m_avian_port;

    std::vector<Avian::HW::Packed_Raw_Data_t> m_avian_buffer;
    RawDataFifo m_fifo;
    std::unique_ptr<Avian::Constant_Wave_Controller> m_cw_controller = nullptr;
};

#endif /* IFX_RADAR_INTERNAL_RADAR_DEVICE_HPP */
