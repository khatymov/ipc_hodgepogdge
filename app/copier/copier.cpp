/*! \file copier.cpp
 * \brief Copier class implementation.
 */

#include "copier.h"
#include "definitions.h"

Copier::Copier(const std::string_view& source_path, const std::string_view& target_path)
    : _sharedMemoryFacade(source_path, target_path), _is_writer(_sharedMemoryFacade.is_writer()), _file(source_path.data(), _is_writer),
      _synchronizer(_is_writer, get_unique_shared_name(source_path, target_path)), _source_path(source_path), _target_path(target_path)
{
}

void Copier::_read_from_file_to_shared_memory()
{
}

void Copier::_write_to_file_from_shared_memory()
{
}

bool Copier::copy()
{
    if (_is_writer)
    {
        _write_to_file_from_shared_memory();
    }
    else
    {
        _read_from_file_to_shared_memory();
    }

    const auto cmd = std::string("diff ") + _source_path.data() + " " + _target_path.data() + std::string("| exit $(wc -l)");

    return (std::system(cmd.c_str()) == 0);
}
