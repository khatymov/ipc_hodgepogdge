/*! \file shared_memory_facade.cpp
 * \brief SharedMemoryFacade class implementation.
 */

#include "shared_memory_facade.h"
#include "buffer.h"
#include "definitions.h"

#include <csignal>
#include <fcntl.h> /* For O_* constants */
#include <stdexcept>
#include <sys/mman.h>

using namespace std;

extern void* gpSharedAddr;
extern std::string gSharedObjectName;

SharedMemoryFacade::SharedMemoryFacade(const std::string_view& sourcePath, const std::string_view& targetPath)
{
    m_SharedObjectName = getUniqueSharedName(sourcePath, targetPath);
    gSharedObjectName = m_SharedObjectName;

    // https://man7.org/linux/man-pages/man3/shm_open.3.html
    // TODO: change magic number
    // last argument: Permission-Bits https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
    const int fileDescriptor = shm_open(m_SharedObjectName.data(), O_CREAT | O_EXCL | O_RDWR, 0666);

    m_mmapSize = sizeof(Buffer);

    if (fileDescriptor > -1)
    {
        std::cout << "READER SharedMemoryFacade()" << std::endl;

        m_fWriter = false;

        if (ftruncate(fileDescriptor, m_mmapSize) == -1)
        {
            std::cerr << "Reader: Error during truncate memory" << std::endl;
            throw std::runtime_error("Error during truncate memory");
        }
        // https://man7.org/linux/man-pages/man2/mmap.2.html
        m_pSharedAddr = mmap(NULL, m_mmapSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0);
        if (m_pSharedAddr == MAP_FAILED)
        {
            std::cerr << "Reader: Error mapping memory " << std::endl;
            throw std::runtime_error("Reader: Error mapping memory");
        }
    }
    else
    {
        m_fWriter = true;
        std::cout << "WRITER. SharedMemoryFacade()" << std::endl;
        const int writerFileDescriptor = shm_open(m_SharedObjectName.data(), O_RDWR, S_IREAD);
        if (writerFileDescriptor > -1)
        {
            m_pSharedAddr = mmap(NULL, m_mmapSize, PROT_READ | PROT_WRITE, MAP_SHARED, writerFileDescriptor, 0);
            if (m_pSharedAddr == MAP_FAILED)
            {
                std::cerr << "Writer: Error mapping memory " << std::endl;
                throw std::runtime_error("Writer: Error mapping memory");
            }
        }
        else
        {
            std::cerr << "Writer: Error to open shared memory " << std::endl;
            throw std::runtime_error("Writer: Error to open shared memory");
        }
    }
}

bool SharedMemoryFacade::isWriter() const noexcept
{
    return m_fWriter;
}

void* SharedMemoryFacade::getSharedMemAddr() const noexcept
{
    return m_pSharedAddr;
}

SharedMemoryFacade::~SharedMemoryFacade()
{

    if (munmap(m_pSharedAddr, m_mmapSize) == -1)
    {
        if (m_fWriter)
        {
            std::cerr << "Writer: Error unmapping memory " << std::endl;
        }
        else
        {
            std::cerr << "Reader: Error unmapping memory " << std::endl;
        }
    }

    if (!m_fWriter)
    {
        std::cout << "Reader: ~SharedMemoryFacade()" << std::endl;
        shm_unlink(m_SharedObjectName.c_str());
    }
    else
    {
        std::cout << "Writer: ~SharedMemoryFacade()" << std::endl;
    }
}
