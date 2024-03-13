/*! \file copier.h
 * \brief Copier class interface.
 *
 * Class description.
 *
 */

#pragma once

#include <string_view>

#include "file_handler.h"
#include "shared_memory_facade.h"
#include "synchronizer.h"

/*! \class Copier
 * \brief Some briefing
 */
class Copier
{
    Copier(const Copier&) = delete;
    Copier(Copier&&) = delete;
    Copier operator=(const Copier&) = delete;
    Copier operator=(Copier&&) = delete;

public:
    //! \brief default constructor.
    Copier(const std::string_view& sourcePath, const std::string_view& targetPath);

    //! \brief default destructor.
    ~Copier() = default;

    //! \brief copy from source file to target file
    //! it compares files via system tool and if files are same - return true
    void copy();

    bool isSame() const;

private:
    //! List of private variables.
    SharedMemoryFacade m_sharedMemoryFacade;
    const bool m_fWriter;
    FileHandler m_file;
    Synchronizer m_synchronizer;
    const std::string_view& m_sourcePath;
    const std::string_view& m_targetPath;

    void m_readFromFileToSharedMemory();
    void m_writeToFileFromSharedMemory();
};
