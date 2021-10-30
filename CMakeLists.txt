cmake_minimum_required(VERSION 3.20)
project(stmpfs)

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
        src/stmpfs/pathname.cpp     src/include/pathname.h
        src/stmpfs/buffer.cpp       src/include/buffer.h
        )
target_include_directories(stmpfs PUBLIC src/include)
target_compile_definitions(stmpfs PUBLIC "_FILE_OFFSET_BITS=64")

# mount thread
add_executable(mount.stmpfs
        src/fuse/main.cpp)
target_include_directories(mount.stmpfs PUBLIC src/include)
target_link_libraries(mount.stmpfs PUBLIC stmpfs fuse)

# add unit test
function(stmpfs_add_test TEST DESCRIPTION)
    set(TEST_NAME "UT_${TEST}")

    add_executable(${TEST_NAME} "src/tests/${TEST}_test.cpp")
    target_link_libraries(${TEST_NAME} PUBLIC stmpfs)
    target_include_directories(${TEST_NAME} PUBLIC src/include)

    add_test(NAME ${TEST_NAME}
            COMMAND ${TEST_NAME}
            WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    message("Unit test `${TEST_NAME}`\t(${DESCRIPTION}) enabled")
endfunction()

# unit tests

# passed on
stmpfs_add_test(pathname "Filesystem pathname resolver test")

stmpfs_add_test(buffer "Filesystem data buffer test")
