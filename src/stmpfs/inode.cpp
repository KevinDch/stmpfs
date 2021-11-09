/** @file
 *
 * This file implements inode and relevant operations
 */

#include <inode.h>
#include <stmpfs_error.h>
#include <iostream>
#include <debug.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

/// read buffer from data
/** @param buffer output buffer
 *  @param length read length
 *  @param offset read offset
 *  @param data input buffer
 *  **/
size_t read_buffer(char * & buffer,
                   size_t & length,
                   off_t & offset,
                   std::vector < char * > & data)
{
    uint64_t orphaned_skipped_read = offset % BLOCK_SIZE;
    uint64_t orphaned_skipped_blocks = offset / BLOCK_SIZE;
    uint64_t orphaned_read = MIN(BLOCK_SIZE - orphaned_skipped_read, length);
    uint64_t rest_read_length = length - orphaned_read;
    uint64_t full_read_count = rest_read_length / BLOCK_SIZE;
    uint64_t orphaned_tail = rest_read_length % BLOCK_SIZE;
    size_t read_offset = 0;

    // head
    memcpy(buffer, data[orphaned_skipped_blocks] + orphaned_skipped_read, orphaned_read);
    read_offset += orphaned_read;
    
    // full
    for (uint64_t i = 1; i <= full_read_count; i++)
    {
        memcpy(buffer + read_offset, 
               data[orphaned_skipped_blocks + i], 
               BLOCK_SIZE);
        read_offset += BLOCK_SIZE;
    }

    // tail
    if (orphaned_tail)
    {
        memcpy(buffer + read_offset,
               data[orphaned_skipped_blocks + full_read_count + 1], orphaned_tail);
        read_offset += orphaned_tail;
    }

    return read_offset;
}

/// write buffer to data
/** @param buffer output buffer
 *  @param length read length
 *  @param offset read offset
 *  @param data input buffer
 *  **/
size_t write_buffer(const char * buffer,
                   size_t length,
                   off_t offset,
                   std::vector < char * > & data)
{
    uint64_t orphaned_skipped_write = offset % BLOCK_SIZE;
    uint64_t orphaned_skipped_blocks = offset / BLOCK_SIZE;
    uint64_t orphaned_write = MIN(BLOCK_SIZE - orphaned_skipped_write, length);
    uint64_t rest_write_length = length - orphaned_write;
    uint64_t full_write_count = rest_write_length / BLOCK_SIZE;
    uint64_t orphaned_tail = rest_write_length % BLOCK_SIZE;
    size_t write_offset = 0;

    // head
    memcpy(data[orphaned_skipped_blocks] + orphaned_skipped_write, buffer, orphaned_write);
    write_offset += orphaned_write;

    // full
    for (uint64_t i = 1; i <= full_write_count; i++)
    {
        data.emplace_back(new char [BLOCK_SIZE]);
        memcpy(data[orphaned_skipped_blocks + i],
               buffer + write_offset,
               BLOCK_SIZE);
        write_offset += BLOCK_SIZE;
    }

    // tail
    if (orphaned_tail)
    {
        data.emplace_back(new char[BLOCK_SIZE]);
        memcpy(data[orphaned_skipped_blocks + full_write_count + 1],
               buffer + write_offset, orphaned_tail);
        write_offset += orphaned_tail;
    }

    return write_offset;
}

/// fill buffer with 0s at back
/**
 *  @param length read length
 *  @param data input buffer
 *  **/
uint64_t fill_buffer(size_t length, std::vector < char * > & data)
{
    uint64_t alloc_blk_count = (length / BLOCK_SIZE) + (length % BLOCK_SIZE == 0 ? 0 : 1);
    for (uint64_t i = 0; i < alloc_blk_count; i++)
    {
        char * new_block = new char [BLOCK_SIZE];
//        memset(new_block, 0, BLOCK_SIZE);
        data.emplace_back(new_block);
    }

    return length;
}

size_t inode_t::read(char *buffer, size_t length, off_t offset)
{
#ifdef CMAKE_BUILD_DEBUG
    if (if_enable_hash_check)
    {
        std::cerr << "[DEBUG] Current sha256: " << hash() << std::endl;
    }
#endif // CMAKE_BUILD_DEBUG

    if (cur_data_size == 0)
    {
        return 0;
    }

    if (offset > cur_data_size)
    {
        return 0;
    }

    if (offset + length > cur_data_size)
    {
        length = cur_data_size - offset;
    }

    // read from changeable buffer
    return read_buffer(buffer, length, offset, data);
}

