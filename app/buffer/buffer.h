/*! \file _buffer.h
 * \brief Buffer class interface.
 *
 * Class description.
 *
 */


#pragma once

#include <semaphore.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <string>
#include <iostream>
#include <time.h>
#include <errno.h>

#define STORAGE_SIZE 4096

/*! \struct Buffer
 * \brief Buffer for any type of data, including binary
 */
struct Buffer
{
    size_t size = 0;
    char data[STORAGE_SIZE];
};


/*! \class BufferMode
 * \brief Choose current mode: write or read
 */
enum class BufferMode : int
{
    read,
    write
};

//TODO: delete
#include <thread>
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

//static void handler(int sig, sem_t* sem)
//{
//    write(STDOUT_FILENO, "sem_post() from handler\n", 24);
//    if (sem_post(&sem) == -1) {
//        write(STDERR_FILENO, "sem_post() failed\n", 18);
//        _exit(EXIT_FAILURE);
//    }
//}

class SemaphoreHandler
{
public:
    SemaphoreHandler(const std::string& shared_object_name, uint8_t index)
    {
        _sem_ready_path = shared_object_name + std::string("_ready_") + std::to_string(index);
        _sem_ack_path = shared_object_name + std::string("_ack_") + std::to_string(index);

        std::cout << "_sem_ready_path: " << _sem_ready_path << std::endl;
        std::cout << "_sem_ack_path: " << _sem_ack_path << std::endl;

        _sem_ready = sem_open(_sem_ready_path.c_str(), O_CREAT | O_EXCL, 0644, 0);
        _sem_ack = sem_open(_sem_ack_path.c_str(), O_CREAT | O_EXCL, 0644, 0);

        if (_sem_ready == nullptr)
        {
            std::cout << "Can't create _sem_ready, try to attach: " << _sem_ack_path << std::endl;
            _sem_ready = try_to_get_semaphore(_sem_ready_path.c_str());
        }

        if (_sem_ack == nullptr)
        {
            std::cout << "Can't create _sem_ack , try to attach: " << _sem_ack_path << std::endl;
            _sem_ack = try_to_get_semaphore(_sem_ack_path.c_str());
        }

        int sem_ready_val;
        int sem_ack_val;

        int res_sem_ready_val;
        int res_sem_ack_val;

        if ((res_sem_ready_val = sem_getvalue(_sem_ready, &sem_ready_val) == -1) or (res_sem_ack_val = sem_getvalue(_sem_ack, &sem_ack_val) == -1))
        {
            std::cerr << "Can't get a semaphore value." << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << "sem_ready_val: " << sem_ready_val << " res_sem_ready_val = " << res_sem_ready_val << std::endl;
        std::cout << "sem_ack_val: " << sem_ack_val << " res_sem_ack_val = " << res_sem_ack_val << std::endl;

        if (sem_ready_val > 1 or sem_ack_val > 1)
        {
//            sem_wait(_sem_ready);
//            sem_wait(_sem_ack);
            sem_close(_sem_ready);
            sem_close(_sem_ack);
            std::cout << "Reached max number of clients. Exit" << std::endl;
            exit(EXIT_SUCCESS);
        }
    }

    ~SemaphoreHandler()
    {
        _close_semaphores();
    }


    sem_t* try_to_get_semaphore(const char* path)
    {
        sem_t* sem;
        const int num_of_try = 3;
        int i = 0;
        // 3 times
        while (i != num_of_try)
        {
            sem = sem_open(path, 0, 0644, 0);
            i++;
            if (sem == nullptr)
            {
                std::cout << "Failed to attach, try again: " << path << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            else
            {
                std::cout << "Semaphore attached: " << path << std::endl;
                // increment semaphore value to do_ping_pong, that one client has already attached
//                sem_post(sem);
                return sem;
            }
        }

        if (sem == nullptr)
        {
            std::cerr << "Can not open a semaphore." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void do_ping_pong(const BufferMode& mode)
    {
        //!!! be sure that semaphores values are 0

        //TODO find a location for this var
        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
            handle_error("clock_gettime");

        ts.tv_sec += 10;  // Wait for up to 5 seconds

        if (mode == BufferMode::write)
        {
            std::cout << "BufferMode::write" << std::endl;
            if (_sem_ready == nullptr)
            {
                std::cerr << "The semaphore is nullptr." << std::endl;
            }

            if (_sem_ready == (void *) - 1)
            {
                std::cerr << "The semaphore is -1." << std::endl;
            }

            sem_post(_sem_ready);
            int sem_wd_res = sem_timedwait(_sem_ack, &ts);
            if (sem_wd_res == -1) {
                if (errno == ETIMEDOUT)
                {
                    std::cout << "BufferMode::write sem_timedwait() timed out"<< std::endl;
                    _close_semaphores();
                    exit(EXIT_FAILURE);
                } else
                {
                    std::cerr << "BufferMode::write sem_timedwait" << std::endl;
                    _close_semaphores();
                    exit(EXIT_FAILURE);
                }
            } else
            {
                std::cout << "BufferMode::write sem_timedwait() succeeded" << std::endl;
            }
        } else if (mode == BufferMode::read)
        {
            std::cout << "BufferMode::read" << std::endl;
            int sem_wd_res = sem_timedwait(_sem_ready, &ts);
            if (sem_wd_res == -1) {
                if (errno == ETIMEDOUT)
                {
                    std::cout << "BufferMode::read sem_timedwait() timed out" << std::endl;
                    _close_semaphores();
                    exit(EXIT_FAILURE);
                }
                else
                {
                    std::cerr << "BufferMode::read sem_timedwait" << std::endl;
                    _close_semaphores();
                    exit(EXIT_FAILURE);
                }
            } else
            {
                std::cout << "BufferMode::read sem_timedwait() succeeded" << std::endl;
            }

            sem_post(_sem_ack);
        }
    }

private:
    sem_t* _sem_ready;
    sem_t* _sem_ack;
    std::string _sem_ready_path;
    std::string _sem_ack_path;

    void _close_semaphores()
    {
        sem_close(_sem_ready);
        sem_close(_sem_ack);
        //TODO: handle a proper way to unlink. Should be done once, for the entity, that created the semaphore
        sem_unlink(_sem_ready_path.c_str());
        sem_unlink(_sem_ack_path.c_str());
    }
};
