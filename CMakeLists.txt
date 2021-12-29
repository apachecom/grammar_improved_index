cmake_minimum_required(VERSION 3.7)
project(improved_grammar_index)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${PROJECT_SOURCE_DIR}/cmake)

# Guard against in-source builds and bad build-type strings
#
include(ConfigSafeGuards)
#Testing
include(CTest)
#set(LIBS string_dictionaries)

find_package(Threads REQUIRED)
find_package(GTest)

if (GTEST_FOUND)
    set(LIBS ${LIBS} GTest::Main)
else ()

    if (CMAKE_COMPILER_IS_GNUCXX)
        add_definitions(-Wall -ansi -Wno-deprecated -pthread)
    endif ()

    if (MSVC)
        #vc 2012 fix for vararg templates
        set(MSVC_COMPILER_DEFS "-D_VARIADIC_MAX=10")
    endif ()

    include(ConfigGTest)
    set(LIBS ${LIBS} gtest gtest_main)
endif ()




#Benchmark
# If you want to self-test benchmark lib too, turn me ON
#
set(BENCHMARK_ENABLE_TESTING OFF)

# Configure google micro benchmark
#

# c++11 is required
#
if ((CMAKE_CXX_COMPILER_ID MATCHES GNU) OR (CMAKE_CXX_COMPILER_ID MATCHES Clang))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

include(ConfigGBench)

include_directories(~/include)
link_directories(~/lib)

set(SLP_SAMPLING
        SelfGrammarIndex.cpp SelfGrammarIndex.h
        SelfGrammarIndexBS.cpp SelfGrammarIndexBS.h
        ################REPAIR FILES#########################
        utils/repair/arrayG.cpp utils/repair/arrayG.h
        utils/repair/basics.cpp utils/repair/basics.h
        utils/repair/dictionary.cpp utils/repair/dictionary.h
        utils/repair/records.cpp utils/repair/records.h
        utils/repair/heap.cpp utils/repair/heap.h
        utils/repair/hash.cpp utils/repair/hash.h
        utils/repair/RePair.cpp utils/repair/RePair.h
        utils/build_hyb_lz77.h
        ################REPAIR FILES#########################
        binary_relation.cpp binary_relation.h
        compressed_grammar.cpp compressed_grammar.h

        trees/dfuds_tree.cpp trees/dfuds_tree.h
        trees/bp_tree.cpp trees/bp_tree.h
        trees/trie/compact_trie.cpp trees/trie/compact_trie.h
        trees/trie/Trie.cpp trees/trie/Trie.h

        utils/grammar.cpp utils/grammar.h
        )

option(USE_MEM_MONITOR "Enter mem-monitor mode" OFF)

if (USE_MEM_MONITOR STREQUAL ON)
    add_definitions(-DMEM_MONITOR)
else()
    remove_definitions(-DMEM_MONITOR)
endif()


option(USE_PRINT_MODE "Enter print mode" OFF)

if (USE_PRINT_MODE STREQUAL ON)
    add_definitions(-DPRINT_LOGS)
else()
    remove_definitions(-DPRINT_LOGS)
endif()


option(BUILD_EXTERNAL_INDEXES "Enter print mode" OFF)

if (BUILD_EXTERNAL_INDEXES STREQUAL ON)
    add_definitions(-DBUILD_EXTERNAL_INDEXES)
else()
    remove_definitions(-DBUILD_EXTERNAL_INDEXES)
endif()


add_executable(improved_grammar_index main.cpp  )

include(internal_utils)


cxx_executable_with_flags(build_indices "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;cds" bench/build_indices.cpp ${SLP_SAMPLING})
cxx_executable_with_flags(process_coll "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;cds" bench/bench_collections.cpp ${SLP_SAMPLING})
cxx_executable_with_flags(locate_indices "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;cds" bench/locate_indices.cpp ${SLP_SAMPLING})
cxx_executable_with_flags(display_indices "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;cds" bench/display_indices.cpp ${SLP_SAMPLING})

if (NOT INV_PI_T)
    set(INV_PI_T 8)
endif()
message("INV_PI_T ${INV_PI_T}")

if (NOT INV_PI_T_TRIE)
    set(INV_PI_T_TRIE 8)
endif()
message("INV_PI_T_TRIE ${INV_PI_T_TRIE}")

if (NOT INV_PI_T_QGRAM)
    set(INV_PI_T_QGRAM 8)
endif()
message("INV_PI_T_QGRAM ${INV_PI_T_QGRAM}")

#message("MACROS ${PROJECT_SOURCE_DIR}/config_macros.h.in")
#
#configure_file(
#        "${PROJECT_SOURCE_DIR}/config_macros.h.in"
#        "${PROJECT_SOURCE_DIR}/macros.h"
#)


target_link_libraries(improved_grammar_index sdsl)
target_link_libraries(improved_grammar_index cds)
target_link_libraries(improved_grammar_index divsufsort)
target_link_libraries(improved_grammar_index divsufsort64)



