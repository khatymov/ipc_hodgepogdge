/*! \file file_handler.cpp
 * \brief FileHandler class implementation.
 */

#include "file_handler.h"

FileHandler::FileHandler(const std::string& file_name, const std::string& flags)
{
    _file = std::fopen(file_name.data(), flags.data());
}

FileHandler::~FileHandler()
{
    std::fclose(_file);
}

void FileHandler::fread(Buffer* buffer)
{
    buffer->size = std::fread(&buffer->data, sizeof(char), STORAGE_SIZE, _file);
}

void FileHandler::fwrite(Buffer* buffer)
{
    std::fwrite(buffer->data, sizeof(char), buffer->size, _file);
}