size_t inode_t::write(const char *buffer, size_t length, off_t offset)
{
#ifdef CMAKE_BUILD_DEBUG
    if (if_enable_hash_check)
    {
        std::cerr << "[DEBUG] Current sha256 (Before write): " << hash() << std::endl;
    }
#endif // CMAKE_BUILD_DEBUG

    // fill buffer
    if ((offset + length) > cur_data_size)
    {
        fill_buffer((offset + length) - cur_data_size, data);
        cur_data_size = length + offset;
        fs_stat.st_size = (off_t)cur_data_size;
    }

    // overwrite
    write_buffer(buffer, length, offset, data);

#ifdef CMAKE_BUILD_DEBUG
    if (if_enable_hash_check)
    {
        std::cerr << "[DEBUG] Current sha256 (After write): " << hash() << std::endl;
    }
#endif // CMAKE_BUILD_DEBUG

    return length;
}

void inode_t::clear()
{
    for (auto i : data)
    {
        delete []i;
    }
    data.clear();

    for (auto & i : dentry)
    {
        if (i.second.if_constructed_by_inode)
        {
            delete i.second.inode;
        }
    }
    dentry.clear();
}

void inode_t::add_dentry(const std::string& name, inode_t& inode, uint64_t if_alloc_by_inode)
{
    auto it = dentry.find(name);
    if (it != dentry.end())
    {
        if (it->second.if_constructed_by_inode)
        {
            delete it->second.inode;
        }

        dentry.erase(it);
    }

    dentry_t new_dentry {
        .if_constructed_by_inode = if_alloc_by_inode,
        .inode = &inode,
    };

    dentry.emplace(name, new_dentry);
}

void inode_t::emplace_new_dentry(const std::string& name, const inode_t& inode)
{
    auto it = dentry.find(name);
    if (it != dentry.end())
    {
        if (it->second.if_constructed_by_inode)
        {
            delete it->second.inode;
        }

        dentry.erase(it);
    }

    dentry_t new_dentry
    {
        .if_constructed_by_inode = 1,
        .inode = new inode_t,
    };

    new_dentry.inode->fs_stat = inode.fs_stat;
    new_dentry.inode->dentry = inode.dentry;
    for (auto i : inode.data)
    {
        char * new_block = new char [BLOCK_SIZE];
        memcpy(new_block, i, BLOCK_SIZE);
        new_dentry.inode->data.emplace_back(new_block);
    }

    dentry.emplace(name, new_dentry);
}

void inode_t::del_dentry(const std::string& name, bool protect_child)
{
    auto it = dentry.find(name);
    if (it != dentry.end())
    {
        if (it->second.if_constructed_by_inode && !protect_child)
        {
            delete it->second.inode;
        }

        dentry.erase(it);
        return;
    }

    throw stmpfs_error_t(STMPFS_ERROR_NO_SUCH_FILE_OR_DIRECTORY);
}

inode_t *inode_t::find_in_dentry(const std::string &name)
{
    auto it = dentry.find(name);
    if (it != dentry.end())
    {
        return it->second.inode;
    }

    throw stmpfs_error_t(STMPFS_ERROR_NO_SUCH_FILE_OR_DIRECTORY);
}

inode_t::inode_t() noexcept = default;

void inode_t::truncate(off_t size)
{
    uint64_t alloc_count = (size / BLOCK_SIZE) + (size % BLOCK_SIZE == 0 ? 0 : 1);
    uint64_t cur_blk_count = data.size();

    if (alloc_count < cur_blk_count)
    {
        for (uint64_t i = 0; i < cur_blk_count - alloc_count; i++)
        {
            delete[] data.back();
            data.pop_back();
        }
    }
    else if (alloc_count > cur_blk_count)
    {
        for (uint64_t i = 0; i < alloc_count - cur_blk_count; i++)
        {
            data.emplace_back(new char [BLOCK_SIZE]);
//            memset(data.back(), 0, BLOCK_SIZE);
        }
    }

    cur_data_size = size;
}

size_t inode_t::count_inode()
{
    uint64_t count = 0;

    for (auto i : dentry)
    {
        count += i.second.inode->count_inode();
    }

    return count + 1;
}


#ifdef CMAKE_BUILD_DEBUG
std::string inode_t::hash()
{
    std::string buff;

    for (auto i : data)
    {
        uint64_t rest = MIN(cur_data_size - buff.size(), BLOCK_SIZE);
        for (uint64_t j = 0; j < rest; j++)
        {
            buff += i[j];
        }
    }

    return sha256(buff);
}
#endif // CMAKE_BUILD_DEBUG
