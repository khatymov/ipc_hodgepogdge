/*! \file shared_memory_facade.h
 * \brief SharedMemoryFacade class interface.
 *
 * Class description.
 *
 */

#pragma once

/*! \class SharedMemoryFacade
 * \brief Some briefing
 */
class SharedMemoryFacade
{
    SharedMemoryFacade(const SharedMemoryFacade&) = delete;
    SharedMemoryFacade(SharedMemoryFacade&&) = delete;
    SharedMemoryFacade& operator=(const SharedMemoryFacade&) = delete;
    SharedMemoryFacade& operator=(SharedMemoryFacade&&) = delete;

public:
    //! \brief default constructor.
    SharedMemoryFacade();

    //! \brief default destructor.
    ~SharedMemoryFacade() = default;

private:
    //! List of private variables.
};
