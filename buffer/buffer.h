/*! \file buffer.h
 * \brief Buffer class interface.
 *
 * Class description.
 *
 */


#pragma once


#define STORAGE_SIZE 4096

/*! \struct Buffer
 * \brief Buffer for any type of data, including binary
 */
struct Buffer
{
    size_t size = 0;
    char data[STORAGE_SIZE];
};


/*! \class BufferMode
 * \brief Choose current mode: write or read
 */
enum class BufferMode : int
{
    read,
    write
};
