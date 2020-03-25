# Adapted from https://github.com/Crascit/DownloadProject/blob/master/CMakeLists.txt
#
# CAVEAT: use DownloadProject.cmake
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#

   include(DownloadProject)
   download_project(PROJ                googletest
                    GIT_REPOSITORY      https://github.com/google/googletest.git
                    GIT_TAG             master
                    UPDATE_DISCONNECTED 1
                    QUIET
   )

   add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
