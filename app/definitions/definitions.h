/*! \file definitions.h
 * \brief This header uses for different kind of common variables and functions that are used in many classes.
 *
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <string_view>

static std::string getUniqueSharedName(const std::string_view& sourcePath, const std::string_view& targetPath)
{
    std::string sharedName = std::string(sourcePath) + std::string(targetPath);
    std::replace(sharedName.begin(), sharedName.end(), '/', '_');
    sharedName = '/' + sharedName;
    return sharedName;
}

class MyException : public std::exception
{
public:
    MyException(const char* Location) : m_exceptionLocation(Location)
    {
        //        m_exceptionLocation = __FILE__ + std::string(" ") + std::to_string(__LINE__) + std::string(" ") + m_exceptionLocation;
    }

    virtual const char* what() const noexcept override
    {
        return m_exceptionLocation.c_str();
    }

private:
    std::string m_exceptionLocation;
};
