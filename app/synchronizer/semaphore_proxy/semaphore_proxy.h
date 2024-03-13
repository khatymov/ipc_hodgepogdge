/*! \file semaphore_proxy.h
 * \brief SemaphoreProxy class interface.
 *
 * Class description.
 *
 */

#pragma once

#include <semaphore>
#include <string>

/*! \class SemaphoreProxy
 * \brief Some briefing
 */
class SemaphoreProxy
{
    SemaphoreProxy(const SemaphoreProxy&) = delete;
    SemaphoreProxy(SemaphoreProxy&&) = delete;
    SemaphoreProxy& operator=(const SemaphoreProxy&) = delete;
    SemaphoreProxy& operator=(SemaphoreProxy&&) = delete;

public:
    //! \brief constructor.
    SemaphoreProxy(bool isWriter, const std::string& sharedMemName, const std::string& semaphoreType);

    //! \brief destructor.
    ~SemaphoreProxy();

    //! \brief post semaphore signal
    void setSignaled() noexcept;

    //! \brief get semaphore signaled
    //! Upon receiving a signal the func return true, otherwise - false.
    bool getSignaled();

private:
    //! \brief get semaphore for reader in case if writer didn't create one
    sem_t* _getReaderSemaphore(const char* path);

    //! List of private variables.
    bool m_fWriter;
    sem_t* m_pSemaphore;
    std::string m_sSemaphoreName;
};
