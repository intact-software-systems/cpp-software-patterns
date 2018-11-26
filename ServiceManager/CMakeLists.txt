cmake_minimum_required(VERSION 3.3.2)
project(ServiceManager)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
    Export.h
    IncludeExtLibs.h
    main.cpp
    ServiceManager.cpp
    ServiceManager.h
    ServiceManagerUI.cpp
    ServiceManagerUI.h)


add_definitions(-DMAKE_DLL_ServiceManager)
SET(EXTRA_LIBS BaseLib NetworkLib MicroMiddleware IntactMiddleware)
include_directories(${SOURCE_FILES})
add_executable(ServiceManager ${SOURCE_FILES})
target_link_libraries(ServiceManager ${EXTRA_LIBS})
