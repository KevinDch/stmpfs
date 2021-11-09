#ifndef STMPFS_PATHNAME_T_H
#define STMPFS_PATHNAME_T_H

/** @file
 *
 * This file defines pathname resolver
 */

#include <string>
#include <vector>

typedef std::vector < std::string > pathname_t;

/// STMPFS pathname Entry
class stmpfs_pathname_t
{
private:
    pathname_t pathname;

public:
    /// create an entry link
    /// @param pathname entry
    explicit stmpfs_pathname_t(std::string pathname);

    [[nodiscard]] pathname_t get_pathname() const { return pathname; }
    [[nodiscard]] pathname_t & get_direct_pathname() { return pathname; }
};

#endif //STMPFS_PATHNAME_T_H
