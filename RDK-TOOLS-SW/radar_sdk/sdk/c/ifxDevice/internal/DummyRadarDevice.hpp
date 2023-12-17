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
 * @file DummyRadarDevice.hpp
 *
 * @brief Defines the structure for the Dummy Radar Device Controller Module.
*/

#ifndef IFX_RADAR_INTERNAL_DUMMY_RADAR_DEVICE_HPP
#define IFX_RADAR_INTERNAL_DUMMY_RADAR_DEVICE_HPP

#include "ifxDevice/internal/RadarDeviceBase.hpp"

#include "ifxDevice/internal/DummyControlPort.hpp"

#include "ifxDevice/StrataAvianPortAdapter.hpp"


struct ifx_Dummy_Radar_Device_s : public RadarDeviceBase
{
  NONCOPYABLE(ifx_Dummy_Radar_Device_s);

  ifx_Dummy_Radar_Device_s(int device_type);
  ifx_Dummy_Radar_Device_s() = delete;
  virtual ~ifx_Dummy_Radar_Device_s() = default;

  BoardInstance* get_strata_avian_board() const override;

  Infineon::Communication::StrataAvianPortAdapter* get_strata_avian_port() const override;

  bool start_acquisition() override;
  bool stop_acquisition() override;

  void get_temperature(ifx_Float_t* temperature_celsius) override;
  ifx_Error_t get_next_frame(ifx_Frame_R_t* frame, uint16_t timeout_ms) override;

  Infineon::Avian::Constant_Wave_Controller* get_constant_wave_controller() override;
  ifx_Float_t get_tx_power(uint8_t tx_antenna) override;
  ifx_Error_t send_to_device() override;

//private:
	DummyControlPort m_avian_dummy_port;
};

#endif
