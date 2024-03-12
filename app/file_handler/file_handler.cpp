/*! \file file_handler.cpp
 * \brief FileHandler class implementation.
 */

#include "file_handler.h"

FileHandler::FileHandler(const std::string& source, const std::string& target, const bool is_writer)
{
    std::string flags;

    // TODO rework, really bad..
    if (is_writer)
    {
        flags = "w";
        std::cout << "Writer writes to file: " << target << std::endl;
        _file = std::fopen(target.data(), flags.data());
    }
    else
    {
        flags = "rb";
        std::cout << "Reader reads a file: " << source << std::endl;
        _file = std::fopen(source.data(), flags.data());
    }
}

FileHandler::~FileHandler()
{
    std::fclose(_file);
    _file = nullptr;
}

void FileHandler::fread(Buffer* buffer)
{
    buffer->size = std::fread(&buffer->data, sizeof(char), STORAGE_SIZE, _file);
}

void FileHandler::fwrite(Buffer* buffer)
{
    std::fwrite(buffer->data, sizeof(char), buffer->size, _file);
}
