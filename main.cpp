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
#include <thread>

#include <semaphore.h>
#include <cstdlib>
#include <functional>
namespace fs = std::filesystem;

#define STORAGE_SIZE 24

class Buffer {
public:

    Buffer() = default;
    ~Buffer()
    {
        size = 0;
    }

    size_t size = 0;
    char data[STORAGE_SIZE];
};

char semaphore_path_1[] = "/tmp_named_sem_1";
char semaphore_path_2[] = "/tmp_named_sem_2";

enum Label
{
    Server,
    Client
};

class BufferRotator
{
public:


private:


};

class SemaphoreHandler{
public:
    SemaphoreHandler(const Label label)
    {
        std::cout << "SemaphoreHandler" << std::endl;
        if (label == Label::Server)
        {
            _shared_semaphore = sem_open(semaphore_path_1, O_CREAT, 0644, 0);
            std::cout << "Server, create semaphore" << std::endl;
        }
        // means client
        else
        {
            // Где гарантия, что клиент первым не попытается окрыть семафор, которого не существует
            _shared_semaphore = sem_open(semaphore_path_1, 0, 0644, 0);
            std::cout << "Client, use existing semaphore" << std::endl;
        }

        if(_shared_semaphore == (void*) -1){
            std::cerr << "sem_open failure"  << std::endl;
            exit(EXIT_FAILURE);
        }
//        int value;
//        sem_getvalue(_shared_semaphore, &value);
//        std::cout << "sem_getvalue = " << value << std::endl;
        std::cout << "errno = " << errno << "Description: " << strerror(errno) << std::endl;
        std::cout << "SEM_FAILED = " << SEM_FAILED << std::endl;
        std::cout << "EEXIST = " << EEXIST << std::endl;
    }

    ~SemaphoreHandler()
    {
        std::cout << "~SemaphoreHandler" << std::endl;
        sem_close(_shared_semaphore);
        sem_unlink(semaphore_path_1);
    }

    auto post()
    {
//        int value;
//        sem_getvalue(_shared_semaphore, &value);
//        std::cout << "sem_getvalue = " << value << std::endl;
        return sem_post(_shared_semaphore);
    }

    auto wait()
    {
//        int value;
//        sem_getvalue(_shared_semaphore, &value);
//        std::cout << "sem_getvalue = " << value << std::endl;
        return sem_wait(_shared_semaphore);
    }

private:
    sem_t * _shared_semaphore;
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
    auto shared_name_hash = std::hash<std::string>()(shared_name);
    shared_name = "/" + std::to_string(shared_name_hash);

    std::cout << "Shared object name (hash): " << shared_name << std::endl;
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
    int fd_read = shm_open(shared_name.data(),  O_CREAT | O_EXCL | O_RDWR | O_TRUNC, 0600);

    std::cout << "fd_read: " << fd_read << std::endl;
    /*
     * SERVER - publisher
     */
    if (fd_read > -1)
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
        void *addr = mmap(NULL, STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_read, 0);
        if (addr == MAP_FAILED)
        {
            std::cerr << "Server: Error mapping memory " << std::endl;
            return EXIT_FAILURE;
        }
        //открываем файл чтение
        //do
        //читаем STORAGE_SIZE байт в буффер из файла
        //записываем в общую память
        //!!! продумать механизм синхронизации
        // пока кол-во прочитанных байт != 0
        // The abbreviation  "rb"  includes the representation of binary mode, as denoted by  b  code.
        std::FILE* read_file = std::fopen(source_path.data(), "rb");
//        Buffer buffers[2];
        Buffer* buffer_ptr = static_cast<Buffer*>(addr);
        new (&buffer_ptr[0]) Buffer();
        new (&buffer_ptr[1]) Buffer();
        std::cout << "Server: Initial size of data: " << buffer_ptr->size << " | Data: " << buffer_ptr->data << std::endl;
//        SemaphoreHandler semaphore_handler(Label::Server);
        sem_t* _shared_semaphore_1 = sem_open(semaphore_path_1, O_CREAT, 0644, 0);
        sem_t* _shared_semaphore_2 = sem_open(semaphore_path_2, O_CREAT, 0644, 0);

        std::cout << "Server, create semaphore" << std::endl;

        if (_shared_semaphore_1 == (void*) -1)
        {
            std::cerr << "sem_open 1 failure"  << std::endl;
            exit(EXIT_FAILURE);
        }

        if (_shared_semaphore_2 == (void*) -1)
        {
            std::cerr << "sem_open 2 failure"  << std::endl;
            exit(EXIT_FAILURE);
        }

