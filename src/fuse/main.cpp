/** @file
 *
 * This file implements the mount thread for fuse_main and user-interface
 */

#define FUSE_USE_VERSION 31
#include <fuse.h>
#include <iostream>
#include <fuse_ops.h>
#include <stmpfs_error.h>
#include <stmpfs.h>

static struct fuse_operations fuse_ops =
        {
                .getattr    = do_getattr,
                .readlink   = do_readlink,
                .mknod      = do_mknod,
                .mkdir      = do_mkdir,
                .unlink     = do_unlink,
                .rmdir      = do_rmdir,
                .symlink    = do_symlink,
                .rename     = do_rename,
                .chmod      = do_chmod,
                .chown      = do_chown,
                .truncate   = do_truncate,
                .open       = do_open,
                .read       = do_read,
                .write      = do_write,
                .statfs     = do_statfs,
                .flush      = do_flush,
                .release    = do_release,
                .fsync      = do_fsync,
                .setxattr   = do_setxattr,
                .getxattr   = do_getxattr,
                .listxattr  = do_listxattr,
                .removexattr = do_removexattr,
                .opendir    = do_open,
                .readdir    = do_readdir,
                .releasedir = do_releasedir,
                .fsyncdir   = do_fsyncdir,
                .create     = do_create,
                .utimens    = do_utimens,
//                .ioctl      = do_ioctl,
                .fallocate  = do_fallocate,
        };

static void usage(const char *progname)
{
    printf(
            "usage: %s mountpoint [options]\n"
            "\n"
            "general options:\n"
            "    -o opt,[opt...]        Mount options.\n"
            "    -h, --help             Print help.\n"
            "    -V, --version          Print version.\n"
#ifdef CMAKE_BUILD_DEBUG
            "    -k, --hash_check       Enable hash check on every R/W.\n"
#endif // CMAKE_BUILD_DEBUG
            "\n", progname);
}

enum {
    KEY_VERSION,
    KEY_HELP,
#ifdef CMAKE_BUILD_DEBUG
    KET_HASH_CHECK,
#endif // CMAKE_BUILD_DEBUG
};

static struct fuse_opt fs_opts[] = {
        FUSE_OPT_KEY("-V",              KEY_VERSION),
        FUSE_OPT_KEY("--version",       KEY_VERSION),
        FUSE_OPT_KEY("-h",              KEY_HELP),
        FUSE_OPT_KEY("--help",          KEY_HELP),
#ifdef CMAKE_BUILD_DEBUG
        FUSE_OPT_KEY("-k",              KET_HASH_CHECK),
        FUSE_OPT_KEY("--hash_check",    KET_HASH_CHECK),
#endif // CMAKE_BUILD_DEBUG
        FUSE_OPT_END,
};

static int opt_proc(void *, const char *, int key, struct fuse_args *outargs)
{
    static struct fuse_operations ss_nullptr { };

    switch (key)
    {
        case KEY_VERSION:
            printf("%s Version %s\n", PACKAGE_NAME, PACKAGE_VERSION);
            fuse_opt_add_arg(outargs, "--version");
            fuse_main(outargs->argc, outargs->argv, &ss_nullptr, nullptr);
            fuse_opt_free_args(outargs);
            exit(EXIT_SUCCESS);

        case KEY_HELP:
            usage(outargs->argv[0]);
            fuse_opt_add_arg(outargs, "-ho");
            fuse_main(outargs->argc, outargs->argv, &ss_nullptr, nullptr);
            fuse_opt_free_args(outargs);
            exit(EXIT_SUCCESS);

#ifdef CMAKE_BUILD_DEBUG
        case KET_HASH_CHECK:
            if_enable_hash_check = true;
            break;
#endif // CMAKE_BUILD_DEBUG

        default:
            return 1;
    }

    return 0;
}

int main(int argc, char ** argv)
{
    try
    {
        struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

        if (fuse_opt_parse(&args, nullptr, fs_opts, opt_proc) == -1)
        {
            throw stmpfs_error_t(STMPFS_ERROR_CANNOT_PARSE_ARGUMENT);
        }

        auto cur_time = current_time();

        filesystem_root.fs_stat.st_mode = S_IFDIR | 0755;
        filesystem_root.fs_stat.st_atim = cur_time;
        filesystem_root.fs_stat.st_ctim = cur_time;
        filesystem_root.fs_stat.st_mtim = cur_time;

        /*
         * s: run single threaded
         * d: enable debugging
         * f: stay in foreground
         */
        fuse_opt_add_arg(&args, "-s");

#ifdef CMAKE_BUILD_DEBUG
        fuse_opt_add_arg(&args, "-d");
        fuse_opt_add_arg(&args, "-f");
#endif // CMAKE_BUILD_DEBUG

        int ret = fuse_main(args.argc, args.argv, &fuse_ops, nullptr);

        if (ret != 0)
        {
            throw stmpfs_error_t(STMPFS_ERROR_EXTERNAL_LIB_ERROR);
        }

        fuse_opt_free_args(&args);

        return EXIT_SUCCESS;
    }
    catch (stmpfs_error_t & error)
    {
        std::cerr << error.what() << " (errno=" << error.what_errno() << ")" << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception & error)
    {
        std::cerr << error.what() << " (errno=" << strerror(errno) << ")" << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        return EXIT_FAILURE;
    }
}
