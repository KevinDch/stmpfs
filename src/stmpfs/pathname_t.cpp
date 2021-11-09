/** @file
 *
 * This file implements pathname resolver
 */

#include <pathname_t.h>

stmpfs_pathname_t::stmpfs_pathname_t(std::string pathname)
{
    if (pathname == "/")
    {
        return;
    }

    if (!pathname.empty() && *pathname.begin() == '/')
    {
        pathname.erase(pathname.begin());
    }

    if (!pathname.empty() && *pathname.end() == '/')
    {
        pathname.pop_back();
    }

    std::string cur;

    for (auto & it : pathname)
    {
        if (it == '/')
        {
            this->pathname.emplace_back(cur);
            cur.clear();
        }
        else
        {
            cur += it;
        }
    }

    this->pathname.emplace_back(cur);
}