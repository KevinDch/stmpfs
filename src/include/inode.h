#ifndef SMNXFS_INODE_H
#define SMNXFS_INODE_H

/** @file
 *
 * This file defines inode and relevant operations
 */

#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <map>
#include <debug.h>

#define BLOCK_SIZE (1024)

class inode_t
{
private:
    struct dentry_t
    {
        uint64_t    if_constructed_by_inode:1;  // if this dentry is emplace'd
        inode_t *   inode;
    };

    std::vector < char * > data;                // if is a file, use this data
    uint64_t cur_data_size = 0;
    std::map < std::string, dentry_t > dentry;  // if is a directory, use this dentry

#ifdef CMAKE_BUILD_DEBUG
    /// return hash of current data
    std::string hash();
#endif // CMAKE_BUILD_DEBUG

public:
    struct stat fs_stat { };            // file/dir stat, publicly changeable

    std::map < std::string, std::string > xattr;

    /// read from buffer
    /** @param buffer output buffer
     *  @param length length for reading
     *  @param offset read offset **/
    size_t read(char * buffer, size_t length, off_t offset);

    /// write to buffer
    /** @param buffer output buffer
     *  @param length length for writing
     *  @param offset write offset **/
    size_t write(const char * buffer, size_t length, off_t offset);

    /// clear content
    void clear();

    /// add directory entry
    /** @param name dentry name
     *  @param inode inode **/
    void add_dentry(const std::string& name, inode_t& inode, uint64_t if_alloc_by_inode = 0);

    /// create a new directory entry
    /** @param name dentry name
     *  @param inode inode **/
    void emplace_new_dentry(const std::string& name, const inode_t& inode);

    /// delete directory entry
    /** @param name dentry name
     *  @param protect_child if delete child **/
    void del_dentry(const std::string& name, bool protect_child = false);

    /// find name in next level dentry list
    /** @param name pathname (one level) **/
    inode_t* find_in_dentry(const std::string& name);

    /// get dentry list
    [[nodiscard]] std::map < std::string, dentry_t > my_dentry () const { return dentry; }

    /// deconstruction
    ~inode_t() { clear(); }

    /// construction
    inode_t() noexcept;

    /// change buffer size
    /** @param size target size **/
    void truncate(off_t size);

    /// count inode (includes self) since this inode
    size_t count_inode();

    inode_t & operator=(const inode_t&&) = delete;
    inode_t(const inode_t &&) = delete;
};


#endif //SMNXFS_INODE_H
