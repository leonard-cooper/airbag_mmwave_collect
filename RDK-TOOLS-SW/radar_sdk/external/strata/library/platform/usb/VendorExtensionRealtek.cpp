/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include <common/Logger.hpp>
#include <platform/exception/EProtocol.hpp>

#include "VendorExtensionRealtek.hpp"


//todo: proper namespace
namespace
{

#define DATA_BUFFER_LENGTH 4096  // available buffer size for control transfers (minimum has to be the SPI page size, otherwise the writing to certain flash ICs may fail!)


#define I2C_IMAGER_ALIAS 0x7F


    /* Values for bmReqType */
#define VENDOR_REQ_READ  0xC1  // read commands - Direction: Device to Host (IN), Type: Vendor, Recipient: Interface
#define VENDOR_REQ_WRITE 0x41  // write commands - Direction: Device to Host (OUT), Type: Vendor, Recipient: Interface


    /* Values for bRequest */

#define CMD_LAST_ERROR   0x80 /* Read out last error information (this is mandatory to check if write commands succeeded)
                                                   write - (not implemented)
                                                   read  - wLength: 8
                                                           return: 32bit error value, 32bit detailed error
                                                   */
#define CMD_VERSION_INFO 0x81 /* Read out version information and supported features
                                                   write - (not implemented)
                                                   read  - wLength: 8
                                                           return: 8 byte values with bit fields
                                                   */

#define CMD_I2C_NO_REGISTER    0x00 /* I2C access with no addressing
                                                   read/write - wValue: I2C address
                                                                wLength: byte count
                                                   */
#define CMD_I2C_8BIT_REGISTER  0x01 /* I2C access with 8bit addressing
                                                   read/write - wValue: I2C address
                                                                wIndex: register address
                                                                wLength: byte count
                                                   */
#define CMD_I2C_16BIT_REGISTER 0x02 /* I2C access with 16bit addressing
                                                   read/write - wValue: I2C address
                                                                wIndex: register address
                                                                wLength: byte count
                                                   */
#define CMD_I2C_BUS_CONFIG     0x03 /* I2C bus configuration
                                                   write - wValue: 32bit clock HIGH word
                                                           wIndex: 32bit clock LOW word
                                                           wLength: 0
                                                   read  - (not implemented)
                                                   */
#define CMD_I2C_IMAGER_ADDRESS 0x04 /* I2C address used for Imager alias access
                                                   write - wValue: 7bit I2C address
                                                           wLength: 0
                                                   read  - wLength: 1
                                                           return: 7bit I2C address
                                                   */

#define CMD_FLASH_TRANSFER  0x06 /* Access (SPI) flash (must be page-wise for write and segment-wise for read (read restriction only if applicable))
                                                   read/write - wValue: (id << 8) & (24bit address HIGH byte)
                                                                wIndex: 24bit address LOW word
                                                                wLength: byte count
                                                   */
#define CMD_FLASH_ERASE_WIP 0x07 /* write: Erase (SPI) flash sector (the one at given address) / read: Flash WIP (write-in-progress) bit
                                                   write   - wValue: (id << 8) & (24bit address HIGH byte)
                                                             wIndex: 24bit address LOW word
                                                             wLength: 0
                                                   read    - wLength: 1
                                                             return: 1 bit WIP
                                                   */
#define CMD_FLASH_SELECT    0x08 /* Select 8bit SPI slave device ID to be used for custom command
                                                   write - wValue: 8bit device ID
                                                           wLength: 0
                                                   read  - wLength: 1
                                                           return: 8bit device ID
                                                   */
#define CMD_FLASH_COMMAND   0x09 /* send custom command to (SPI) Flash
                                                   read/write - wValue: (command << 8) & (24bit address HIGH byte)
                                                                wIndex: 24bit address LOW word
                                                                wLength: byte count
                                                   */
#define CMD_SPI_GENERIC     0x10 /* send/receive raw byte stream to any SPI device
                                                   read/write - wValue: SPI slave device ID
                                                                wIndex: LOW byte  - 0 for releasing slave select, 1 for keeping slave select
                                                                        HIGH byte - 0 for "operation finished with this call", 1 for "more calls following for this operation"
                                                                wLength: byte count
                                                  */
#define CMD_SPI_BUS_CONFIG  0x11 /* SPI bus configuration
                                                   write - wValue: 32bit clock HIGH word
                                                           wIndex: 32bit clock LOW word
                                                           wLength: 0
                                                   read  - (not implemented)
                                                   */

#define CMD_GPIO_CONFIG  0x20 /* configure GPIO by number or by name
                                                   write - wValue: GPIO config flags
                                                           wIndex: GPIO number or name
                                                           wLength: 0
                                                   read  - (not implemented)
                                                   */
#define CMD_GPIO_CONTROL 0x21 /* set GPIO to a certain logical level or read logical level
                                                   write - wValue: 0 for low, 1 for high
                                                           wIndex: GPIO number or name
                                                           wLength: 0
                                                   read  - wIndex: GPIO number or name
                                                           wLength: 1
                                                           return: 0 for low, 1 for high
                                                   */

}


