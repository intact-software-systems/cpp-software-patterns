cmake_minimum_required(VERSION 3.3.2)
project(NetworkInformationClient)

set(SOURCE_FILES
    ActiveMeasurement.cpp
    ActiveMeasurement.h
    IncludeExtLibs.h
    main.cpp
    VivaldiClient.cpp
    VivaldiClient.h)

SET(EXTRA_LIBS BaseLib NetworkLib MicroMiddleware IntactMiddleware GraphLib NetworkInformationLib)
include_directories(${SOURCE_FILES})
add_executable(NetworkInformationClient ${SOURCE_FILES})
target_link_libraries(NetworkInformationClient ${EXTRA_LIBS})
