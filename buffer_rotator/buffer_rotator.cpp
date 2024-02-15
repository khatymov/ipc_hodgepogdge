/*! \file buffer_rotator.cpp
 * \brief BufferRotator class implementation.
 */

#include "buffer_rotator.h"
#include <string>

using namespace std;

BufferRotator::BufferRotator(const BufferMode& mode, Buffer* buffer_ptr, const std::string& shared_memory_name)
    :_buffer(buffer_ptr)
    ,_semaphore_handler_0(SemaphoreHandler(shared_memory_name, 0))
    ,_semaphore_handler_1(SemaphoreHandler(shared_memory_name, 1))
{
    if (BufferMode::ready_to_read == mode)
    {
        new (&_buffer[0]) Buffer();
        new (&_buffer[1]) Buffer();
    }
}

Buffer* BufferRotator::get_buffer(const int index)
{
    return &_buffer[index];
}

void BufferRotator::notify_buffer_is_ready(const BufferMode& mode, const int index)
{
    if (index == 0)
    {
        _semaphore_handler_0.do_ping_pong(mode);
    }
    else if (index == 1)
    {
        _semaphore_handler_1.do_ping_pong(mode);
    }
}