VendorExtensionRealtek::VendorExtensionRealtek(IUvcExtension *)
{
}

VendorExtensionRealtek::~VendorExtensionRealtek()
{
}

void VendorExtensionRealtek::getBoardInfo(BoardInfo_t & /*buffer*/)
{
}

void VendorExtensionRealtek::checkVersion()
{
}

uint16_t VendorExtensionRealtek::getMaxTransfer() const
{
    return DATA_BUFFER_LENGTH;
}

void VendorExtensionRealtek::openConnection()
{
}

void VendorExtensionRealtek::closeConnection()
{
}

void VendorExtensionRealtek::vendorWrite(uint16_t /*command*/, uint16_t /*length*/, uint32_t /*address*/)
{
}

void VendorExtensionRealtek::vendorWrite(uint16_t /*command*/, uint16_t /*addressLow*/, uint16_t /*length*/, uint16_t /*addressHigh*/, const uint8_t /*buffer*/[])
{
}

void VendorExtensionRealtek::vendorRead(uint16_t /*command*/, uint16_t /*addressLow*/, uint16_t /*length*/, uint16_t /*addressHigh*/, uint8_t /*buffer*/[])
{
}

void VendorExtensionRealtek::checkError()
{
}


void VendorExtensionRealtek::configureGpio(uint16_t id, uint8_t flags)
{
    vendorWrite(CMD_GPIO_CONFIG, flags, id);
}

void VendorExtensionRealtek::setGpio(uint16_t id, bool state)
{
    vendorWrite(CMD_GPIO_CONTROL, state ? 1 : 0, id);
}

bool VendorExtensionRealtek::getGpio(uint16_t id)
{
    uint8_t buf;
    vendorRead(CMD_GPIO_CONTROL, 0, id, 1, &buf);
    return (buf != 0);
}

uint16_t VendorExtensionRealtek::getI2cMaxTransfer()
{
    return DATA_BUFFER_LENGTH;
}

void VendorExtensionRealtek::writeI2cWithoutPrefix(uint16_t devAddr, uint16_t length, const uint8_t buffer[])
{
    vendorWrite(CMD_I2C_NO_REGISTER, devAddr, 0, length, buffer);
}

void VendorExtensionRealtek::writeI2cWith8BitPrefix(uint16_t devAddr, uint8_t prefix, uint16_t length, const uint8_t buffer[])
{
    vendorWrite(CMD_I2C_8BIT_REGISTER, devAddr, prefix, length, buffer);
}

void VendorExtensionRealtek::writeI2cWith16BitPrefix(uint16_t devAddr, uint16_t prefix, uint16_t length, const uint8_t buffer[])
{
    vendorWrite(CMD_I2C_16BIT_REGISTER, devAddr, prefix, length, buffer);
}

