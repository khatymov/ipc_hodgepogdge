/*! \file buffer_rotator.h
 * \brief BufferRotator class interface.
 *
 * Class description.
 *
 */


#pragma once


#include <string>

#include "buffer.h"

/*! \class BufferRotator
 * \brief Some briefing
 */
class BufferRotator
{
    BufferRotator (const BufferRotator&) = delete;
    BufferRotator (BufferRotator&&) = delete;
    BufferRotator& operator = (const BufferRotator&) = delete;
    BufferRotator& operator = (BufferRotator &&) = delete;
public:

    //! \brief default constructor.
    BufferRotator(const BufferMode& mode, Buffer* buffer_ptr, const std::string& shared_memory_name);

    //! \brief default destructor.
    ~BufferRotator() = default;

    Buffer* get_buffer(const int index);

    void notify_buffer_is_ready(const BufferMode& mode, const int index);

private:

    //! List of private variables.
    SemaphoreHandler _semaphore_handler_0;
    SemaphoreHandler _semaphore_handler_1;
    Buffer* _buffer;
};
