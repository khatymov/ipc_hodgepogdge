/*! \file timer.h
 * \brief Calculate time consumption using RAII
 */

#pragma once

#include <chrono>
#include <iostream>

class Timer
{
public:
    Timer();
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    ~Timer();

private:
    //! \brief time point initializes at the start of a creating Timer object
    const std::chrono::time_point<std::chrono::system_clock> start_time;
};