void VendorExtensionRealtek::readI2cWithoutPrefix(uint16_t devAddr, uint16_t length, uint8_t buffer[])
{
    vendorRead(CMD_I2C_NO_REGISTER, devAddr, 0, length, buffer);
}

void VendorExtensionRealtek::readI2cWith8BitPrefix(uint16_t devAddr, uint8_t prefix, uint16_t length, uint8_t buffer[])
{
    vendorRead(CMD_I2C_8BIT_REGISTER, devAddr, prefix, length, buffer);
}

void VendorExtensionRealtek::readI2cWith16BitPrefix(uint16_t devAddr, uint16_t prefix, uint16_t length, uint8_t buffer[])
{
    vendorRead(CMD_I2C_16BIT_REGISTER, devAddr, prefix, length, buffer);
}

void VendorExtensionRealtek::setI2cBusSpeed(uint32_t speed)
{
    vendorWrite(CMD_I2C_BUS_CONFIG, static_cast<uint16_t>(speed >> 16), static_cast<uint16_t>(speed));
}

void VendorExtensionRealtek::configureSpi(uint8_t /*devId*/, uint32_t speed)
{
    // devId is ignored, since the FW currently does not support to set different clocks for different devices.
    // If desired, it can still be implemented in this class.
    vendorWrite(CMD_SPI_BUS_CONFIG, static_cast<uint16_t>(speed >> 16), static_cast<uint16_t>(speed));
}

uint32_t VendorExtensionRealtek::getSpiMaxTransfer()
{
    return DATA_BUFFER_LENGTH;
}

void VendorExtensionRealtek::writeSpi(uint8_t devId, uint32_t count, const uint8_t buffer[])
{
    vendorWrite(CMD_SPI_GENERIC, devId, 0, count, buffer);
}
void VendorExtensionRealtek::writeSpi(uint8_t /*devId*/, uint32_t /*count*/, const uint16_t /*buffer*/[])
{
}
void VendorExtensionRealtek::writeSpi(uint8_t /*devId*/, uint32_t /*count*/, const uint32_t /*buffer*/[])
{
}


void VendorExtensionRealtek::readSpi(uint8_t devId, uint32_t count, uint8_t buffer[])
{
    vendorRead(CMD_SPI_GENERIC, devId, 0, count, buffer);
}
void VendorExtensionRealtek::readSpi(uint8_t /*devId*/, uint32_t /*count*/, uint16_t /*buffer*/[])
{
}
void VendorExtensionRealtek::readSpi(uint8_t /*devId*/, uint32_t /*count*/, uint32_t /*buffer*/[])
{
}

uint32_t VendorExtensionRealtek::getFlashMaxTransfer()
{
    return DATA_BUFFER_LENGTH;
}

void VendorExtensionRealtek::readFlash(uint8_t devId, uint32_t address, uint32_t length, uint8_t buffer[])
{
    vendorRead(CMD_FLASH_TRANSFER, static_cast<uint16_t>(devId << 8 | address >> 16), static_cast<uint16_t>(address), length, buffer);
}

void VendorExtensionRealtek::writeFlash(uint8_t devId, uint32_t address, uint32_t length, const uint8_t buffer[])
{
    vendorWrite(CMD_FLASH_TRANSFER, static_cast<uint16_t>(devId << 8 | address >> 16), static_cast<uint16_t>(address), length, buffer);
}

void VendorExtensionRealtek::eraseFlash(uint8_t devId, uint32_t address)
{
    vendorWrite(CMD_FLASH_ERASE_WIP, static_cast<uint16_t>(devId << 8 | address >> 16), static_cast<uint16_t>(address));
}

uint8_t VendorExtensionRealtek::getFlashStatus(uint8_t devId)
{
    uint8_t status;
    vendorRead(CMD_FLASH_ERASE_WIP, static_cast<uint16_t>(devId << 8), 0, 1, &status);
    return status;
}
