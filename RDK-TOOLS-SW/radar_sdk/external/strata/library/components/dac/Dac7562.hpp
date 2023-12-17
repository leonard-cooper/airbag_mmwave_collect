/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/interfaces/IDac.hpp>
#include <components/interfaces/IRegisters.hpp>
#include <platform/interfaces/access/ISpi.hpp>


class Dac7562 :
    public IDac
{
public:
    Dac7562(ISpi *accessSpi, uint8_t devId);

    /**
     * \brief Set DAC gain
     *
     * This function writes 3 bytes to the specified QSPI channel.
     * The values are formatted as DAC gains as per TI DACxx6x specification.
     *
     * \param   IfxQspi_SpiMaster_Channel *ch   : QSPI channel to DAC
     *          u16 gain1                       : DAC-A gain
     *          u16 gain2                       : DAC-B gain
     * \return  void
     */
    void setGain(uint8_t gain1, uint8_t gain2);

    /**
     * \brief Enable/Disable DAC internal reference
     *
     * This function writes 3 bytes to the specified QSPI channel.
     * The values are formatted to enable/disable DAC internal reference
     * as per TI DACxx6x specification.
     *
     * \param   IfxQspi_SpiMaster_Channel *ch   : QSPI channel to DAC
     *          boolean enableInternalReference : TRUE - enable, FALSE - disable
     * \return  void
     */
    void enableInternalReference(bool enableInternalReference);

    /**
     * \brief Power Up/Down DAC
     *
     * This function writes 3 bytes to the specified QSPI channel.
     * The values are formatted to power up/down DAC as per TI DACxx6x specification.
     *
     * \param   IfxQspi_SpiMaster_Channel *ch   : QSPI channel to DAC
     *          u8 ID                           : 0 -> DAC-A, 1 -> DAC-B
     *          boolean powerUp                 : TRUE - Power UP, FALSE - Power DOWN
     * \return  void
     */
    void powerUpDown(uint8_t ID, bool powerUp);

    /**
     * \brief Write to DAC
     *
     * This function writes 3 bytes to the specified QSPI channel.
     * The values are written to the DAC input channel as per TI DACxx6x specification.
     *
     * \param   IfxQspi_SpiMaster_Channel *ch   : QSPI channel to DAC
     *          u8 ID                           : 0 -> DAC-A, 1 -> DAC-B
     *          u16 value                       : DAC input value
     * \return  void
     */
    void setValue(uint8_t ID, uint16_t value);

private:
    ISpi *m_accessSpi;
    uint8_t m_devId;
};
