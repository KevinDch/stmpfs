cmake_minimum_required(VERSION 3.13)
project(smnxfs)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions("PACKAGE_NAME=\"FUSE-based Temporary Filesystem\"")

# determine build type (and also inform source files)
if ("X${CMAKE_BUILD_TYPE}" STREQUAL "XDebug")
    message("CMake is building in debug mode!")
    enable_testing()
    add_compile_definitions("CMAKE_BUILD_DEBUG=1")
    add_compile_definitions("PACKAGE_VERSION=\"1.0.0 (beta)\"")
else()
    message("CMake is building in release mode!")
    add_compile_definitions("CMAKE_BUILD_RELEASE=1")
    add_compile_definitions("PACKAGE_VERSION=\"1.0.0\"")
endif()

# filesystem
add_library(stmpfs STATIC
        src/stmpfs/pathname_t.cpp           src/include/pathname_t.h
        src/stmpfs/inode.cpp                src/include/inode.h
        src/stmpfs/stmpfs_error.cpp         src/include/stmpfs_error.h
        src/stmpfs/stmpfs.cpp               src/include/stmpfs.h
        src/include/debug.h
        src/stmpfs/sha256sum.cpp
        )
target_include_directories(stmpfs PUBLIC src/include)
target_compile_definitions(stmpfs PUBLIC "_FILE_OFFSET_BITS=64")

# mount thread
add_executable(mount.stmpfs
        src/fuse/main.cpp
        src/fuse/fuse_ops.cpp               src/include/fuse_ops.h)
target_include_directories(mount.stmpfs PUBLIC src/include)
target_link_libraries(mount.stmpfs PUBLIC stmpfs fuse)

# add unit test
function(stmpfs_add_test TEST DESCRIPTION)
    set(TEST_NAME "UT_${TEST}")

    add_executable(${TEST_NAME} "tests/${TEST}_test.cpp")
    target_link_libraries(${TEST_NAME} PUBLIC stmpfs)
    target_include_directories(${TEST_NAME} PUBLIC src/include)

    add_test(NAME ${TEST_NAME}
            COMMAND ${TEST_NAME}
            WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    message("Unit test `${TEST_NAME}`\t(${DESCRIPTION}) enabled")
endfunction()

# unit tests

if ("X${CMAKE_BUILD_TYPE}" STREQUAL "XDebug")
    # passed on a1c84c20d8e9c3bbb5e911723d8906751b3b8e0c
    stmpfs_add_test(pathname "Filesystem pathname resolver test")

    # passed on 7c0963f6f884c3853c2589c50239b4bf5156ced3
    stmpfs_add_test(inode "Filesystem inode test")

    # passed on 17171d144e36d4c27f09ee7ade5bc254bc05043b
    stmpfs_add_test(nami "Filesystem pathname to inode test")
endif()
