/*! \file main.cpp
 * \brief Entry point.
 */

#include <filesystem>
#include <iostream>
#include <span>

#include "copier.h"
#include "timer.h"

namespace fs = std::filesystem;

// TODO delete. only for testing

// #include "semaphore_proxy/semaphore_proxy.h"
// #include "shared_memory_facade.h"
// #include "synchronizer.h"

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
    const std::string_view source_path{args[1]};
    const std::string_view target_path{args[2]};

    if (!fs::exists(source_path.data()))
    {
        std::cout << "Source file doesn't exist." << std::endl;
        return EXIT_FAILURE;
    }
    // do not overwrite destination file by default.
    if (source_path == target_path)
    {
        std::cout << "Source and target files are the same." << std::endl;
        return EXIT_SUCCESS;
    }

    Timer timer;

    try
    {
        Copier copier(source_path, target_path);
        copier.copy();

        if (copier.is_same())
        {
            std::cerr << "Error copying file." << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Catch in " << __func__ << std::endl;
        std::cerr << e.what() << std::endl;
    }

    // Testing
    //    SemaphoreProxy semReadyWriter(true, SharedMemoryHandler::get_unique_shared_name(source_path, target_path), "semReady");
    //    SemaphoreProxy semReadyReader(false, SharedMemoryHandler::get_unique_shared_name(source_path, target_path), "semReady");
    //
    //    if (!semReadyWriter.get_signaled())
    //    {
    //        std::cout << "sem works" << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "sem doesn't work" << std::endl;
    //    }
    //
    //    semReadyReader.set_signaled();
    //    if (semReadyWriter.get_signaled())
    //    {
    //        std::cout << "sem works" << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "sem doesn't work" << std::endl;
    //    }

    //    Synchronizer w_synchronizer(true, SharedMemoryHandler::get_unique_shared_name(source_path, target_path));
    //    Synchronizer r_synchronizer(false, SharedMemoryHandler::get_unique_shared_name(source_path, target_path));
    //
    //    r_synchronizer.sem_ready.set_signaled();
    //    if (w_synchronizer.sem_ready.get_signaled())
    //    {
    //        std::cout << "sem works" << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "sem doesn't work" << std::endl;
    //    }

    //    if (!w_synchronizer.sem_ready.get_signaled())
    //    {
    //        std::cout << "sem works" << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "sem doesn't work" << std::endl;
    //    }
    //
    //    w_synchronizer.sem_ack.set_signaled();
    //    if (r_synchronizer.sem_ack.get_signaled())
    //    {
    //        std::cout << "sem works" << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "sem doesn't work" << std::endl;
    //    }

    // !!! check 14 times, it works
    //    SharedMemoryFacade sharedMemoryFacade(source_path, target_path);
    //    Buffer* buffer_ptr = static_cast<Buffer*>(sharedMemoryFacade.get_shared_mem_addr());
    //    if (sharedMemoryFacade.is_writer())
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