/** @file
 *
 * This file implements relevant operations for filesystem
 */

#include <stmpfs.h>

inode_t & pathname_to_inode(const stmpfs_pathname_t & pathname, inode_t & root)
{
    inode_t * cur_dir = &root;
    for (const auto& path : pathname.get_pathname())
    {
        cur_dir = cur_dir->find_in_dentry(path);
    }

    return *cur_dir;
}

struct timespec current_time()
{
    struct timespec ts{};
    timespec_get(&ts, TIME_UTC);
    return ts;
}
