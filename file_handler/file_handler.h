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
public:

    //! \brief default constructor.
    explicit FileHandler(const std::string& file_name, const std::string& flags);

    void fread(Buffer* buffer);

    void fwrite(Buffer* buffer);

    //! \brief default destructor.
    ~FileHandler();

private:
    std::FILE* _file;
};
