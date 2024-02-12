/*! \file timer.h
 * \brief Calculate time consumption using RAII
 */

#pragma once

#include <chrono>
#include <iostream>

class Timer
{
public:
    Timer() : start_time(std::chrono::system_clock::now())
    {
    }

    Timer(const Timer&) = delete;

    ~Timer()
    {
        // Calculate duration
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time);
        // Extract seconds and milliseconds
        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        const auto milliseconds = duration - seconds;
        // Print the results
        std::cout << "Copy time: " << seconds.count() << "s " << milliseconds.count() << " ms\n";
    }

private:
    //! \brief time point initializes at the start of a creating Timer object
    const std::chrono::time_point<std::chrono::system_clock> start_time;
};
