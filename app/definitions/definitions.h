/*! \file definitions.h
 * \brief This header uses for different kind of common variables and functions that are used in many classes.
 *
 */

#pragma once

#define STORAGE_SIZE 4096

#include <algorithm>
#include <string_view>

static std::string get_unique_shared_name(const string_view& source_path, const string_view& target_path)
{
    std::string shared_name = std::string(source_path) + std::string(target_path);
    std::replace(shared_name.begin(), shared_name.end(), '/', '_');
    shared_name = '/' + shared_name;
    return shared_name;
}
