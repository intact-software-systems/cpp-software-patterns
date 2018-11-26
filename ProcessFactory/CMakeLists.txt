cmake_minimum_required(VERSION 3.3.2)
project(ProcessFactory)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_definitions(-DMAKE_DLL_ProcessFactory)

#SET(LIB_NAME ProcessFactory)
SET(EXTRA_LIBS BaseLib NetworkLib MicroMiddleware IntactMiddleware)
#SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

set(SOURCE_FILES
    FactoryLibrary.cpp
    FactoryLibrary.h
    FactoryLibraryLinux.cpp
    FactoryLibraryWindows.cpp
    IncludeExtLibs.h
    main.cpp
    PdhQuery.cpp
    PdhQuery.h
    ProcessFactory.cpp
    ProcessFactory.h
    ResourceMonitor.cpp
    ResourceMonitor.h)

include_directories(${SOURCE_FILES})
#target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
#add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
add_executable(ProcessFactory ${SOURCE_FILES})
target_link_libraries(ProcessFactory ${EXTRA_LIBS})

