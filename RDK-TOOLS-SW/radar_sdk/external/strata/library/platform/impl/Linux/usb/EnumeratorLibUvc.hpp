/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <platform/interfaces/IEnumerator.hpp>

#include <memory>

//typedef std::shared_ptr<uvc_context_t *> ContextRef;


class BoardDescriptorLibUvc :
    public BoardDescriptor
{
public:
    BoardDescriptorLibUvc(const BoardData &data, const char name[], std::string &&devicePath) :
        BoardDescriptor {data, name},
        m_devicePath {std::move(devicePath)}
    {}

    std::shared_ptr<IBridge> createBridge() override;

private:
    bool isUsed();
//    ContextRef &m_context;
//    uvc_device_t *m_instance;
#if defined(TARGET_PLATFORM_ANDROID)
    uint32_t androidUsbDeviceFD;
#endif

    std::string m_devicePath;
};


class EnumeratorLibUvc :
    public IEnumerator
{
public:
    EnumeratorLibUvc();
    ~EnumeratorLibUvc();

    void enumerate(IEnumerationListener &listener, BoardData::const_iterator begin, BoardData::const_iterator end) override;
};
