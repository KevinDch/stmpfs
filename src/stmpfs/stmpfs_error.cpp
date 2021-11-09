/** @file
 *
 * This file implements error class and relevant operations
 */

#include <stmpfs_error.h>

#define STMPFS_PREFIX "(stmpfs) "

const char *stmpfs_error_t::what() const noexcept
{
    switch (error_code)
    {
        case STMPFS_ERROR_NO_SUCH_FILE_OR_DIRECTORY:
            return STMPFS_PREFIX "No such file or directory";

        case STMPFS_ERROR_PATHNAME_ALREADY_USED:
            return STMPFS_PREFIX "Pathname is already used in directory";

        case STMPFS_ERROR_CANNOT_PARSE_ARGUMENT:
            return STMPFS_PREFIX "Cannot parse the argument";

        case STMPFS_ERROR_EXTERNAL_LIB_ERROR:
            return STMPFS_PREFIX "External library error";

        default:
            return STMPFS_PREFIX "Unknown";
    }
}

