cmake_minimum_required(VERSION 3.3.2)
project(SystemManager)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
    ExceptionMonitor.cpp
    ExceptionMonitor.h
    IncludeExtLibs.h
    main.cpp
    ResourceMonitor.cpp
    ResourceMonitor.h
    SystemManager.cpp
    SystemManager.h
    SystemManagerUI.cpp
    SystemManagerUI.h)

add_definitions(-DMAKE_DLL_SystemManger)
SET(EXTRA_LIBS BaseLib NetworkLib MicroMiddleware IntactMiddleware)
include_directories(${SOURCE_FILES})
add_executable(SystemManager ${SOURCE_FILES})
target_link_libraries(SystemManager ${EXTRA_LIBS})
