/*! \file copier.h
 * \brief Copier class interface.
 *
 */

#pragma once

#include <string_view>

#include "file_handler.h"
#include "shared_memory_facade.h"
#include "synchronizer.h"

/*! \class Copier
 * \brief The purpose of this class is to copy data from source file to target file
 *
 * This is main class that includes all logic for coping
 *
 */
class Copier
{
    Copier(const Copier&) = delete;
    Copier(Copier&&) = delete;
    Copier operator=(const Copier&) = delete;
    Copier operator=(Copier&&) = delete;

public:
    /**
     * \brief Creates a Copier object.
     *
     * This Constructor is responsible for
     * - creation shared memory
     * - file object for reading/writing
     * - semaphores for sync
     *
     * \param sourcePath used to open file/create name for shared memory
     * \param targetPath used to open file/create  name for shared memory
     *
     */
    Copier(const std::string_view& sourcePath, const std::string_view& targetPath);

    //! \brief default destructor.
    ~Copier() = default;

    /**
     * \brief do copy from source to target
     *
     * According to the role:
     * - for reader - read from file buffer
     * - for writer - read from buffer and write to a file
     *
     */
    void copy();

    //! \brief Using when copy is done and comapre source and target file via system command 'diff'
    bool isSame() const noexcept;

private:
    //! \brief shared memory object that create/attach to shared memory and defines writer/reader
    SharedMemoryFacade m_sharedMemoryFacade;
    //! \brief a flag of reader/writer
    const bool m_fWriter;
    //! \brief a file object to read write
    FileHandler m_file;
    //! \brief synchronize access to shared memory
    Synchronizer m_synchronizer;
    //! \brief path to source file, using in isSame() func
    const std::string_view& m_sourcePath;
    //! \brief path to target file, using in isSame() func
    const std::string_view& m_targetPath;

    void m_readFromFileToSharedMemory();
    void m_writeToFileFromSharedMemory();
};
