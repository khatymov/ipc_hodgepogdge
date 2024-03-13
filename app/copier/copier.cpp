/*! \file copier.cpp
 * \brief Copier class implementation.
 */

#include "copier.h"
#include "definitions.h"

Copier::Copier(const std::string_view& sourcePath, const std::string_view& targetPath)
    : m_sharedMemoryFacade(sourcePath, targetPath), m_fWriter(m_sharedMemoryFacade.isWriter()),
      m_file(m_fWriter ? targetPath.data() : sourcePath.data(), m_fWriter ? "w" : "rb"), m_synchronizer(m_fWriter, getUniqueSharedName(sourcePath, targetPath)),
      m_sourcePath(sourcePath), m_targetPath(targetPath)
{
}

void Copier::m_readFromFileToSharedMemory()
{
    int i = 0;

    Buffer* p_buf = static_cast<Buffer*>(m_sharedMemoryFacade.getSharedMemAddr());
    do
    {
        m_file.fread(p_buf);

        const bool everything_done = p_buf->size == 0;

        m_synchronizer.semReady.setSignaled();

        if (i++ == 5)
        {
            throw MyException("he he, in reader");
        }
        if (!m_synchronizer.semAck.getSignaled() or everything_done)
        {
            break;
        }

    } while (true);
}

void Copier::m_writeToFileFromSharedMemory()
{
    int i = 0;

    Buffer* p_buf = static_cast<Buffer*>(m_sharedMemoryFacade.getSharedMemAddr());
    while (true)
    {
        // waiting when reader write data to shared memory
        if (!m_synchronizer.semReady.getSignaled())
        {
            break;
        }

        //        if (i++ == 4)
        //        {
        //            throw MyException("he he, in writer");
        //        }

        // we have buffer with data at that moment
        const bool everything_done = p_buf->size == 0;
        m_file.fwrite(p_buf);
        m_synchronizer.semAck.setSignaled();

        if (everything_done)
        {
            break;
        }
    }
}

void Copier::copy()
{
    if (m_fWriter)
    {
        m_writeToFileFromSharedMemory();
    }
    else
    {
        m_readFromFileToSharedMemory();
    }
}
bool Copier::isSame() const noexcept
{
    const auto cmd = std::string("diff ") + m_sourcePath.data() + " " + m_targetPath.data() + std::string("| exit $(wc -l)");
    const int cmdResVal = std::system(cmd.c_str());
    return (cmdResVal == 0);
}
