/*! \file shared_memory_facade.cpp
 * \brief SharedMemoryFacade class implementation.
 */

#include "shared_memory_facade.h"
#include "buffer.h"
#include "definitions.h"

#include <csignal>
#include <fcntl.h> /* For O_* constants */
#include <stdexcept>
#include <sys/mman.h>

using namespace std;

SharedMemoryFacade::SharedMemoryFacade(const std::string_view& source_path, const std::string_view& target_path)
{
    _shared_object_name = get_unique_shared_name(source_path, target_path);
    // TODO delete
    std::cout << "_shared_object_name = " << _shared_object_name << std::endl;

    //    shm_unlink(_shared_object_name.c_str());
    // https://man7.org/linux/man-pages/man3/shm_open.3.html
    // TODO: change magic number
    // last argument: Permission-Bits https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
    const int file_descriptor = shm_open(_shared_object_name.data(), O_CREAT | O_EXCL | O_RDWR, 0666);

    _mmap_size = sizeof(Buffer);

    if (file_descriptor > -1)
    {
        std::cout << "READER." << std::endl;

        _is_writer = false;

        if (ftruncate(file_descriptor, _mmap_size) == -1)
        {
            std::cerr << "Reader: Error during truncate memory" << std::endl;
            throw std::runtime_error("Error during truncate memory");
        }
        // https://man7.org/linux/man-pages/man2/mmap.2.html
        _shared_mem_ptr = mmap(NULL, _mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0);
        if (_shared_mem_ptr == MAP_FAILED)
        {
            std::cerr << "Reader: Error mapping memory " << std::endl;
            throw std::runtime_error("Reader: Error mapping memory");
        }
    }
    else
    {
        _is_writer = true;
        std::cout << "WRITER." << std::endl;
        const int writer_file_descriptor = shm_open(_shared_object_name.data(), O_RDWR, S_IREAD);
        if (writer_file_descriptor > -1)
        {
            _shared_mem_ptr = mmap(NULL, _mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, writer_file_descriptor, 0);
            if (_shared_mem_ptr == MAP_FAILED)
            {
                std::cerr << "Client:Error mapping memory " << std::endl;
                throw std::runtime_error("Writer: Error mapping memory");
            }
        }
        else
        {
            std::cerr << "Writer: Error to open shared memory " << std::endl;
        }
    }
}

bool SharedMemoryFacade::is_writer() const
{
    return _is_writer;
}

void* SharedMemoryFacade::get_shared_mem_addr() const
{
    return _shared_mem_ptr;
}

SharedMemoryFacade::~SharedMemoryFacade()
{
    if (munmap(_shared_mem_ptr, _mmap_size) == -1)
    {
        if (_is_writer)
        {
            std::cerr << "Writer: Error unmapping memory " << std::endl;
        }
        else
        {
            std::cerr << "Reader: Error unmapping memory " << std::endl;
        }
    }

    if (!_is_writer)
    {
        shm_unlink(_shared_object_name.c_str());
    }
}
