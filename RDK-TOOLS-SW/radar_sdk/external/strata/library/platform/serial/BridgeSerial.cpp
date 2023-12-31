/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "BridgeSerial.hpp"

#include <common/Buffer.hpp>
#include <common/Logger.hpp>
#include <common/ScopeExit.hpp>
#include <common/Serialization.hpp>
#include <common/Time.hpp>
#include <common/crc/Crc16.hpp>
#include <platform/exception/EBridgeData.hpp>
#include <platform/exception/EProtocol.hpp>
#include <platform/exception/EProtocolFunction.hpp>
#include <platform/frames/ErrorFrame.hpp>
#include <universal/protocol/protocol_definitions.h>


//#define BRIDGE_SERIAL_DATA_DEBUG


namespace
{
    constexpr bool setLocalTimestamp = false;

    constexpr const uint16_t packetStartSize = 4;
    constexpr const uint16_t packetCrcSize   = 2;
    constexpr const uint16_t frameHeaderSize = 6;
    constexpr const uint32_t timestampSize   = sizeof(uint64_t);

    constexpr const uint32_t defaultBaudrate = 921600;
    constexpr const uint16_t portTimeout     = 100;

    constexpr const std::chrono::milliseconds enumerateTimeout(portTimeout);
    constexpr const std::chrono::milliseconds defaultTimeout(1000);
}


BridgeSerial::BridgeSerial(const char port[]) :
    m_protocol(this),
    m_portName {port}
{
    openConnection();
}

BridgeSerial::~BridgeSerial()
{
    closeConnection();
}

bool BridgeSerial::isConnected()
{
    return m_port.isOpened();
}

void BridgeSerial::openConnection()
{
    m_commandActive = false;
    m_commandError  = false;

    m_cachedPacket = None;

    m_port.open(m_portName.c_str(), defaultBaudrate, portTimeout);
    m_port.clearInputBuffer();  // if the previous connection was not closed gracefully, there might be stale data left

    m_timeout = enumerateTimeout;
}

void BridgeSerial::closeConnection()
{
    stopStreaming();
    m_port.close();
}

IBridgeData *BridgeSerial::getIBridgeData()
{
    return this;
}

void BridgeSerial::setFrameBufferSize(uint32_t size)
{
    m_framePool.setFrameBufferSize(size + timestampSize);
}

void BridgeSerial::setFramePoolCount(uint16_t count)
{
    m_framePool.setFrameCount(count);
}

IBridgeControl *BridgeSerial::getIBridgeControl()
{
    return &m_protocol;
}

void BridgeSerial::startStreaming()
{
    if (isBridgeDataStarted())
    {
        return;
    }

    if (!m_port.isOpened())
    {
        throw EBridgeData("Calling startData() without being connected");
    }
    if (!m_framePool.initialized())
    {
        throw EBridgeData("Calling startData() without frame pool being initialized");
    }

    startBridgeData();

    m_dataThread = std::thread(&BridgeSerial::dataThreadFunction, this);
}

void BridgeSerial::stopStreaming()
{
    if (!isBridgeDataStarted())
    {
        return;
    }

    stopBridgeData();

    {
        std::lock_guard<std::mutex> lock(m_lock);  // lock to update CV without race condition
    }
    m_cv.notify_all();
    m_dataThread.join();
}

void BridgeSerial::setDefaultTimeout()
{
    m_timeout = defaultTimeout;
}

uint16_t BridgeSerial::getMaxTransfer() const
{
    return m_maxPayload - m_commandHeaderSize - packetCrcSize;
}

void BridgeSerial::sendRequest(uint8_t bmReqType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint8_t buffer[])
{
    uint8_t packet[m_commandHeaderSize + packetCrcSize] = {
        bmReqType,
        bRequest,
        static_cast<uint8_t>(wValue),
        static_cast<uint8_t>(wValue >> 8),
        static_cast<uint8_t>(wIndex),
        static_cast<uint8_t>(wIndex >> 8),
        static_cast<uint8_t>(wLength),
        static_cast<uint8_t>(wLength >> 8),
    };

    if (bmReqType == VENDOR_REQ_READ)
    {
        wLength = 0;
    }

    uint16_t crc = Crc16CcittFalse(packet, m_commandHeaderSize);
    if (wLength)
    {
        crc = Crc16CcittFalse(buffer, wLength, crc);
    }
    packet[m_commandHeaderSize]     = static_cast<uint8_t>(crc >> 8);
    packet[m_commandHeaderSize + 1] = static_cast<uint8_t>(crc);

    if (m_commandError)
    {
        m_port.clearInputBuffer();
    }

    if (wLength)
    {
        m_port.send(packet, m_commandHeaderSize);
        m_port.send(buffer, wLength);
        m_port.send(&packet[m_commandHeaderSize], packetCrcSize);
    }
    else
    {
        m_port.send(packet, m_commandHeaderSize + packetCrcSize);
    }
    m_commandActive = true;
}

