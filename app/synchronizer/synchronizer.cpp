/*! \file synchronizer.cpp
 * \brief Synchronizer class implementation.
 */

#include "synchronizer.h"

#include <iostream>

using namespace std;

Synchronizer::Synchronizer(bool isWriter, const std::string& sharedMemName)
    : semReady(SemaphoreProxy(isWriter, sharedMemName, string("ready"))), semAck(SemaphoreProxy(isWriter, sharedMemName, string("ack")))
{
    //    std::cout << "Synchronizer()" << std::endl;
}
Synchronizer::~Synchronizer()
{
    //    std::cout << "~Synchronizer()" << std::endl;
}
