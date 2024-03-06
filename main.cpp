/*! \file main.cpp
 * \brief Entry point.
 */

#include <filesystem>
#include <iostream>
#include <span>

#include "timer.h"

namespace fs = std::filesystem;

// TODO delete. only for testing
#include "semaphore_proxy/semaphore_proxy.h"
#include "shared_memory_handler.h"

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

    //    // Testing
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

    return 0;
}