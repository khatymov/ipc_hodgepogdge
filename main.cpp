/*! \file main.cpp
 * \brief Entry point.
 */

#include <filesystem>
#include <iostream>
#include <span>
#include <sys/mman.h>

#include "copier.h"
#include "definitions.h"
#include "timer.h"

namespace fs = std::filesystem;

// TODO delete. only for testing
bool gfWriter;
void* gpSharedAddr;
std::string gSharedObjectName;
std::FILE* gpFile;

sem_t* pSemaphoreReady;
sem_t* pSemaphoreAck;

std::string SemaphoreReadyName;
std::string SemaphoreAckName;

// #include "semaphore_proxy/semaphore_proxy.h"
// #include "shared_memory_facade.h"
// #include "synchronizer.h"
void myTerminate()
{
    std::cout << std::boolalpha;
    std::cout << "gfWriter = " << gfWriter << std::endl;

    if (!gpSharedAddr and munmap(gpSharedAddr, sizeof(Buffer)) == -1)
    {
        if (gfWriter)
        {
            std::cerr << "myTerminate() Writer: Error unmapping memory " << std::endl;
        }
        else
        {
            std::cerr << "myTerminate() Reader: Error unmapping memory " << std::endl;
        }
    }

    shm_unlink(gSharedObjectName.c_str());

    if (gpFile)
    {
        std::fclose(gpFile);
    }

    bool is_ready_exists = false;
    if (pSemaphoreReady)
    {
        is_ready_exists = true;
        sem_close(pSemaphoreReady);
    }

    bool is_ack_exists = false;
    if (pSemaphoreAck)
    {
        is_ack_exists = true;
        sem_close(pSemaphoreAck);
    }

    if (gfWriter and is_ready_exists)
    {
        std::cout << "myTerminate() Writer: ~SemaphoreProxy() unlink" << std::endl;
        sem_unlink(SemaphoreReadyName.c_str());
    }

    if (gfWriter and is_ack_exists)
    {
        std::cout << "myTerminate() Writer: ~SemaphoreProxy() unlink" << std::endl;
        sem_unlink(SemaphoreAckName.c_str());
    }

    std::cerr << "myTerminate() Finish" << std::endl;
    std::abort();
}
int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: ./cpp_course path_to_source path_to_target." << std::endl;
        return EXIT_FAILURE;
    }

    // https://en.cppreference.com/w/cpp/container/span
    // warning: 'do not use pointer arithmetic'
    const auto args = std::span(argv, size_t(argc));
    const std::string_view sourcePath{args[1]};
    const std::string_view targetPath{args[2]};

    if (!fs::exists(sourcePath.data()))
    {
        std::cout << "Source file doesn't exist." << std::endl;
        return EXIT_FAILURE;
    }
    // do not overwrite destination file by default.
    if (sourcePath == targetPath)
    {
        std::cout << "Source and target files are the same." << std::endl;
        return EXIT_SUCCESS;
    }

    Timer timer;

    //    try
    //    {
    std::set_terminate(myTerminate);
    Copier copier(sourcePath, targetPath);
    copier.copy();

    if (copier.isSame())
    {
        std::cerr << "Error copying file." << std::endl;
    }
    //    }
    //    catch (const MyException& e)
    //    {
    //        std::cout << "Catch in " << __func__ << std::endl;
    //        std::cerr << e.what() << std::endl;
    //    }
    //    catch (const std::exception& e)
    //    {
    //        std::cerr << e.what() << std::endl;
    //    }
    //    catch (...)
    //    {
    //        std::cout << "Catch undefined exception" << std::endl;
    //    }

    // Testing
    //    SemaphoreProxy semReadyWriter(true, SharedMemoryHandler::getUniqueSharedName(source_path, target_path), "semReady");
    //    SemaphoreProxy semReadyReader(false, SharedMemoryHandler::getUniqueSharedName(source_path, target_path), "semReady");
    //
    //    if (!semReadyWriter.getSignaled())
    //    {
    //        std::cout << "sem works" << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "sem doesn't work" << std::endl;
    //    }
    //
    //    semReadyReader.setSignaled();
    //    if (semReadyWriter.getSignaled())
    //    {
    //        std::cout << "sem works" << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "sem doesn't work" << std::endl;
    //    }

    //    Synchronizer w_synchronizer(true, SharedMemoryHandler::getUniqueSharedName(source_path, target_path));
    //    Synchronizer r_synchronizer(false, SharedMemoryHandler::getUniqueSharedName(source_path, target_path));
    //
    //    r_synchronizer.semReady.setSignaled();
    //    if (w_synchronizer.semReady.getSignaled())
    //    {
    //        std::cout << "sem works" << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "sem doesn't work" << std::endl;
    //    }

    //    if (!w_synchronizer.semReady.getSignaled())
    //    {
    //        std::cout << "sem works" << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "sem doesn't work" << std::endl;
    //    }
    //
    //    w_synchronizer.semAck.setSignaled();
    //    if (r_synchronizer.semAck.getSignaled())
    //    {
    //        std::cout << "sem works" << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "sem doesn't work" << std::endl;
    //    }

    // !!! check 14 times, it works
    //    SharedMemoryFacade sharedMemoryFacade(source_path, target_path);
    //    Buffer* buffer_ptr = static_cast<Buffer*>(sharedMemoryFacade.getSharedMemAddr());
    //    if (sharedMemoryFacade.isWriter())
    //    {
    //        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //        std::cout << "Writer got size: " << buffer_ptr->size << std::endl;
    //    }
    //    else
    //    {
    //        buffer_ptr->size = 1996;
    //    }

    return 0;
}