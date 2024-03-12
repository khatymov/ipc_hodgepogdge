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
    Copier(const std::string_view& source_path, const std::string_view& target_path);

    //! \brief default destructor.
    ~Copier() = default;

    //! \brief copy from source file to target file
    //! it compares files via system tool and if files are same - return true
    void copy();

    bool is_same() const;

private:
    //! List of private variables.
    SharedMemoryFacade _sharedMemoryFacade;
    bool _is_writer;
    FileHandler _file;
    Synchronizer _synchronizer;

    const std::string_view& _source_path;
    const std::string_view& _target_path;

    void _read_from_file_to_shared_memory();
    void _write_to_file_from_shared_memory();
};
