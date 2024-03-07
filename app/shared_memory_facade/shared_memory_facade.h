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
    SharedMemoryFacade(const std::string_view& source_path, const std::string_view& target_path);

    //! \brief destructor.
    ~SharedMemoryFacade();

    bool is_writer() const;

    void* get_shared_mem_addr() const;

private:
    //! List of private variables.
    bool _is_writer;
    size_t _mmap_size;
    void* _shared_mem_ptr;
    std::string _shared_object_name;
};
