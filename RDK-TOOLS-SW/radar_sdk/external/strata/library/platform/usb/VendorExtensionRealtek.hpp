/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <mutex>

#include <platform/bridge/BridgeControl.hpp>
#include <platform/interfaces/link/IUvcExtension.hpp>

#define REALTEK_XU_GUID 0x8C, 0xA7, 0x29, 0x12, 0xB4, 0x47, 0x94, 0x40, 0xCE, 0xB0, 0xDB, 0x07, 0x38, 0x6F, 0xB9, 0x38  // {1229A78C-47B4-4094-B0CE-DB07386FB938}


class VendorExtensionRealtek :
    public BridgeControl
{
public:
    VendorExtensionRealtek(IUvcExtension *uvcExtension);
    ~VendorExtensionRealtek();

    void checkVersion() override;
    void getBoardInfo(BoardInfo_t &buffer) override;
    uint16_t getMaxTransfer() const override;

    void openConnection();
    void closeConnection();

    //IAccessGpio
    void configureGpio(uint16_t id, uint8_t flags);
    void setGpio(uint16_t id, bool state);
    bool getGpio(uint16_t id);

    //IAccessI2C
    uint16_t getI2cMaxTransfer();
    void setI2cBusSpeed(uint32_t speed);
    void writeI2cWithoutPrefix(uint16_t devAddr, uint16_t length, const uint8_t buffer[]);
    void writeI2cWith8BitPrefix(uint16_t devAddr, uint8_t prefix, uint16_t length, const uint8_t buffer[]);
    void writeI2cWith16BitPrefix(uint16_t devAddr, uint16_t prefix, uint16_t length, const uint8_t buffer[]);
    void readI2cWithoutPrefix(uint16_t devAddr, uint16_t length, uint8_t buffer[]);
    void readI2cWith8BitPrefix(uint16_t devAddr, uint8_t prefix, uint16_t length, uint8_t buffer[]);
    void readI2cWith16BitPrefix(uint16_t devAddr, uint16_t prefix, uint16_t length, uint8_t buffer[]);

    //IAccessSpi
    uint32_t getSpiMaxTransfer();
    void configureSpi(uint8_t devId, uint32_t speed);
    void writeSpi(uint8_t devId, uint32_t count, const uint8_t buffer[]);
    void writeSpi(uint8_t devId, uint32_t count, const uint16_t buffer[]);
    void writeSpi(uint8_t devId, uint32_t count, const uint32_t buffer[]);
    void readSpi(uint8_t devId, uint32_t count, uint8_t buffer[]);
    void readSpi(uint8_t devId, uint32_t count, uint16_t buffer[]);
    void readSpi(uint8_t devId, uint32_t count, uint32_t buffer[]);

    //IAccessFlash
    uint32_t getFlashMaxTransfer();
    void readFlash(uint8_t devId, uint32_t address, uint32_t length, uint8_t buffer[]);
    void writeFlash(uint8_t devId, uint32_t address, uint32_t length, const uint8_t buffer[]);
    void eraseFlash(uint8_t devId, uint32_t address);
    uint8_t getFlashStatus(uint8_t devId);

    void vendorWrite(uint16_t command, uint16_t length, uint32_t address);
    void vendorWrite(uint16_t command, uint16_t addressLow, uint16_t length, uint16_t addressHigh, const uint8_t buffer[]);
    void vendorRead(uint16_t command, uint16_t addressLow, uint16_t length, uint16_t addressHigh, uint8_t buffer[]);

private:
    std::mutex m_lock;

    void checkError();
};
