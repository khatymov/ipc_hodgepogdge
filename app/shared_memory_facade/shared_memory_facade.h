/*! \file shared_memory_facade.h
 * \brief SharedMemoryFacade class interface.
 *
 *
 */

#pragma once

#include <string>

/*! \class SharedMemoryFacade
 * \brief Creates 2 named semaphores (ack and ready) to synchronize access to shared memory
 */
class SharedMemoryFacade
{
    SharedMemoryFacade(const SharedMemoryFacade&) = delete;
    SharedMemoryFacade(SharedMemoryFacade&&) = delete;
    SharedMemoryFacade& operator=(const SharedMemoryFacade&) = delete;
    SharedMemoryFacade& operator=(SharedMemoryFacade&&) = delete;

public:
    /**
     * \brief Creates a SharedMemory object.
     *
     * Create a shared memory if it is a reader or attach to a shared memory if it is a writer
     *
     * \param sourcePath used to create unique name for shared memory
     * \param targetPath used to create unique name for shared memory
     *
     */
    SharedMemoryFacade(const std::string_view& sourcePath, const std::string_view& targetPath);

    /**
     * \brief Destroy SharedMemory object.
     *
     * Unmap from shared memory(for reader and writer) and close shared memory object(for reader)
     *
     */
    ~SharedMemoryFacade();

    //! \brief get a flag whether it is a reader or writer.
    bool isWriter() const noexcept;

    //! \brief get a pointer to shared memory
    void* getSharedMemAddr() const noexcept;

private:
    //! \brief a flag that defines reader/writer
    bool m_fWriter;
    //! \brief the size of mapped memory in a heap
    size_t m_mmapSize;
    //! \brief points to a shared memory
    void* m_pSharedAddr;
    //! \brief name of a shared memory object
    std::string m_SharedObjectName;
};
