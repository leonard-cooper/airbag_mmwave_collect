/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "ProcessingLidar.hpp"

#include <common/Logger.hpp>


ProcessingLidar::ProcessingLidar() :
    m_rawPointCloud(m_rawPointQueue),
    m_processedPointCloud(m_processedPointQueue),
    m_processPointCloud(false)
{
}

void ProcessingLidar::onNewFrame(IFrame *frame)
{
    m_rawPointQueue.enqueue(frame);

    if (m_processPointCloud)
    {
        try
        {
            // try to dequeue Frame
            auto pointCloudBuffer = m_pool.dequeueFrame();

            // prepare frame buffer variables
            auto buf     = reinterpret_cast<float *>(pointCloudBuffer->getBuffer());
            auto bufSize = pointCloudBuffer->getBufferSize();


            // scope for using the current processing steps config to protect it from being changed while used here
            {
                std::lock_guard<std::mutex> lock(m_lock);


                // do processing according to config
            }
            // do further processing when config is not relevant any more


            m_processedPointQueue.enqueue(pointCloudBuffer);
        }
        catch (...)
        {
            LOG(DEBUG) << "ProcessingLidar::onNewFrame() - Buffer pool depleted, frame will not be processed!";
        }
    }
}

void ProcessingLidar::registerRawPointCloudListener(IFrameListener *listener)
{
    m_rawPointCloud.registerListener(listener);
}

void ProcessingLidar::registerProcessedPointCloudListener(IFrameListener *listener)
{
    m_processedPointCloud.registerListener(listener);
    m_processPointCloud = true;
}

void ProcessingLidar::configure(const LidarProcessingSteps *stages)
{
    std::lock_guard<std::mutex> lock(m_lock);

    m_steps = *stages;

    const uint32_t pclSize = 32 * 120 * 4 * 4;
    m_pool.setFrameBufferSize(pclSize);
    m_pool.setFrameCount(16);
}

const LidarProcessingSteps *ProcessingLidar::getConfig()
{
    return &m_steps;
}