void BridgeSerial::receiveResponse(uint8_t bmReqType, uint8_t bRequest, uint16_t &wLength, uint8_t buffer[])
{
    const auto maxLength = wLength;
    uint8_t packet[m_responseHeaderSize + packetCrcSize];
    {
        std::unique_lock<std::mutex> lock(m_lock);
        auto endCommand = stdext::ScopeExit([this] {
            m_commandActive = false;
            m_cv.notify_one();
        });

        m_commandError        = true;
        uint16_t returnedSize = 0;
        const auto expiry     = std::chrono::steady_clock::now() + m_timeout;
        do
        {
            m_cv.wait(lock, [this] { return (m_cachedPacket != Data) || !isBridgeDataStarted(); });
            if (readPacketStart(packet, Control, !isBridgeDataStarted()))
            {
                returnedSize = packetStartSize;  // no further read necessary, since (m_responseHeaderSize == packetStartSize)
                break;
            }
        } while (std::chrono::steady_clock::now() < expiry);
        if (returnedSize < m_responseHeaderSize)
        {
            throw EProtocol("Request response header not received", returnedSize);
        }

        wLength = serialToHost<uint16_t>(&packet[2]);
        if (wLength > maxLength)
        {
            throw EProtocol("Request response too long for buffer", (wLength << 16) | (bmReqType << 8) | bRequest);
        }
        if (wLength != 0)
        {
            if (m_port.receive(buffer, wLength) != wLength)
            {
                throw EProtocol("Request response payload not completely received");
            }
        }
        if (m_port.receive(&packet[m_responseHeaderSize], packetCrcSize) != packetCrcSize)
        {
            throw EProtocol("Request response CRC not completely received");
        }
    }
    m_commandError = false;

    const uint8_t &bmResType = packet[0];
    const uint8_t &bStatus   = packet[1];

    uint16_t crc = Crc16CcittFalse(packet, m_responseHeaderSize);
    if (wLength != 0)
    {
        crc = Crc16CcittFalse(buffer, wLength, crc);
    }
    crc = Crc16CcittFalse(&packet[m_responseHeaderSize], packetCrcSize, crc);
    if (crc)
    {
        throw EProtocol("Request response CRC error", (crc << 16) | (bmResType << 8) | bRequest);
    }
    if (bmResType != bmReqType)
    {
        throw EProtocol("Request response type error", (bmResType << 8) | bmReqType);
    }
    if (bStatus)
    {
        throw EProtocolFunction(bStatus);
    }
    if (bmReqType == VENDOR_REQ_READ)
    {
        if (wLength != maxLength)
        {
            throw EProtocol("Read request response length error", (wLength << 16) | maxLength);
        }
    }
}

void BridgeSerial::vendorWrite(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, const uint8_t buffer[])
{
    std::lock_guard<std::mutex> lock(m_commandLock);

    sendRequest(VENDOR_REQ_WRITE, bRequest, wValue, wIndex, wLength, buffer);
    wLength = 0;
    receiveResponse(VENDOR_REQ_WRITE, bRequest, wLength, nullptr);
}

void BridgeSerial::vendorRead(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint8_t buffer[])
{
    std::lock_guard<std::mutex> lock(m_commandLock);

    sendRequest(VENDOR_REQ_READ, bRequest, wValue, wIndex, wLength, nullptr);
    receiveResponse(VENDOR_REQ_READ, bRequest, wLength, buffer);
}

void BridgeSerial::vendorTransfer(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLengthSend, const uint8_t bufferSend[], uint16_t &wLengthReceive, uint8_t bufferReceive[])
{
    std::lock_guard<std::mutex> lock(m_commandLock);

    sendRequest(VENDOR_REQ_TRANSFER, bRequest, wValue, wIndex, wLengthSend, bufferSend);
    receiveResponse(VENDOR_REQ_TRANSFER, bRequest, wLengthReceive, bufferReceive);
}

