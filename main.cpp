/*! \file main.cpp
 * \brief Entry point.
 */

#include <cassert>
#include <filesystem>
#include <iostream>
#include <span>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <unistd.h>          /* ftruncate */

#include <semaphore.h>
#include <cstdlib>
namespace fs = std::filesystem;

#define STORAGE_SIZE 24

struct Buffer {
    size_t size = 0;
    char data[STORAGE_SIZE];
};

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: ./cpp_course path_to_source path_to_target." << std::endl;
        return EXIT_FAILURE;
    }

    // https://en.cppreference.com/w/cpp/container/span
    // warning: 'do not use pointer arithmetic'
    const auto args = std::span(argv, size_t(argc));
    const std::string_view source_path{args[1]};
    const std::string_view target_path{args[2]};
    //should be unique
    std::string shared_name = std::string(source_path) + std::string(target_path);
    shared_name.erase(std::remove(shared_name.begin(), shared_name.end(), '/'), shared_name.end());
    shared_name.erase(std::remove(shared_name.begin(), shared_name.end(), '.'), shared_name.end());
    shared_name = "/" + shared_name;

    if (!fs::exists(source_path.data()))
    {
        std::cout << "Source file doesn't exist." << std::endl;
        return EXIT_FAILURE;
    }
    // do not overwrite destination file by default.
    if (source_path == target_path)
    {
        std::cout << "Source and target files are the same." << std::endl;
        return EXIT_SUCCESS;
    }

    // https://man7.org/linux/man-pages/man3/shm_open.3.html
    //обращаемся к общей памяти по имени, создаем общий объект в памяти если нету и разрешаем туда писать.
    //S_IRUSR and S_IWUSR -  read and write permission for the owner of the file
    //O_EXCL - If O_EXCL and O_CREAT are set, shm_open() fails if the shared memory object exists.
    int fd_read = shm_open(shared_name.data(),  O_RDWR | O_CREAT | O_EXCL, 0777);

    /*
     * SERVER - publisher
     */
    if (fd_read != -1)
    {
        std::cout << "Server side." << std::endl;
        // устанавливаем размер общей памяти на основе page-aligned (4KB)
        // extend shared memory object as by default it's initialized
        //  with size 0
        int res = ftruncate(fd_read, STORAGE_SIZE);
        if (res == -1)
        {
            std::cerr << "Server: Error during truncate memory" << std::endl;
            return EXIT_FAILURE;
        }

        // ссылаемся на указанную общую память-на адресное пространство процесса
        // ч/з дескриптор
        //https://man7.org/linux/man-pages/man2/mmap.2.html
        void *addr = mmap(NULL, STORAGE_SIZE, PROT_WRITE, MAP_SHARED, fd_read, 0);
        if (addr == MAP_FAILED)
        {
            std::cerr << "Server: Error mapping memory " << std::endl;
            return EXIT_FAILURE;
        }
        //открываем файл чтение
        //do
        //читаем STORAGE_SIZE байт в буффер из файла
        //записываем в общую память
        //прочесть Use 'placement' new  ??memcpy(buffer, addr, STORAGE_SIZE)??
        //!!! продумать механизм синхронизации
        // пока кол-во прочитанных байт != 0
        // The abbreviation  "rb"  includes the representation of binary mode, as denoted by  b  code.
        //            std::FILE* read_file = std::fopen(source_path.data(), "rb");
        Buffer buffer;
        buffer.size = 24;
        memcpy(buffer.data, "Hello World             ", STORAGE_SIZE);
        do
        {
            memcpy(addr, &buffer.data,  STORAGE_SIZE);
            buffer.size = 1;
            const bool everything_done = buffer.size == 0;
            if (everything_done)
            {
                break;
            }
        } while (true);

        //            fclose(read_file);
        // Unmap the memory when done
        if (munmap(addr, STORAGE_SIZE) == -1) {
            std::cerr << "Server: Error unmapping memory " << std::endl;
        }
    }
    /*
     * Client - listener
     */
    else {
        std::cout << "Client side." << std::endl;
        int fd_write = shm_open(shared_name.data(),  O_RDWR, S_IRUSR | S_IWUSR);
        if (fd_write != -1)
        {
            // ссылаемся на указанную общую память-на адресное пространство процесса
            // ч/з дескриптор
            void *addr = mmap(NULL, STORAGE_SIZE, PROT_READ, MAP_SHARED, fd_write, 0);
            if (addr == MAP_FAILED)
            {
                std::cerr << "Client:Error mapping memory " << std::endl;
                return EXIT_FAILURE;
            }

            usleep(1000);
            //                char* buf = static_cast<char *>(addr);
            Buffer buffer;
            memcpy(&buffer.data, addr,  STORAGE_SIZE);
            std::cout << "Client: msg" << buffer.data << std::endl;
            // Unmap the memory when done
            if (munmap(addr, STORAGE_SIZE) == -1) {
                std::cerr << "Server: Error unmapping memory " << std::endl;
            }
        } else {
            std::cerr << "Client: Error to open shared memory " << std::endl;
        }
    }

    shm_unlink(shared_name.data());

    return 0;
}