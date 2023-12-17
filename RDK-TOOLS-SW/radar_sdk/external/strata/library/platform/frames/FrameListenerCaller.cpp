/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "FrameListenerCaller.hpp"

template <typename FrameType>
FrameListenerCaller<FrameType>::FrameListenerCaller() :
    m_listener {nullptr}
{
}

template <typename FrameType>
void FrameListenerCaller<FrameType>::registerListener(IFrameListener<FrameType> *listener)
{
    //Locking the mutex makes sure the function only returns if any call on a previous listener is completed
    std::lock_guard<std::mutex> lock(m_listenerMutex);
    m_listener = listener;
}

template <typename FrameType>
void FrameListenerCaller<FrameType>::callListener(FrameType *frame)
{
    //Locking the mutex makes sure the pointer is not changed while it is inside a call
    std::lock_guard<std::mutex> lock(m_listenerMutex);
    if (m_listener)
    {
        m_listener->onNewFrame(frame);
    }
    else
    {
        frame->release();
    }
}


/**
 * This avoids having to have the complete implementation in the header file,
 * since the linker of an external executable needs to find the implementation
 * of the class. It has to be after the member declaration, since some compilers
 * otherwise would not emit the symbols.
 */
template class FrameListenerCaller<IFrame>;