bool BridgeSerial::readPacketStart(uint8_t buffer[], PacketType type, bool discardOther)
{
    if (m_cachedPacket == type)
    {
        std::copy(std::begin(m_packetStartCache), std::end(m_packetStartCache), buffer);

        // lock to update CV without race condition is already acquired by caller
        m_cachedPacket = None;
        m_cv.notify_one();

        return true;
    }
    else if (m_cachedPacket == None)
    {
        const uint16_t returnedSize = m_port.receive(buffer, packetStartSize);
        if (!returnedSize)
        {
            return false;
        }
        else if (returnedSize < packetStartSize)
        {
            throw EProtocol("readPacketStart() - incomplete", returnedSize);
        }
        else
        {
            PacketType currentPacket;
            const auto &bmPktType = buffer[0];

            if ((bmPktType & 0xF0) == DATA_FRAME_PACKET)
            {
                currentPacket = Data;
            }
            else
            {
                switch (bmPktType)
                {
                    case VENDOR_REQ_READ:
                    case VENDOR_REQ_WRITE:
                    case VENDOR_REQ_TRANSFER:
                        currentPacket = Control;
                        break;
                    default:
                        throw EProtocol("readPacketStart() - unknown packet type, synchronization lost!", (buffer[0] << 24) | (buffer[1] << 24) | (buffer[2] << 8) | buffer[3]);
                        // or implement synchronization recovery mechanism
                }
            }

            if (currentPacket == type)
            {
                return true;
            }

            std::copy(buffer, buffer + packetStartSize, m_packetStartCache);
            m_cachedPacket = currentPacket;
        }
    }
    else if (discardOther)
    {
        uint16_t wLength;
        if (m_cachedPacket == Data)
        {
            // data is not started, but we received a data packet, so we just dump it...
            m_port.receive(reinterpret_cast<uint8_t *>(&wLength), 2);
            littleToHost(wLength);
#ifdef BRIDGE_SERIAL_DATA_DEBUG
            LOG(DEBUG) << "BridgeSerial::readPacketStart() - discarding cached data packet";
#endif
        }
        else  // Control
        {
            // control response received, but no command is active, so we just dump it...
            wLength = serialToHost<uint16_t>(buffer + 2);
#ifdef BRIDGE_SERIAL_DATA_DEBUG
            LOG(DEBUG) << "BridgeSerial::readPacketStart() - discarding cached control packet";
#endif
        }

        stdext::buffer<uint8_t> dump(wLength + packetCrcSize);
        m_port.receive(dump.data(), wLength + packetCrcSize);

        m_cachedPacket = None;
    }

    // lock to update CV without race condition is already acquired by caller
    m_cv.notify_one();
    return false;
}

void BridgeSerial::dumpRemainder(uint16_t wLength)
{
    stdext::buffer<uint8_t> dump(wLength + packetCrcSize);
    m_port.receive(dump.data(), wLength + packetCrcSize);  // try to not lose the snyc by reading the remaining data packet
}

