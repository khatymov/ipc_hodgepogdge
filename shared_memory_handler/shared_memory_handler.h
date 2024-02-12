/*! \file shared_memory_handler.h
 * \brief SharedMemoryHandler class interface.
 *
 * Class description.
 *
 */



#pragma once


#include <string>
#include <cassert>
#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <unistd.h>          /* ftruncate */
#include <thread>

#include "buffer.h"
#include "buffer_rotator.h"

/*! \class SharedMemoryHandler
 * \brief Handle access to shared memory
 */
class SharedMemoryHandler
{
public:

    //! \brief default constructor.    
    SharedMemoryHandler();


    //! \brief default destructor.
    ~SharedMemoryHandler() = default;

    //! \brief copy from source to target.
    static bool copy(const std::string_view& source_path, const std::string_view& target_path);

    static std::string get_unique_shared_name(const std::string_view& source_path, const std::string_view& target_path);

private:

    //! List of private variables.


};
