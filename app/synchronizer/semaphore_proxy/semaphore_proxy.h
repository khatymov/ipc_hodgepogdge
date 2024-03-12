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
    SemaphoreProxy(bool is_writer, const std::string& shared_mem_name, const std::string& sem_type);

    //! \brief destructor.
    ~SemaphoreProxy();

    //! \brief post semaphore signal
    void set_signaled();

    //! \brief get semaphore signaled
    //! Upon receiving a signal the func return true, otherwise - false.
    bool get_signaled();

private:
    //! \brief get semaphore for reader in case if writer didn't create one
    sem_t* _get_reader_semaphore(const char* path);

    //! List of private variables.
    bool _is_writer;
    sem_t* _semaphore;
    std::string _semaphore_name;
};
