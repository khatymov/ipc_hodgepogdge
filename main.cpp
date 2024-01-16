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
//TODO delete
#include <optional>
#include <variant>
int main(int argc, char* argv[])
{
    std::cout << sizeof(std::string_view) << std::endl;
    std::cout << sizeof(std::string) << std::endl;
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
    const std::string_view shared_memory_object{args[3]};

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


    const char* sem_name = "/semaphore_shared_memory";

    sem_t* semaphore;
    // Create or open the semaphore with an initial value of 1
    semaphore = sem_open(sem_name, O_CREAT | O_EXCL, 0666, 1);

    /*
     * Client - listener
     */
    if (semaphore == SEM_FAILED) {
        if (errno == EEXIST) {
            std::cout << "Client." << std::endl;
            int fd_write = shm_open(shared_memory_object.data(),  O_RDWR, S_IRUSR | S_IWUSR);
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

            } else {
                std::cerr << "Client: Error to open shared memory " << std::endl;
            }

        } else {
            std::cerr << "Client: Unable to open semaphore" << std::endl;
            return EXIT_FAILURE;
        }
    /*
     * SERVER - publisher
     */
    } else {
        std::cout << "Server." << std::endl;
        //обращаемся к общей памяти по имени, создаем общий объект в памяти если нету и разрешаем туда писать.
        //S_IRUSR and S_IWUSR -  read and write permission for the owner of the file
        //O_EXCL - If O_EXCL and O_CREAT are set, shm_open() fails if the shared memory object exists.
        int fd_read = shm_open(shared_memory_object.data(),  O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

        //TODO: delete magic -1
        if (fd_read != -1)
        {
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
//            if (read_file == nullptr)
//            {
//                std::cerr << "Unable to open read file " << "\n";
//                return EXIT_FAILURE;
//            }

            do
            {
//                buffer.size = std::fread(&buffer.data, sizeof(char), STORAGE_SIZE, read_file);
//                std::cout << "Server: msg from file " << buffer.data << std::endl;
                memcpy(addr, &buffer.data,  STORAGE_SIZE);
                buffer.size = 0;
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
        } else {
            std::cerr << "Server: Error to open shared memory " << std::endl;
        }

        shm_unlink(shared_memory_object.data());
    }

    // Close and unlink the semaphore when done
    sem_close(semaphore);
    sem_unlink(sem_name);

    return 0;
}