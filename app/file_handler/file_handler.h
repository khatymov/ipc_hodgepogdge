/*! \file file_handler.h
 * \brief FileHandler class interface.
 *
 * Class description.
 *
 */

#pragma once

#include <cstdio>
#include <string>

#include "buffer.h"

/*! \class FileHandler
 * \brief Some briefing
 */
class FileHandler
{
    FileHandler(const FileHandler&) = delete;
    FileHandler(FileHandler&&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;
    FileHandler& operator=(FileHandler&&) = delete;

public:
    FileHandler(const std::string& fileName, const std::string& flags);

    void fread(Buffer* pBuffer) noexcept;

    void fwrite(Buffer* pBuffer) noexcept;

    //! \brief default destructor.
    ~FileHandler();

private:
    std::FILE* m_pFile;
};
