/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include "RemoteVendorCommands.hpp"
#include <components/interfaces/IRegisters.hpp>
#include <platform/interfaces/IVendorCommands.hpp>


template <typename AddressType, typename ValueType = AddressType>
class RemoteRegisters :
    public IRegisters<AddressType, ValueType>
{
public:
    RemoteRegisters(AddressType increment, IVendorCommands *commands, uint8_t bType, uint8_t bImplementation, uint8_t id);

    ValueType read(AddressType regAddr) override;
    void read(AddressType regAddr, ValueType &value) override;
    void write(AddressType regAddr, ValueType value) override;
    void read(AddressType regAddr, AddressType count, ValueType values[]) override;
    void write(AddressType regAddr, AddressType count, const ValueType values[]) override;

    using typename IRegisters<AddressType, ValueType>::BatchType;
    void writeBatch(const BatchType regVals[], AddressType count, bool optimize) override;

    void setBits(AddressType regAddr, ValueType bitmask) override;
    void clearBits(AddressType regAddr, ValueType bitmask) override;
    void modifyBits(AddressType regAddr, ValueType clearBitmask, ValueType setBitmask) override;

protected:
    AddressType m_increment;
    RemoteVendorCommands m_vendorCommands;
};


extern template class RemoteRegisters<uint8_t>;
extern template class RemoteRegisters<uint16_t>;
extern template class RemoteRegisters<uint8_t, uint32_t>;
extern template class RemoteRegisters<uint8_t, uint16_t>;
