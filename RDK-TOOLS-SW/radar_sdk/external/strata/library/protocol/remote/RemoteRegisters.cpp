/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "RemoteRegisters.hpp"
#include <common/Buffer.hpp>
#include <common/Serialization.hpp>
#include <universal/components/subinterfaces.h>
#include <universal/components/subinterfaces/iregisters.h>
#include <universal/protocol/protocol_definitions.h>


template <typename AddressType, typename ValueType>
RemoteRegisters<AddressType, ValueType>::RemoteRegisters(AddressType increment, IVendorCommands *commands, uint8_t bType, uint8_t bImplementation, uint8_t id) :
    m_increment {increment},
    m_vendorCommands {commands, CMD_COMPONENT, bType, bImplementation, id, SUBIF_REGISTERS}
{
}

template <typename AddressType, typename ValueType>
ValueType RemoteRegisters<AddressType, ValueType>::read(AddressType regAddr)
{
    ValueType value;
    read(regAddr, 1, &value);
    return value;
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::read(AddressType regAddr, ValueType &value)
{
    read(regAddr, 1, &value);
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::write(AddressType regAddr, ValueType value)
{
    write(regAddr, 1, &value);
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::read(AddressType regAddr, AddressType count, ValueType values[])
{
    constexpr uint16_t elemSize = sizeof(values[0]);
    constexpr uint16_t argSize  = sizeof(regAddr) + sizeof(count);
    uint8_t payload[argSize];
    const decltype(count) maxCount = m_vendorCommands.getMaxTransfer() / elemSize;

    while (count > 0)
    {
        const decltype(count) wCount = std::min(count, maxCount);
        const uint16_t wLength       = wCount * elemSize;
        uint8_t *it                  = payload;
        it                           = hostToSerial(it, regAddr);
        it                           = hostToSerial(it, wCount);

        m_vendorCommands.vendorTransferChecked(FN_REGISTERS_READ_BURST, argSize, payload, wLength, reinterpret_cast<uint8_t *>(values));

        regAddr += (wCount * m_increment);
        values += wCount;
        count -= wCount;
    }
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::write(AddressType regAddr, AddressType count, const ValueType values[])
{
    constexpr uint16_t elemSize    = sizeof(values[0]);
    constexpr uint16_t argSize     = sizeof(regAddr);
    const decltype(count) maxCount = (m_vendorCommands.getMaxTransfer() - argSize) / elemSize;
    stdext::buffer<uint8_t> payload(std::min(maxCount * elemSize, count * elemSize) + argSize);

    while (count > 0)
    {
        const decltype(count) wCount = std::min(count, maxCount);
        const uint16_t wLength       = wCount * elemSize;
        uint8_t *it                  = payload.data();
        it                           = hostToSerial(it, values, values + wCount);
        it                           = hostToSerial(it, regAddr);

        m_vendorCommands.vendorWrite(FN_REGISTERS_WRITE_BURST, wLength + argSize, payload.data());

        regAddr += (wCount * m_increment);
        values += wCount;
        count -= wCount;
    }
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::writeBatch(const BatchType regVals[], AddressType count, bool /*optimize*/)
{
    // todo: implement optimized command
    for (unsigned int i = 0; i < count; i++)
    {
        write(regVals[i].address, 1, &regVals[i].value);
    }
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::setBits(AddressType regAddr, ValueType bitmask)
{
    const uint16_t wLength = sizeof(regAddr) + sizeof(bitmask);
    uint8_t payload[wLength];
    uint8_t *it = payload;
    it          = hostToSerial(it, regAddr);
    it          = hostToSerial(it, bitmask);
    m_vendorCommands.vendorWrite(FN_REGISTERS_SET_BITS, wLength, payload);
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::clearBits(AddressType regAddr, ValueType bitmask)
{
    const uint16_t wLength = sizeof(regAddr) + sizeof(bitmask);
    uint8_t payload[wLength];
    uint8_t *it = payload;
    it          = hostToSerial(it, regAddr);
    it          = hostToSerial(it, bitmask);
    m_vendorCommands.vendorWrite(FN_REGISTERS_CLEAR_BITS, wLength, payload);
}

template <typename AddressType, typename ValueType>
void RemoteRegisters<AddressType, ValueType>::modifyBits(AddressType regAddr, ValueType clearBitmask, ValueType setBitmask)
{
    const uint16_t wLength = sizeof(regAddr) + sizeof(setBitmask) + sizeof(clearBitmask);
    uint8_t payload[wLength];
    uint8_t *it = payload;
    it          = hostToSerial(it, regAddr);
    it          = hostToSerial(it, clearBitmask);
    it          = hostToSerial(it, setBitmask);
    m_vendorCommands.vendorWrite(FN_REGISTERS_MODIFY_BITS, wLength, payload);
}


/**
 * This avoids having to have the complete implementation in the header file,
 * since the linker of an external executable needs to find the implementation
 * of the class. It has to be after the member declaration, since some compilers
 * otherwise would not emit the symbols.
 */
template class RemoteRegisters<uint8_t>;
template class RemoteRegisters<uint16_t>;
template class RemoteRegisters<uint8_t, uint32_t>;
template class RemoteRegisters<uint8_t, uint16_t>;
