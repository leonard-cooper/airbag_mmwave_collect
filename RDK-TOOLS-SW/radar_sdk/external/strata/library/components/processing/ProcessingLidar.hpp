/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/interfaces/IProcessingLidar.hpp>
#include <platform/frames/FrameForwarder.hpp>
#include <platform/frames/FramePool.hpp>

#include <mutex>


class ProcessingLidar :
    public IProcessingLidar
{
public:
    ProcessingLidar();
    virtual ~ProcessingLidar() = default;

    // IFrame Listener implementation
    void onNewFrame(IFrame *frame);

    void registerRawPointCloudListener(IFrameListener *listener);
    void registerProcessedPointCloudListener(IFrameListener *listener);

    void configure(const LidarProcessingSteps *stages);
    const LidarProcessingSteps *getConfig();

private:
    FramePool m_pool;
    FrameQueue m_rawPointQueue;
    FrameForwarder m_rawPointCloud;
    FrameQueue m_processedPointQueue;
    FrameForwarder m_processedPointCloud;
    bool m_processPointCloud;

    std::mutex m_lock;
    LidarProcessingSteps m_steps;
};
