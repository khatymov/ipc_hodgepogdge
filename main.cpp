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

#include <queue>

#include <semaphore.h>
#include <cstdlib>
#include <functional>
namespace fs = std::filesystem;

#define STORAGE_SIZE 4096

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

char semaphore_path_0_0[] = "/tmp_sem_0_0_aaa";
char semaphore_path_0_1[] = "/tmp_sem_0_1_aaa";
char semaphore_path_1_0[] = "/tmp_sem_1_0_aaa";
char semaphore_path_1_1[] = "/tmp_sem_1_1_aaa";
/*! \class BufferMode
 * \brief Choose current mode: write or read
 */
enum class BufferMode : int
{
    read,
    write
};


//using BufferIndex = std::pair<Buffer*,uint8_t>;

struct BufferIndex
{
    BufferIndex() = default;
    BufferIndex(Buffer* buffer_, uint8_t buffer_index_)
        :buffer(buffer_)
        ,buffer_index(buffer_index_)
    {}
    Buffer* buffer;
    uint8_t buffer_index;
};

class BufferRotator
{
    BufferRotator (const BufferRotator&) = delete;
    BufferRotator (BufferRotator&&) = delete;
    BufferRotator& operator = (const BufferRotator&) = delete;
    BufferRotator& operator = (BufferRotator &&) = delete;
public:
    BufferRotator(BufferMode mode, Buffer* shared_buffers_ptr, sem_t* sem_0_0, sem_t* sem_0_1, sem_t* sem_1_0, sem_t* sem_1_1)
        : _sem_0_0(sem_0_0)
        , _sem_0_1(sem_0_1)
        , _sem_1_0(sem_1_0)
        , _sem_1_1(sem_1_1)
        , _mmap_buffer(shared_buffers_ptr)
    // Почему я здесь принимаю указатель на буфер, а не просто адрес, полученный с помощью mmap. Ответ - нам нужно,
    // чтобы new (&buffer_ptr[i]) Buffer() вызывался единожды, и отвественный за это - Writer. Поэтому в BufferRotator
    // мы получаем готовый указатель, когда new placement уже произошло
    {
        if (BufferMode::write == mode)
        {
            new (&_mmap_buffer[0]) Buffer();
            new (&_mmap_buffer[1]) Buffer();
        }
    }

    //! \brief get buffer according to buffer mode (from  _read_buffers or _write_buffers)
    Buffer* get_buffer(const BufferMode mode, const uint index) const
    {
        if (mode == BufferMode::write)
        {
            return &_mmap_buffer[index];
        }

        if (mode == BufferMode::read)
        {
            if (index == 0)
            {
                sem_post(_sem_0_0);
                sem_wait(_sem_0_1);
                return &_mmap_buffer[index];
            }

            if (index == 1)
            {
                sem_post(_sem_1_0);
                sem_wait(_sem_1_1);
                return &_mmap_buffer[index];
            }
        }

        return nullptr;
    }


    void notify(const BufferMode mode, const uint index) const
    {
        if (mode == BufferMode::read)
        {
            if (index == 0)
            {
                sem_wait(_sem_0_0);
                sem_post(_sem_0_1);
                return;
            }

            if (index == 1)
            {
                sem_wait(_sem_1_0);
                sem_post(_sem_1_1);
                return;
            }
        }
    }


    ~BufferRotator()
    {

    }

private:
    sem_t* _sem_0_0;
    sem_t* _sem_0_1;
    sem_t* _sem_1_0;
    sem_t* _sem_1_1;


    Buffer* _mmap_buffer;
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
        int res = ftruncate(fd_read, STORAGE_SIZE * 2 + 16);
        if (res == -1)
        {
            std::cerr << "Server: Error during truncate memory" << std::endl;
            return EXIT_FAILURE;
        }

        // ссылаемся на указанную общую память-на адресное пространство процесса
        // ч/з дескриптор
        //https://man7.org/linux/man-pages/man2/mmap.2.html
        void *addr = mmap(NULL, STORAGE_SIZE * 2 + 16, PROT_READ | PROT_WRITE, MAP_SHARED, fd_read, 0);
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

        std::cout << "Server: Initial size of data: " << buffer_ptr->size << " | Data: " << buffer_ptr->data << std::endl;
//        SemaphoreHandler semaphore_handler(Label::Server);
        sem_t* _semaphore_0_0 = sem_open(semaphore_path_0_0, O_CREAT, 0644, 0);
        sem_t* _semaphore_0_1 = sem_open(semaphore_path_0_1, O_CREAT, 0644, 0);
        sem_t* _semaphore_1_0 = sem_open(semaphore_path_1_0, O_CREAT, 0644, 0);
        sem_t* _semaphore_1_1 = sem_open(semaphore_path_1_1, O_CREAT, 0644, 0);

        std::cout << "Server, create semaphore" << std::endl;

        if (_semaphore_0_0 == (void*) -1)
        {
            std::cerr << "sem_open 2 failure"  << std::endl;
            exit(EXIT_FAILURE);
        }

        if (_semaphore_0_1 == (void*) -1)
        {
            std::cerr << "sem_open 1 failure"  << std::endl;
            exit(EXIT_FAILURE);
        }

