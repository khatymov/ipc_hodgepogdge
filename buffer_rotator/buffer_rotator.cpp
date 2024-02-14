/*! \file buffer_rotator.cpp
 * \brief BufferRotator class implementation.
 */

#include "buffer_rotator.h"
#include <string>

using namespace std;

BufferRotator::BufferRotator(const BufferMode& mode, Buffer* buffer_ptr, const std::string& shared_memory_name)
    :buffer(buffer_ptr)
{
    if (BufferMode::write == mode)
    {
        new (&buffer[0]) Buffer();
        new (&buffer[1]) Buffer();
    }

    semaphore_handler[0] = SemaphoreHandler(shared_memory_name, 0);
    semaphore_handler[1] = SemaphoreHandler(shared_memory_name, 1);

}
