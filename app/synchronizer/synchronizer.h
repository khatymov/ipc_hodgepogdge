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
    //! \brief default constructor.
    Synchronizer();

    //! \brief default destructor.
    ~Synchronizer() = default;

private:
    //! List of private variables.
};
