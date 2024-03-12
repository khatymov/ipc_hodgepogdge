/*! \file _buffer.h
 * \brief Buffer class interface.
 *
 * Class description.
 *
 */

#pragma once

#include <errno.h>
#include <fcntl.h> /* For O_* constants */
#include <iostream>
#include <semaphore.h>
#include <string>
#include <sys/stat.h> /* For mode constants */
#include <time.h>

#define STORAGE_SIZE 4096

/*! \struct Buffer
 * \brief Buffer for any type of data, including binary
 */
struct Buffer
{
    size_t size = 0;
    char data[STORAGE_SIZE];
};
