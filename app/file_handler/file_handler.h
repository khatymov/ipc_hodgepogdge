/*! \file file_handler.h
 * \brief FileHandler class interface.
 *
 */

#pragma once

#include <cstdio>
#include <string>

#include "buffer.h"

/*! \class FileHandler
 * \brief A wrapper for std::File. The purpose of this class is to use RAII
 */
class FileHandler
{
    FileHandler(const FileHandler&) = delete;
    FileHandler(FileHandler&&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;
    FileHandler& operator=(FileHandler&&) = delete;

public:
    /**
     * \brief Creates a FileHandler object.
     *
     * Open a file to read or write with corresponding flags
     *
     * \param fileName path to a file
     * \param flags flags that defines in which mode are we going to work: read from file/write to file
     *
     */
    FileHandler(const std::string& fileName, const std::string& flags);

    /**
     * \brief Read data from a file to a buffer
     *
     * Fills a buffer with data of size STORAGE_SIZE(should be system's page size)
     * if data size is less than STORAGE_SIZE it reads the remaining size
     *
     * \param pBuffer a pointer to Buffer struct
     *
     */
    void fread(Buffer* pBuffer) noexcept;
    /**
     * \brief Write data from a buffer to a file
     *
     * Writes buffer's data to file according to buffer's size
     *
     * \param pBuffer a pointer to Buffer struct
     *
     */
    void fwrite(Buffer* pBuffer) noexcept;

    /**
     * \brief Destroy a FileHandler object.
     *
     * Close a file
     *
     */
    ~FileHandler();

private:
    //! \brief core of FileHandler class.
    std::FILE* m_pFile;
};
