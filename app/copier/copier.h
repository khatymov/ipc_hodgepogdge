/*! \file copier.h
 * \brief Copier class interface.
 *
 * Class description.
 *
 */

#pragma once

/*! \class Copier
 * \brief Some briefing
 */
class Copier
{
    Copier(const Copier&) = delete;
    Copier(Copier&&) = delete;
    Copier operator=(const Copier&) = delete;
    Copier operator=(Copier&&) = delete;

public:
    //! \brief default constructor.
    Copier();

    //! \brief default destructor.
    ~Copier() = default;

    //! \brief copy from source file to target file
    //! it compares files via system tool and if files are same - return true
    bool copy();

private:
    //! List of private variables.
};
