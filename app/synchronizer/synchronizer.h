/*! \file synchronizer.h
 * \brief Synchronizer class interface.
 *
 */

#pragma once

#include "semaphore_proxy.h"

/*! \class Synchronizer
 * \brief Creates 2 named semaphores (ack and ready) to synchronize access to shared memory
 */
class Synchronizer
{
    Synchronizer(const Synchronizer&) = delete;
    Synchronizer(Synchronizer&&) = delete;
    Synchronizer& operator=(const Synchronizer&) = delete;
    Synchronizer& operator=(Synchronizer&&) = delete;

public:
    /**
     * \brief Creates a Synchronizer object.
     *
     * Creates ready and ack semaphores
     *
     * \param isWriter a flag which is responsible whether a semaphore will be created or be attached to some existing semaphore
     * \param sharedMemName a path of shared memory name that is used to create named semaphore
     *
     */
    Synchronizer(bool isWriter, const std::string& sharedMemName);

    //! \brief Destroy created ack and ready semaphores.
    ~Synchronizer() = default;

    // TODO change according to encapsulation
    //! \brief A semaphore that notifies from reader to writer that shared memory is not longer in use and data is ready to be read
    SemaphoreProxy semReady;
    //! \brief A semaphore that notifies from writer to reader that a data in shared memory is read and reader can use shared memory again
    SemaphoreProxy semAck;
};
