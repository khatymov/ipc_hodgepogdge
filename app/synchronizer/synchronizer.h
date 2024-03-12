/*! \file synchronizer.h
 * \brief Synchronizer class interface.
 *
 * Class description.
 *
 */

#pragma once

#include "semaphore_proxy.h"

/*! \class Synchronizer
 * \brief Some briefing
 */
class Synchronizer
{
    Synchronizer(const Synchronizer&) = delete;
    Synchronizer(Synchronizer&&) = delete;
    Synchronizer& operator=(const Synchronizer&) = delete;
    Synchronizer& operator=(Synchronizer&&) = delete;

public:
    //! \brief constructor.
    Synchronizer(bool is_writer, const std::string& shared_mem_name);

    //! \brief destructor.
    ~Synchronizer() = default;

    //! List of public variables.
    SemaphoreProxy sem_ready;
    SemaphoreProxy sem_ack;

private:
    //! List of private variables.
};