        if (_semaphore_1_0 == (void*) -1)
        {
            std::cerr << "sem_open 2 failure"  << std::endl;
            exit(EXIT_FAILURE);
        }

        if (_semaphore_1_1 == (void*) -1)
        {
            std::cerr << "sem_open 1 failure"  << std::endl;
            exit(EXIT_FAILURE);
        }



        BufferRotator bufferRotator(BufferMode::write, buffer_ptr, _semaphore_0_0, _semaphore_0_1, _semaphore_1_0, _semaphore_1_1);
        Buffer* buffer;
        uint index = 0;
        do
        {
            buffer = bufferRotator.get_buffer(BufferMode::write, index);

            buffer->size = std::fread(&buffer->data, sizeof(char), STORAGE_SIZE, read_file);

            const bool everything_done = buffer->size == 0;

            bufferRotator.notify(BufferMode::read, index);

            if (index == 0)
            {
                index++;
            }
            else {
                index--;
            }

            if (everything_done)
                break;

        } while (true);

        sem_close(_semaphore_0_0);
        sem_close(_semaphore_0_1);
        sem_close(_semaphore_1_0);
        sem_close(_semaphore_1_1);


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
            void *addr = mmap(NULL, STORAGE_SIZE * 2 + 16, PROT_READ | PROT_WRITE, MAP_SHARED, fd_write, 0);
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


            sem_t* _semaphore_0_0 = sem_open(semaphore_path_0_0, 0, 0644, 0);
            sem_t* _semaphore_0_1 = sem_open(semaphore_path_0_1, 0, 0644, 0);
            sem_t* _semaphore_1_0 = sem_open(semaphore_path_1_0, 0, 0644, 0);
            sem_t* _semaphore_1_1 = sem_open(semaphore_path_1_1, 0, 0644, 0);

            while (_semaphore_0_0 == nullptr)
            {
                std::cout << "Client, can't attach to existing semaphore 1, try again" << std::endl;
                std::this_thread::yield();
                _semaphore_0_0 = sem_open(semaphore_path_0_0, 0, 0644, 0);
            }
            if (_semaphore_0_0 == (void*) -1)
            {
                std::cerr << "sem_open 1 failure"  << std::endl;
                exit(EXIT_FAILURE);
            }


            while (_semaphore_0_1 == nullptr)
            {
                std::cout << "Client, can't attach to existing semaphore 1, try again" << std::endl;
                std::this_thread::yield();
                _semaphore_0_1 = sem_open(semaphore_path_0_1, 0, 0644, 0);
            }
            if (_semaphore_0_1 == (void*) -1)
            {
                std::cerr << "sem_open 1 failure"  << std::endl;
                exit(EXIT_FAILURE);
            }


            while (_semaphore_1_0 == nullptr)
            {
                std::cout << "Client, can't attach to existing semaphore 1, try again" << std::endl;
                std::this_thread::yield();
                _semaphore_1_0 = sem_open(semaphore_path_1_0, 0, 0644, 0);
            }
            if (_semaphore_1_0 == (void*) -1)
            {
                std::cerr << "sem_open 1 failure"  << std::endl;
                exit(EXIT_FAILURE);
            }

            while (_semaphore_1_1 == nullptr)
            {
                std::cout << "Client, can't attach to existing semaphore 1, try again" << std::endl;
                std::this_thread::yield();
                _semaphore_1_1 = sem_open(semaphore_path_1_1, 0, 0644, 0);
            }
            if (_semaphore_1_1 == (void*) -1)
            {
                std::cerr << "sem_open 1 failure"  << std::endl;
                exit(EXIT_FAILURE);
            }

            std::cout << "Client, use existing semaphores" << std::endl;

            BufferRotator bufferRotator(BufferMode::read, buffer_ptr, _semaphore_0_0, _semaphore_0_1, _semaphore_1_0, _semaphore_1_1);

            std::FILE* write_file = std::fopen(target_path.data(), "w");


            std::cout << "Client, Start processing" << std::endl;
            Buffer* buffer;
            uint i = 0;
            uint index = 0;
            while (true)
            {
                buffer = bufferRotator.get_buffer(BufferMode::read, index);

                auto cur_size = buffer->size;

                std::cout << "Client iteration: " << i++ << std::endl;
//                for (int i = 0; i < buffer->size; i++)
//                    std::cout << buffer->data[i];

                fwrite(buffer->data, sizeof(char), buffer->size, write_file);

                bufferRotator.notify(BufferMode::write, index);

                if (index == 0)
                {
                    index++;
                }
                else {
                    index--;
                }
                if (cur_size == 0)
                    break;
            }

            std::cout << "Client, Finish processing" << std::endl;
            sem_close(_semaphore_0_0);
            sem_close(_semaphore_0_1);
            sem_close(_semaphore_1_0);
            sem_close(_semaphore_1_1);
            sem_unlink(semaphore_path_0_0);
            sem_unlink(semaphore_path_0_1);
            sem_unlink(semaphore_path_1_0);
            sem_unlink(semaphore_path_1_1);
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