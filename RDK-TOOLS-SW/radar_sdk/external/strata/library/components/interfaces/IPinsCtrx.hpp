#pragma once

#include <Definitions.hpp>
#include <stdint.h>

/**
 * @brief Interface to interact with certain pins of the CTRX chip
 */
class IPinsCtrx
{
public:
    STRATA_API virtual ~IPinsCtrx() = default;

    /**
     * @brief Set the reset pin of the DUT
     * @param state Reset state: true puts the chip into reset mode which has to be released again 
     * by setting the state to false
     *
     */
    virtual void setResetPin(bool state) = 0;

    /**
     * @brief Get the "ok-pin" of the DUT
     */
    virtual bool getOkPin() = 0;

    /**
     * @brief Configure the direction (I/O) of a DMUX pin
     * @param index used to chose the DMUX pin to configure 
     *              index=1,2,3 maps to DMUX1, DMUX2, and DMUX3
     * @param output defines whether the pin is defined as output (true) or as input (false - default)
     */
    virtual void configureDmuxPin(uint8_t index, bool output) = 0;

    /**
     * @brief Reads the current value of a DMUX pin
     * @param index used to chose the DMUX pin to read 
     *              index=1,2,3 maps to DMUX1, DMUX2, and DMUX3
     */
    virtual bool getDmuxPin(uint8_t index) = 0;

    /**
     * @brief Sets the value of a DMUX pin
     * @param index used to chose the DMUX pin to write
     *              index=1,2,3 maps to DMUX1, DMUX2, and DMUX3
     * @param state true sets the DMUX pin to high, false sets the pin to low
     */
    virtual void setDmuxPin(uint8_t index, bool state) = 0;

    /**
     * @deprecated Not supported on the hardware
     */
    virtual double getAmuxPin(uint8_t index) = 0;

    /**
     * @deprecated Not supported on the hardware
     */
    virtual void setAmuxPin(uint8_t index, double value) = 0;

    /**
     * @brief Get the Ready For Transfer pin which identifies that the chip is ready for a SPI
     * transaction
     */
    virtual bool getSpiRft() const = 0;

    /**
    * @brief Wait for the Ready for transfer input pin indicating that SPI is ready for a new transfer.
    * @note    The implementations of this interface can implement their specific waiting logic
    * @return True if the device is ready for a new transmission, false upon timeout (device not yet ready after waiting time)
    */
    STRATA_API virtual bool waitForSpiRft() const = 0;
};
