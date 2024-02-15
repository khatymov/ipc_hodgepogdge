/*! \file shared_memory_handler.cpp
 * \brief SharedMemoryHandler class implementation.
 */

#include "shared_memory_handler.h"

#include "file_handler.h"

using namespace std;

SharedMemoryHandler::SharedMemoryHandler()
{
}

bool SharedMemoryHandler::copy(const string_view& source_path, const string_view& target_path)
{
    const auto shared_name = get_unique_shared_name(source_path, target_path);

    // https://man7.org/linux/man-pages/man3/shm_open.3.html
    // TODO: change magic number
    const int file_descriptor = shm_open(shared_name.data(), O_CREAT | O_EXCL | O_RDWR | O_TRUNC, 0600);

    // __________________________________________________________________________________________
    // | buffer_size(8 byte) | _buffer[STORAGE_SIZE] | buffer_size(8 byte) | _buffer[STORAGE_SIZE] |
    // ------------------------------------------------------------------------------------------
    const size_t mmap_size = STORAGE_SIZE * 2 + sizeof(size_t) * 2;

    /*
     * SERVER - publisher
     */
    if (file_descriptor > -1)
    {
        std::cout << "Server side." << std::endl;

        if (ftruncate(file_descriptor, mmap_size) == -1)
        {
            std::cerr << "Server: Error during truncate memory" << std::endl;
            return EXIT_FAILURE;
        }

        // https://man7.org/linux/man-pages/man2/mmap.2.html
        void* map_addr = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0);
        if (map_addr == MAP_FAILED)
        {
            std::cerr << "Server: Error mapping memory " << std::endl;
            return EXIT_FAILURE;
        }

        FileHandler read_file(source_path.data(), "rb");
        Buffer* buffer_ptr = static_cast<Buffer*>(map_addr);

        BufferRotator bufferRotator(BufferMode::ready_to_read, buffer_ptr, shared_name.data());

        uint index = 0;
        do
        {

            std::cout << "Server index = " << index << std::endl;
            buffer_ptr = bufferRotator.get_buffer(index);

            read_file.fread(buffer_ptr);

            const bool everything_done = buffer_ptr->size == 0;

            std::cout << "Server bufferRotator.notify_buffer_is_ready(BufferMode::write, index);" << std::endl;
            bufferRotator.notify_buffer_is_ready(BufferMode::ready_to_read, index);

            std::cout << "Server bufferRotator.notify_buffer_is_ready(BufferMode::read, index);" << std::endl;
            bufferRotator.notify_buffer_is_ready(BufferMode::wait, index);

//            if (index == 0)
//            {
//                index++;
//            }
//            else {
//                index--;
//            }

            if (everything_done)
                break;

        } while (true);

        if (munmap(map_addr, mmap_size) == -1)
        {
            std::cerr << "Server: Error unmapping memory " << std::endl;
        }

        shm_unlink(shared_name.data());
        std::cout << "Server finished" << std::endl;
        return true;
    }
    /*
     * Client - listener
     */
    else
    {
        std::cout << "Client side." << std::endl;
        // TODO: handle the case when second client tries to open to the same shared memory
        int fd_write = shm_open(shared_name.data(), O_RDWR, 0);
        if (fd_write != -1)
        {

            void* mmap_addr = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_write, 0);
            if (mmap_addr == MAP_FAILED)
            {
                std::cerr << "Client:Error mapping memory " << std::endl;
                return EXIT_FAILURE;
            }

            FileHandler write_file(target_path.data(), "w");

            Buffer* buffer_ptr = static_cast<Buffer*>(mmap_addr);
            BufferRotator bufferRotator(BufferMode::wait, buffer_ptr, shared_name.data());

            uint i = 0;
            uint index = 0;
            while (true)
            {
                std::cout << "Client index = " << index << std::endl;
                std::cout << "Client bufferRotator.notify_buffer_is_ready(BufferMode::read, index);" << std::endl;
                bufferRotator.notify_buffer_is_ready(BufferMode::wait, index);

                buffer_ptr = bufferRotator.get_buffer(index);
                std::cout << "Client data: " << buffer_ptr->data << std::endl;
                write_file.fwrite(buffer_ptr);
                auto cur_size = buffer_ptr->size;

                std::cout << "Client iteration: " << i++ << std::endl;

                std::cout << "Client bufferRotator.notify_buffer_is_ready(BufferMode::write, index);" << std::endl;
                bufferRotator.notify_buffer_is_ready(BufferMode::ready_to_read, index);

//                if (index == 0)
//                {
//                    index++;
//                }
//                else {
//                    index--;
//                }
                if (cur_size == 0)
                    break;
            }

            // Unmap the memory when done
            if (munmap(mmap_addr, mmap_size) == -1)
            {
                std::cerr << "Client: Error unmapping memory " << std::endl;
            }
            std::cout << "Client finished" << std::endl;
        }
        else
        {
            std::cerr << "Client: Error to open shared memory " << std::endl;
        }
    }

    const auto cmd = std::string("diff ") + source_path.data() + " " + target_path.data() + std::string("| exit $(wc -l)");
    return (std::system(cmd.c_str()) == 0);
}

std::string SharedMemoryHandler::get_unique_shared_name(const string_view& source_path, const string_view& target_path)
{
    std::string shared_name = std::string(source_path) + std::string(target_path);
    std::replace(shared_name.begin(), shared_name.end(), '/', '_');
    shared_name = '/' + shared_name;
    return shared_name;
}
