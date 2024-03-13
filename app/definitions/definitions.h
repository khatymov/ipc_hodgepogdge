/*! \file definitions.h
 * \brief This header uses for different kind of common variables and functions that are used in many classes.
 *
 */

#pragma once

#include <algorithm>
#include <string_view>

static std::string getUniqueSharedName(const std::string_view& sourcePath, const std::string_view& targetPath)
{
    std::string sharedName = std::string(sourcePath) + std::string(targetPath);
    std::replace(sharedName.begin(), sharedName.end(), '/', '_');
    sharedName = '/' + sharedName;
    return sharedName;
}
