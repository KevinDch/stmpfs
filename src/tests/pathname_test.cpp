/** @file
 *
 * This file implements unit test for pathname resolver
 */

#include <pathname.h>

int main()
{
    pathname_t path("/tmp/tmp/tmp");

    for (auto & i : path)
    {
        if (i != "tmp")
        {
            return 1;
        }
    }

    return 0;
}
