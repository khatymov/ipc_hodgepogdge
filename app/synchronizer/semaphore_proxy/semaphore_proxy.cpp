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

SemaphoreProxy::SemaphoreProxy(bool isWriter, const std::string& sharedMemName, const std::string& semaphoreType) : m_fWriter(isWriter)
{
    m_sSemaphoreName = sharedMemName + semaphoreType;
    if (isWriter)
    {
        m_pSemaphore = sem_open(m_sSemaphoreName.c_str(), O_CREAT | O_EXCL, 0644, 0);
        if (m_pSemaphore == nullptr)
        {
            std::cerr << "Can't create semaphore for a writer. Semaphore already in use." << std::endl;
            throw std::runtime_error("Writer can't create a semaphore");
        }
    }
    else
    {
        m_pSemaphore = _getReaderSemaphore(m_sSemaphoreName.c_str());
    }

    std::cout << "SemaphoreProxy()" << std::endl;
}

void SemaphoreProxy::setSignaled()
{
    sem_post(m_pSemaphore);
}

bool SemaphoreProxy::getSignaled()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        handle_error("clock_gettime");

    ts.tv_sec += 10; // Wait for up to 10 seconds

    int watch_dog_result = sem_timedwait(m_pSemaphore, &ts);

    if (watch_dog_result == -1)
    {
        std::cerr << "Semaphore watch dog expired" << std::endl;
        return false;
    }

    return true;
}

SemaphoreProxy::~SemaphoreProxy()
{
    sem_close(m_pSemaphore);
    if (m_fWriter)
    {
        sem_unlink(m_sSemaphoreName.c_str());
    }
    std::cout << "~SemaphoreProxy()" << std::endl;
}

sem_t* SemaphoreProxy::_getReaderSemaphore(const char* path)
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
