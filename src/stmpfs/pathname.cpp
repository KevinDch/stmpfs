/** @file
 *
 * This file implements pathname resolver
 */

#include <pathname.h>

pathname_t::pathname_t(std::string pathname)
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
            this->my_pathname.emplace_back(cur);
            cur.clear();
        }
        else
        {
            cur += it;
        }
    }

    this->my_pathname.emplace_back(cur);
}
