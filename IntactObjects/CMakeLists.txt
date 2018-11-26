cmake_minimum_required(VERSION 3.3.2)
project(IntactObjects)

set(SOURCE_FILES
    Export.h
    IncludeExtLibs.h
    IncludeLibs.h
    IString.h
    ObjectSerializer.cpp
    ObjectSerializer.h)

add_definitions(-DMAKE_DLL_IntactObjects)

SET(LIB_NAME IntactObjects)
SET(EXTRA_LIBS BaseLib NetworkLib MicroMiddleware)

include_directories(${SOURCE_FILES})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
