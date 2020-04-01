cmake_minimum_required(VERSION 3.5)

project(slp)

set(CMAKE_CXX_STANDARD 14)
include_directories(~/include)
set(SOURCES_FILES 

    BinRel/BinaryRelation.cpp
    BinRel/LabeledBinaryRelation.cpp
    RePair/arrayG.cpp
    RePair/basics.cpp
    RePair/dictionary.cpp
    RePair/hash.cpp
    RePair/heap.cpp
    RePair/records.cpp
    RePair/RePair.cpp
    Samplings/CSD.cpp
    Samplings/CSD_HTFC.cpp
    Samplings/sampling.cpp
    Samplings/VByte2.cpp
    Utils/deltacodes.cpp
    Utils/SuffixArray.cpp
                                RePairSLPIndex.cpp

    BinRel/FullQuery.h
    BinRel/Occ.h
    BinRel/Point.h
    BinRel/Rule.h
    BinRel/RuleTrav.h
    BinRel/UtilsBinRel.h

    Samplings/binarynode.h
    Samplings/hutucker2.h
    Utils/offsets.h


        #SLPIndex.cpp



)

#add_executable(slp test/idx_test.cpp ${SOURCES_FILES})
#add_executable(slp_console SLPIndex.cpp ${SOURCES_FILES})


add_library(slp ${SOURCES_FILES})

#target_link_libraries(slp cds)
#target_link_libraries(slp divsufsort)
#target_link_libraries(slp divsufsort64)

#target_link_libraries(slp_console cds)
#target_link_libraries(slp_console divsufsort)
#target_link_libraries(slp_console divsufsort64)
