# Adapted from https://github.com/Crascit/DownloadProject/blob/master/CMakeLists.txt
#
# CAVEAT: use DownloadProject.cmake
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject)
download_project(PROJ                googlebenchmark
                 GIT_REPOSITORY      https://github.com/google/benchmark.git
                 GIT_TAG             master
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "BENCHMARK_ENABLE_TESTING")
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "BENCHMARK_ENABLE_GTEST_TESTS")

add_subdirectory(${googlebenchmark_SOURCE_DIR} ${googlebenchmark_BINARY_DIR})

include_directories("${googlebenchmark_SOURCE_DIR}/include")
