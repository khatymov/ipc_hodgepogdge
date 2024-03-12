/*! \file copier.cpp
 * \brief Copier class implementation.
 */

#include "copier.h"
#include "definitions.h"

Copier::Copier(const std::string_view& source_path, const std::string_view& target_path)
    : _sharedMemoryFacade(source_path, target_path), _is_writer(_sharedMemoryFacade.is_writer()), _file(source_path.data(), target_path.data(), _is_writer),
      _synchronizer(_is_writer, get_unique_shared_name(source_path, target_path)), _source_path(source_path), _target_path(target_path)
{
}

void Copier::_read_from_file_to_shared_memory()
{
    Buffer* buf_ptr = static_cast<Buffer*>(_sharedMemoryFacade.get_shared_mem_addr());
    do
    {
        _file.fread(buf_ptr);

        const bool everything_done = buf_ptr->size == 0;

        _synchronizer.sem_ready.set_signaled();

        if (!_synchronizer.sem_ack.get_signaled() or everything_done)
        {
            break;
        }

    } while (true);
}

void Copier::_write_to_file_from_shared_memory()
{
    Buffer* buf_ptr = static_cast<Buffer*>(_sharedMemoryFacade.get_shared_mem_addr());
    while (true)
    {
        // waiting when reader write data to shared memory
        if (!_synchronizer.sem_ready.get_signaled())
        {
            break;
        }
        // we have buffer with data at that moment
        const bool everything_done = buf_ptr->size == 0;
        _file.fwrite(buf_ptr);
        _synchronizer.sem_ack.set_signaled();

        if (everything_done)
        {
            break;
        }
    }
}

void Copier::copy()
{
    if (_is_writer)
    {
        _write_to_file_from_shared_memory();
    }
    else
    {
        _read_from_file_to_shared_memory();
    }
}
bool Copier::is_same() const
{
    const auto cmd = std::string("diff ") + _source_path.data() + " " + _target_path.data() + std::string("| exit $(wc -l)");
    const int cmd_res = std::system(cmd.c_str());
    return (cmd_res == 0);
}
