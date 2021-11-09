#ifndef SMNXFS_STMPFS_H
#define SMNXFS_STMPFS_H

/** @file
 *
 * This file defines relevant operations for filesystem
 */

#include <chrono>
#include <inode.h>
#include <pathname_t.h>

/// pathname to inode, throw error if not found
/** @param pathname pathname to inode
 *  @param root root inode **/
inode_t & pathname_to_inode(const stmpfs_pathname_t & pathname, inode_t & root);

/// get current time
struct timespec current_time();

#endif //SMNXFS_STMPFS_H
