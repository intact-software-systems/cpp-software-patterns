cmake_minimum_required(VERSION 3.3.2)
project(IntactInfra)

set(SOURCE_FILES
    ConfigurationFile.cpp
    ConfigurationFile.h
    Export.h
    IncludeExtLibs.h
    IncludeLibs.h)

add_definitions(-DMAKE_DLL_IntactInfra)

SET(LIB_NAME IntactInfra)
SET(EXTRA_LIBS BaseLib)

include(FindXercesC)
find_package(XercesC REQUIRED)
include_directories( ${XercesC_INCLUDE_DIR} )

include_directories(${SOURCE_FILES})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS} ${XercesC_LIBRARY})