void BridgeSerial::dataThreadFunction()
{
    IFrame *frame = nullptr;
    uint8_t *bufBegin, *bufEnd;
    uint8_t *buf;

    uint64_t epochTimestamp;
    uint16_t packetCounter = 0;
    uint8_t virtualChannel;

    // fill frame buffer from multiple data packets
    while (isBridgeDataStarted())
    {
        // read one data packet
        std::unique_lock<std::mutex> lock(m_lock);
        m_cv.wait(lock, [this] { return ((m_cachedPacket != Control) || !m_commandActive); });

        try
        {
            uint8_t packetHeader[frameHeaderSize];
            uint16_t returnedSize = 0;
            if (readPacketStart(packetHeader, Data, !m_commandActive))
            {
                returnedSize = packetStartSize + m_port.receive(packetHeader + packetStartSize, frameHeaderSize - packetStartSize);
            }
            if (returnedSize < frameHeaderSize)
            {
                // no data packet available, continue while loop
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                if (returnedSize)
                {
                    LOG(DEBUG) << "Data read thread - Packet incomplete";
                }
#endif
                continue;
            }

            const auto bmPktType = serialToHost<uint8_t>(packetHeader + 0);
            if ((bmPktType & 0xF0) != DATA_FRAME_PACKET)
            {
                {
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                    LOG(DEBUG) << "Data read thread - Packet type error: 0x" << std::hex << static_cast<int>(bmPktType);
#endif
                    continue;
                }
            }
            const auto bChannel = serialToHost<uint8_t>(packetHeader + 1);
            const auto wLength  = serialToHost<uint16_t>(packetHeader + 4);

            const auto wCounter = serialToHost<uint16_t>(packetHeader + 2);
            if (wCounter != packetCounter)
            {
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                LOG(DEBUG) << "Data read thread - Packet loss, counter mismatch: received = 0x" << std::hex << wCounter << " , expected = 0x" << packetCounter;
#endif
                packetCounter = wCounter + 1;

                queueFrame(ErrorFrame::create(bChannel, DataError_FrameDropped));

                if (!(bmPktType & DATA_FRAME_FLAG_FIRST))
                {
                    // if this was a follow-up frame, discard the whole already received part
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                    LOG(DEBUG) << "Data read thread - discarding current frame";
#endif
                    dumpRemainder(wLength);
                    continue;
                }
            }
            else
            {
                packetCounter++;
            }

            if (!frame)
            {
                // try to dequeue frame to read data into
                frame = m_framePool.dequeueFrame();

                if (frame)
                {
                    // prepare frame buffer variables
                    bufBegin = frame->getBuffer();
                    bufEnd   = bufBegin + frame->getBufferSize();
                    buf      = bufBegin;
                }
            }
            const auto remainingSize = bufEnd - buf;
            if (!frame || (wLength > remainingSize))
            {
                dumpRemainder(wLength);

                if (frame == nullptr)
                {
                    queueFrame(ErrorFrame::create(VIRTUAL_CHANNEL_UNDEFINED, DataError_FramePoolDepleted));
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                    LOG(DEBUG) << "Data read thread - frame pool depleted, dumping packet";
#endif
                }
                else
                {
                    queueFrame(ErrorFrame::create(bChannel, DataError_FrameSizeExceeded));
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                    LOG(DEBUG) << "Data read thread - frame size exceeded, dumping packet";
#endif
                }
                continue;  // restart loop which will again try to dequeue frame buffer
            }

            if (bmPktType & DATA_FRAME_FLAG_FIRST)
            {
                if (setLocalTimestamp)
                {
                    epochTimestamp = getEpochTime();
                }
                virtualChannel = bChannel;

                if (buf != bufBegin)
                {
                    // we already started receiving a frame, but now a new frame starts
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                    LOG(DEBUG) << "Data read thread - previous frame incomplete";
#endif
                    buf = bufBegin;  // continue normally for a single/first packet
                }
            }

            if (m_port.receive(buf, wLength) != wLength)
            {
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                LOG(DEBUG) << "Data read thread - Packet payload incomplete";
#endif
                continue;
            }

            uint8_t packetCrc[packetCrcSize];
            if (m_port.receive(packetCrc, packetCrcSize) != packetCrcSize)
            {
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                LOG(DEBUG) << "Data read thread - Packet CRC incomplete";
#endif
                continue;
            }

            // we do not need to read from the serial port for this data packet any more
            lock.unlock();

            uint16_t crc = Crc16CcittFalse(packetHeader, frameHeaderSize);
            crc          = Crc16CcittFalse(buf, wLength, crc);
            crc          = Crc16CcittFalse(packetCrc, packetCrcSize, crc);
            if (crc)
            {
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                LOG(DEBUG) << "Data read thread - Packet CRC error: 0x" << std::hex << crc;
#endif
                continue;
            }

            if (!(bmPktType & DATA_FRAME_FLAG_FIRST))
            {
                if (buf == bufBegin)
                {
                    // we expected a new frame, but we received a follow-up packet
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                    LOG(DEBUG) << "Data read thread - discarding unexpected follow-up packet";
#endif
                    continue;  // don't do anything with the received packet and start over
                }

                if (virtualChannel != bChannel)
                {
#ifdef BRIDGE_SERIAL_DATA_DEBUG
                    LOG(DEBUG) << "Data read thread - Channel mismatch: received = 0x" << std::hex << static_cast<int>(bChannel) << " , expected = 0x" << static_cast<int>(virtualChannel);
#endif
                    continue;  // don't do anything with the received packet and start over
                }
            }

            buf += wLength;

            if (bmPktType & DATA_FRAME_FLAG_LAST)
            {
                if (bmPktType & DATA_FRAME_FLAG_TIMESTAMP)
                {
                    buf -= sizeof(epochTimestamp);
                    if (!setLocalTimestamp)
                    {
                        serialToHost(buf, epochTimestamp);
                    }
                }
                else if (!setLocalTimestamp)
                {
                    epochTimestamp = 0;
                }

                if (bmPktType & DATA_FRAME_FLAG_ERROR)
                {
                    uint32_t code;
                    serialToHost(buf - sizeof(code), code);
                    queueFrame(ErrorFrame::create(bChannel, code));
                    buf = bufBegin;
                }
                else
                {
                    frame->setDataOffset(0);
                    frame->setDataSize(static_cast<uint32_t>(buf - bufBegin));
                    frame->setVirtualChannel(virtualChannel);
                    frame->setTimestamp(epochTimestamp);

                    queueFrame(frame);
                    frame = nullptr;
                }

                continue;
            }
        }
        catch (const std::exception &e)
        {
            queueFrame(ErrorFrame::create(VIRTUAL_CHANNEL_UNDEFINED, DataError_LowLevelError));
            LOG(DEBUG) << "Data read thread - " << e.what();
        }
    }

    // when stopping the data transfer, check if we still have a frame buffer dequeued
    if (frame)
    {
        m_framePool.queueFrame(frame);
    }
}
