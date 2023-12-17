/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "FrameForwarder.hpp"


FrameForwarder::FrameForwarder(IFrameQueue *queue) :
    m_queue {queue},
    m_returned {true}
{
}

FrameForwarder::~FrameForwarder()
{
    stop();
    waitForThreadReturn();
}

void FrameForwarder::registerListener(IFrameListener<> *listener)
{
    if (listener == nullptr)
    {
        /// The queue must be stopped to unblock and stop the forwarding thread waiting for a new frame
        /// if the queue was started, restart it afterwards
        const bool wasQueueing = m_queue->stop();
        stop();
        FrameListenerCaller::registerListener(nullptr);
        if (wasQueueing)
        {
            m_queue->start();
        }
    }
    else
    {
        FrameListenerCaller::registerListener(listener);
    }
}

void FrameForwarder::start()
{
    if (!FrameListenerCaller::hasListener())
    {
        return;
    }

    if (!m_forwardingThread.joinable())
    {
        waitForThreadReturn();
        m_stop             = false;
        m_returned         = false;
        m_forwardingThread = std::thread(&FrameForwarder::forwardingThreadFunction, this);
    }
}

void FrameForwarder::stop()
{
    if (m_forwardingThread.joinable())
    {
        m_stop = true;
        m_forwardingThread.detach();
    }
}

void FrameForwarder::waitForThreadReturn()
{
    /// to avoid a blocking call to join(), we detach() the thread.
    /// to make sure there are no race conditions, before changing anything referenced by the potentially
    /// still running thread, we wait for the flag m_returned, which signals that the thread actually has exited.
    /// This allows also calling stop() from the thread context in a callback
    while (!m_returned)
    {
        std::this_thread::yield();
    }
}

void FrameForwarder::forwardingThreadFunction()
{
    do
    {
        auto *frame = m_queue->blockingDequeue();
        if (frame)
        {
            FrameListenerCaller::callListener(frame);
        }
    } while (!m_stop);

    m_returned = true;
}
