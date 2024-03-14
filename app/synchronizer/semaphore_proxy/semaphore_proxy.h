/*! \file semaphore_proxy.h
 * \brief SemaphoreProxy class interface.
 *
 */

#pragma once

#include <semaphore>
#include <string>

/*! \class SemaphoreProxy
 * \brief SemaphoreProxy is a wrapper for sem_t object
 *
 * This wrapper gives a convenient way to notify that shared memory is not in use anymore
 * and wait some time to be notified
 *
 */
class SemaphoreProxy
{
    SemaphoreProxy(const SemaphoreProxy&) = delete;
    SemaphoreProxy(SemaphoreProxy&&) = delete;
    SemaphoreProxy& operator=(const SemaphoreProxy&) = delete;
    SemaphoreProxy& operator=(SemaphoreProxy&&) = delete;

public:
    /**
     * \brief Creates a SemaphoreProxy object.
     *
     * A wrapper for sem_t
     *
     * \param isWriter a flag which is responsible whether a semaphore will be created or be attached to some existing semaphore
     * \param sharedMemName a path of shared memory name that is used to create named semaphore
     * \param semaphoreType for the semaphore name to distinguish between read/ack
     *
     */
    SemaphoreProxy(bool isWriter, const std::string& sharedMemName, const std::string& semaphoreType);

    /**
     * \brief Destroy semaphore.
     *
     * Closes semaphore and unlink if the semaphore initially was created
     *
     */
    ~SemaphoreProxy();

    //! \brief post semaphore signal
    void setSignaled() noexcept;

    /**
     * \brief get semaphore signaled
     *
     * Upon receiving a signal the func return true, otherwise - false.
     *
     */
    bool getSignaled();

private:
    //! \brief a flag that defines whether a semphore is under reader or writer control
    bool m_fWriter;
    //! \brief a core of current class. The semaphore itself
    sem_t* m_pSemaphore;
    //! \brief name of a semaphore, since we use named semaphore
    std::string m_sSemaphoreName;

    /**
     * \brief try to get/attach a semaphore for reader in case if writer didn't create one
     *
     * Tries to attach 3 times and throw an exception if it failed
     *
     * \param path The path of named semaphore to which it will be attached
     *
     * \return Attached semaphore
     *
     */
    sem_t* _getReaderSemaphore(const char* path);
};
