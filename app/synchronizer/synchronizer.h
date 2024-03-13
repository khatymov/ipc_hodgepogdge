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
    Synchronizer(bool isWriter, const std::string& sharedMemName);

    //! \brief destructor.
    // TODO make default
    ~Synchronizer();

    //! List of public variables.
    SemaphoreProxy semReady;
    SemaphoreProxy semAck;
};
