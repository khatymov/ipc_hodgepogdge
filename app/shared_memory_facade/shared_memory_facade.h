/*! \file shared_memory_facade.h
 * \brief SharedMemoryFacade class interface.
 *
 * Class description.
 *
 */

#pragma once

#include <string>
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
    //! \brief constructor.
    SharedMemoryFacade(const std::string_view& sourcePath, const std::string_view& targetPath);

    //! \brief destructor.
    ~SharedMemoryFacade();

    bool isWriter() const;

    void* getSharedMemAddr() const;

private:
    //! List of private variables.
    bool m_fWriter;
    size_t m_mmapSize;
    void* m_pSharedSem;
    std::string m_SharedObjectName;
};
