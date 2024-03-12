/*! \file synchronizer.cpp
 * \brief Synchronizer class implementation.
 */

#include "synchronizer.h"

using namespace std;

Synchronizer::Synchronizer(bool is_writer, const std::string& shared_mem_name)
    : sem_ready(SemaphoreProxy(is_writer, shared_mem_name, string("ready"))), sem_ack(SemaphoreProxy(is_writer, shared_mem_name, string("ack")))
{
}
