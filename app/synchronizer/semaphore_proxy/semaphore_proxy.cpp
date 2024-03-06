/*! \file semaphore_proxy.cpp
 * \brief SemaphoreProxy class implementation.
 */

#include "semaphore_proxy.h"

#include <fcntl.h> /* For O_* constants */
#include <iostream>
#include <stdexcept>

using namespace std;

#define handle_error(msg)                                                                                                                                      \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        perror(msg);                                                                                                                                           \
        exit(EXIT_FAILURE);                                                                                                                                    \
    } while (0)

SemaphoreProxy::SemaphoreProxy(bool is_writer, const std::string& shared_mem_name, const std::string& sem_type) : _is_writer(is_writer)
{
    _semaphore_name = shared_mem_name + sem_type;
    if (is_writer)
    {
        _semaphore = sem_open(_semaphore_name.c_str(), O_CREAT | O_EXCL, 0644, 0);
        if (_semaphore == nullptr)
        {
            std::cerr << "Can't create semaphore for a writer. Semaphore already in use." << std::endl;
            throw std::runtime_error("Writer can't create a semaphore");
        }
    }
    else
    {
        _semaphore = _get_reader_semaphore(_semaphore_name.c_str());
    }
}

void SemaphoreProxy::set_signaled()
{
    sem_post(_semaphore);
}

bool SemaphoreProxy::get_signaled()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        handle_error("clock_gettime");

    ts.tv_sec += 10; // Wait for up to 10 seconds

    int watch_dog_result = sem_timedwait(_semaphore, &ts);

    if (watch_dog_result == -1)
    {
        return false;
    }

    return true;
}

SemaphoreProxy::~SemaphoreProxy()
{
    sem_close(_semaphore);
    if (_is_writer)
    {
        sem_unlink(_semaphore_name.c_str());
    }
}

sem_t* SemaphoreProxy::_get_reader_semaphore(const char* path)
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
            return sem;
        }
    }

    if (sem == nullptr)
    {
        std::cerr << "Can not open a semaphore." << std::endl;
        throw std::runtime_error("Reader can't attach a semaphore");
    }
}
