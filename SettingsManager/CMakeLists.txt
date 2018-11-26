cmake_minimum_required(VERSION 3.3.2)
project(SettingsManager)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
    Export.h
    IncludeExtLibs.h
    main.cpp
    SettingsManager.cpp
    SettingsManager.h)

add_definitions(-DMAKE_DLL_SettingsManager)
SET(EXTRA_LIBS BaseLib NetworkLib MicroMiddleware IntactMiddleware)
include_directories(${SOURCE_FILES})
add_executable(SettingsManager ${SOURCE_FILES})
target_link_libraries(SettingsManager ${EXTRA_LIBS})