        do
        {
            if (sem_wait(_shared_semaphore_1) == -1)
            {
                std::cerr << "S if (sem_wait(_shared_semaphore_1) == -1)"  << std::endl;
                exit(EXIT_FAILURE);
            }

            buffer_ptr[0].size = std::fread(&buffer_ptr[0].data, sizeof(char), STORAGE_SIZE, read_file);
            buffer_ptr[1].size = std::fread(&buffer_ptr[1].data, sizeof(char), STORAGE_SIZE, read_file);
            const bool everything_done = buffer_ptr[0].size == 0 or buffer_ptr[1].size == 0;

            if (sem_post(_shared_semaphore_2) == -1)
            {
                std::cerr << "S if (sem_post(_shared_semaphore_1) == -1)"  << std::endl;
                exit(EXIT_FAILURE);
            }

            if (everything_done)
                break;

        } while (true);

        sem_close(_shared_semaphore_1);
        sem_close(_shared_semaphore_2);

        fclose(read_file);
        // Unmap the memory when done
        if (munmap(addr, STORAGE_SIZE) == -1) {
            std::cerr << "Server: Error unmapping memory " << std::endl;
        }

        shm_unlink(shared_name.data());
    }
    /*
     * Client - listener
     */
    else {
        std::cout << "Client side." << std::endl;
        //TODO: handle the case when second client tries to open to the same shared memory
        int fd_write = shm_open(shared_name.data(),  O_RDWR, 0);
        if (fd_write != -1)
        {
            // ссылаемся на указанную общую память-на адресное пространство процесса
            // ч/з дескриптор
            void *addr = mmap(NULL, STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_write, 0);
            if (addr == MAP_FAILED)
            {
                std::cerr << "Client:Error mapping memory " << std::endl;
                return EXIT_FAILURE;
            }

            Buffer* buffer_ptr = static_cast<Buffer*>(addr);
//            Buffer buffer_ptr[2];
            std::cout << "Client: Initial size of data: " << buffer_ptr->size << " | Data: " << buffer_ptr->data << std::endl;
//            SemaphoreHandler semaphore_handler(Label::Client);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            sem_t* _shared_semaphore_1 = sem_open(semaphore_path_1, 0, 0644, 0);
            sem_t* _shared_semaphore_2 = sem_open(semaphore_path_2, 0, 0644, 0);
            std::cout << "Client, use existing semaphore" << std::endl;
            if (_shared_semaphore_1 == (void*) -1)
            {
                std::cerr << "sem_open 1 failure"  << std::endl;
                exit(EXIT_FAILURE);
            }

            if (_shared_semaphore_2 == (void*) -1)
            {
                std::cerr << "sem_open 2 failure"  << std::endl;
                exit(EXIT_FAILURE);
            }

            std::FILE* write_file = std::fopen(target_path.data(), "w");

            while (true)
            {
                if (sem_post(_shared_semaphore_1) == -1)
                {
                    std::cerr << "C if (sem_post(_shared_semaphore_1) == -1)"  << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (sem_wait(_shared_semaphore_2) == -1)
                {
                    std::cerr << "C if (sem_wait(_shared_semaphore_2) == -1)"  << std::endl;
                    exit(EXIT_FAILURE);
                }

                auto cur_size_1 = buffer_ptr[0].size;

                std::cout << "Client, first buffer_ptr:" << std::endl;
                for (int i = 0; i < buffer_ptr[0].size; i++)
                    std::cout << buffer_ptr[0].data[i];

                fwrite(buffer_ptr[0].data, sizeof(char), buffer_ptr[0].size, write_file);

                if (cur_size_1 == 0)
                    break;

                auto cur_size_2 = buffer_ptr[1].size;
                std::cout << "Client, second buffer_ptr:" << std::endl;
                for (int i = 0; i < buffer_ptr[1].size; i++)
                    std::cout << buffer_ptr[1].data[i];

                fwrite(buffer_ptr[1].data, sizeof(char), buffer_ptr[1].size, write_file);

                if (cur_size_2 == 0)
                    break;
            }

            sem_close(_shared_semaphore_1);
            sem_close(_shared_semaphore_2);
            sem_unlink(semaphore_path_1);
            sem_unlink(semaphore_path_2);
            fclose(write_file);
            // Unmap the memory when done
            if (munmap(addr, STORAGE_SIZE) == -1) {
                std::cerr << "Client: Error unmapping memory " << std::endl;
            }


            const auto cmd = std::string("diff ") + source_path.data() + " " + target_path.data() + std::string("| exit $(wc -l)");
            assert(std::system(cmd.c_str()) == 0);

        } else {
            std::cerr << "Client: Error to open shared memory " << std::endl;
        }
    }



    return 0;
}