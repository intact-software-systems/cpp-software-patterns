cmake_minimum_required(VERSION 3.2)
project(RxGraph)

add_definitions(-DMAKE_DLL_RxGraph)

SET(LIB_NAME RxGraph)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

set(SOURCE_FILES
        Export.h
        IncludeExtLibs.h
        IncludeLibs.h UndirectedGraph.h UndirectedGraph.cpp)


#find_package(Boost 1.60.0)
#
#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
##    add_executable(foo foo.cc)
#endif()

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
