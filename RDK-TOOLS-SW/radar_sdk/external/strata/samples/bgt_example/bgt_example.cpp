/**
 * @copyright 2021 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include <common/cpp11/iterator.hpp>
#include <components/interfaces/IRadarRxs.hpp>
#include <modules/interfaces/IModuleRadar.hpp>
#include <platform/BoardManager.hpp>
#include <platform/interfaces/IFrameListener.hpp>
#include <universal/types/DataAvianSettings.h>

#include <components/interfaces/IRadarAvian.hpp>

#include <iomanip>
#include <iostream>
#include <thread>


using namespace std;


struct
{
    uint32_t burstAddress;
    uint32_t burstSize;
} meta_data = {
    0xFFC00060,
    4096,
};


const ICommandsAvian::Command default_doppler[] = {
    ICommandsAvian::Write(0x00, 0x1E827C),
    ICommandsAvian::Write(0x01, 0x140210),
    ICommandsAvian::Write(0x04, 0xE967FD),
    ICommandsAvian::Write(0x05, 0x0805B4),
    ICommandsAvian::Write(0x06, 0x102FFF),
    ICommandsAvian::Write(0x07, 0x010F00),
    ICommandsAvian::Write(0x08, 0x000000),
    ICommandsAvian::Write(0x09, 0x000000),
    ICommandsAvian::Write(0x0a, 0x000000),
    ICommandsAvian::Write(0x0b, 0x000BE0),
    ICommandsAvian::Write(0x0c, 0x000000),
    ICommandsAvian::Write(0x0d, 0x000000),
    ICommandsAvian::Write(0x0e, 0x000000),
    ICommandsAvian::Write(0x0f, 0x000000),
    ICommandsAvian::Write(0x10, 0x13FC51),
    ICommandsAvian::Write(0x11, 0x7FF41F),
    ICommandsAvian::Write(0x12, 0x703DEF),
    ICommandsAvian::Write(0x16, 0x000490),
    ICommandsAvian::Write(0x1d, 0x000480),
    ICommandsAvian::Write(0x24, 0x000480),
    ICommandsAvian::Write(0x2b, 0x000480),
    ICommandsAvian::Write(0x2c, 0x11BE0E),
    ICommandsAvian::Write(0x2d, 0x626C0A),
    ICommandsAvian::Write(0x2e, 0x03F000),
    ICommandsAvian::Write(0x2f, 0x787E1E),
    ICommandsAvian::Write(0x30, 0xE82498),
    ICommandsAvian::Write(0x31, 0x00009C),
    ICommandsAvian::Write(0x32, 0x000532),
    ICommandsAvian::Write(0x33, 0x000080),
    ICommandsAvian::Write(0x34, 0x000000),
    ICommandsAvian::Write(0x35, 0x000000),
    ICommandsAvian::Write(0x36, 0x000000),
    ICommandsAvian::Write(0x37, 0x26DB10),
    ICommandsAvian::Write(0x3f, 0x000100),
    ICommandsAvian::Write(0x47, 0x000100),
    ICommandsAvian::Write(0x4f, 0x000100),
    ICommandsAvian::Write(0x55, 0x000000),
    ICommandsAvian::Write(0x56, 0x000000),
    ICommandsAvian::Write(0x5b, 0x000000),

    ICommandsAvian::Write(0x00, 0x1E8271),
};


class FrameListener :
    public IFrameListener<>
{
public:
    void onNewFrame(IFrame *frame) override
    {
        auto code = frame->getStatusCode();
        if (code)
        {
            cout << "Frame error: 0x" << std::hex << code << std::endl;
            return;
        }

        const auto buf    = frame->getData();
        const auto length = frame->getDataSize();
        cout << "Frame size: " << std::dec << length << " - timestamp: " << std::fixed << std::setprecision(1) << (frame->getTimestamp() / 1000000.0) << " - id: " << std::hex << static_cast<int>(((buf[1] & 0x000F) << 8) + buf[0]) << " - address: " << reinterpret_cast<uintptr_t>(buf) << endl;
        frame->release();
        m_count++;
    }

    volatile int m_count = 0;
};

static FrameListener listener;


int main(int argc, char *argv[])
{
    cout << "Starting sensor_example..." << endl;

    cout << "1) trying to connect to a board ..." << endl;
    BoardManager boardMgr;
    boardMgr.enumerate();

    //auto board       = boardMgr.createBoardInstance();  // connect to any found board

    //auto board       = boardMgr.createBoardInstance(1);  // connect to the second found board (with index == 1)

    const uint16_t vid = 0x058b;
    const uint16_t pid = 0x0251;
    auto board         = boardMgr.createBoardInstance(vid, pid);  // connect to the first specified board forund

    //    const uint8_t uuid[UUID_LENGTH] = {};
    //    auto board         = boardMgr.createSpecificBoardInstance(uuid);  // connect to the first specified board forund

    cout << " ... Board Firmware Version: " << board->getVersionString() << endl;

    cout << "2) get the interface to a remote sensor running in the FW of the board..." << endl;

    auto avian = board->getComponent<IRadarAvian>(0);
    auto cmd   = avian->getICommandsAvian();

    auto data      = board->getIData();
    auto dataIndex = avian->getDataIndex();

    cout << "3) configure data readout and sensor..." << endl;

    // configure data readout
    IDataProperties_t properties;
    properties.format = DataFormat_Auto;

    const uint8_t flags   = 0;
    const uint8_t address = (meta_data.burstAddress & 0xFF);
    const uint16_t size   = (meta_data.burstSize & 0xFFFF);
    DataAvianSettings_t settings(flags, address, size);

    data->configure(dataIndex, &properties, &settings);

    // configure host frame buffers
    auto bridgeData                = board->getIBridge()->getIBridgeData();
    const uint32_t frameBuffersize = meta_data.burstSize * sizeof(uint16_t);  // size suits both packed and unpacked raw data
    bridgeData->setFrameBufferSize(frameBuffersize);
    bridgeData->setFrameQueueSize(20);

    const bool useCallback = false;
    if (useCallback)
    {
        bridgeData->registerListener(&listener);
    }

    // start data channel communication over protocol
    bridgeData->startStreaming();

    // start data readout statemachine
    data->start(dataIndex);

    // configure register map which also starts the sensor readout (data interface readout has to be started already!)
    cmd->execute(default_doppler);

    while (listener.m_count < 5)
    {
        if (!useCallback)
        {
            auto frame = board->getFrame();
            if (frame)
            {
                listener.onNewFrame(frame);
            }
        }
    }

    // stop data channel communication over protocol
    bridgeData->stopStreaming();

    // stop data acquisition (this will lead to a FIFO overflow in the sensor, which will stop its state machine)
    data->stop(dataIndex);

    cout << "4) Restart data readout..." << endl;

    // restart sensor again

    // start data channel communication over protocol
    bridgeData->startStreaming();

    // start data readout statemachine
    data->start(dataIndex);

    // configure register map which also starts the sensor readout (data interface readout has to be started already!)
    cmd->execute(default_doppler);

    while (listener.m_count < 10)
    {
        if (!useCallback)
        {
            auto frame = board->getFrame();
            if (frame)
            {
                listener.onNewFrame(frame);
            }
        }
    }

    // stop data channel communication over protocol
    bridgeData->stopStreaming();

    // stop data acquisition (this will lead to a FIFO overflow in the sensor, which will stop its state machine)
    data->stop(dataIndex);

    return 0;
}
