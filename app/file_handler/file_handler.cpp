/*! \file file_handler.cpp
 * \brief FileHandler class implementation.
 */

#include "file_handler.h"
extern std::FILE* gpFile;

FileHandler::FileHandler(const std::string& fileName, const std::string& flags) : m_pFile(std::fopen(fileName.data(), flags.data()))
{
    gpFile = m_pFile;
    std::cout << "FileHandler()" << std::endl;
}

void FileHandler::fread(Buffer* pBuffer) noexcept
{
    pBuffer->size = std::fread(&pBuffer->data, sizeof(char), STORAGE_SIZE, m_pFile);
}

void FileHandler::fwrite(Buffer* pBuffer) noexcept
{
    std::fwrite(pBuffer->data, sizeof(char), pBuffer->size, m_pFile);
}

FileHandler::~FileHandler()
{
    std::fclose(m_pFile);
    m_pFile = nullptr;
    std::cout << "~FileHandler()" << std::endl;
}
