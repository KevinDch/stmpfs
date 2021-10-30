#ifndef STMPFS_PATHNAME_H
#define STMPFS_PATHNAME_H

/** @file
 *
 * This file defines pathname resolver
 */

#include <string>
#include <vector>

/// STMPFS Entry
class pathname_t
{
private:
    std::vector < std::string > my_pathname;

public:
    /// create an entry link
    /// @param pathname entry
    /// @return none
    explicit pathname_t(std::string pathname);

    /// return last element in my_pathname
    /// NOTE that this function DOES NOT check if
    /// the vector is empty or not
    /// @return last element in my_pathname
    std::string back() { return my_pathname.back(); }

    /// pop the last element out
    /// NOTE that this function DOES NOT check if
    /// the vector is empty or not
    /// @return none
    void pop_back() { my_pathname.pop_back(); };

    /// @return size of the vector
    [[nodiscard]] size_t size() const { return my_pathname.size(); }

    /// @return the begin of the vector iterator
    std::vector < std::string >::iterator begin() { return my_pathname.begin(); }

    /// @return the end of the vector iterator
    std::vector < std::string >::iterator end() { return my_pathname.end(); }
};

#endif //STMPFS_PATHNAME_H
