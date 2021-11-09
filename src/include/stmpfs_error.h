#ifndef stmpfs_STMPFS_ERROR_H
#define stmpfs_STMPFS_ERROR_H

/** @file
 *
 * This file defines runtime error class and relevant operations
 */

#include <string>
#include <utility>
#include <cstring>

#define STMPFS_ERROR_NO_SUCH_FILE_OR_DIRECTORY  0xA00001    /* No such file or directory */
#define STMPFS_ERROR_PATHNAME_ALREADY_USED      0xA00002    /* Pathname is already used in directory */
#define STMPFS_ERROR_CANNOT_PARSE_ARGUMENT      0xB00001    /* Cannot parse the argument */
#define STMPFS_ERROR_EXTERNAL_LIB_ERROR         0xB00002    /* External library error */

/// Filesystem Error
class stmpfs_error_t : public std::exception
{
private:
    unsigned int error_code;
    error_t _errno;

public:
    /// Generate a error with error code
    /** @param _code Your error code **/
    explicit stmpfs_error_t(unsigned int _code) noexcept : error_code(_code), _errno(errno) {}

    /// Return explanation of current error
    [[nodiscard]] const char * what() const noexcept override;

    /// Return the explanation of errno snapshoted when the current error is generated
    [[nodiscard]] const char * what_errno() const noexcept { return strerror(_errno); };

    /// Return the errno snashoted when the current error is generated
    [[nodiscard]] error_t my_errno() const noexcept { return _errno; }

    /// Return the error code of current error
    [[nodiscard]] unsigned int my_errcode() const noexcept { return error_code; }
};


#endif //stmpfs_STMPFS_ERROR_H
