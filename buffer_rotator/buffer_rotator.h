/*! \file buffer_rotator.h
 * \brief BufferRotator class interface.
 *
 * Class description.
 *
 */


#pragma once

#include <semaphore.h>
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
    BufferRotator(Buffer* buffer_ptr, const std::string& shared_memory_name);

    //! \brief default destructor.
    ~BufferRotator() = default;


private:

    //! List of private variables.

};
