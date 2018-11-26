cmake_minimum_required(VERSION 3.2)
project(ONE)

add_definitions(-DUSE_GCC)
add_definitions(-DMACX)
#add_definitions(-DWIN32)
#add_definitions(-DSSL_SUPPORT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-reorder -Wno-unused-comparison -Wno-inconsistent-missing-override")

SET(Boost_INCLUDE_DIRS /usr/local/Cellar/boost/1.68.0)
SET(ONE_SOURCE_DIR "./")
SET(CMAKE_MACOSX_RPATH "true")
include_directories(./)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.68.0 COMPONENTS graph)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

FOREACH (libfolder
        BaseLib
        GraphLib
        RxConcurrent
        RxSignal
        RxData
        RxGraph
        RxTransport
        RxDiscovery
        RxActor
        RxObserver
        DataReactor
        RxCommand
        RxStreamer
        RxFSM
        RxMonitor
        NetworkLib
        RTPS
        DDS
        DCPS
        test
        IntactObjects
        IntactInfra
        MicroMiddleware
        IntactMiddleware
        ProcessFactory
        SystemManager
        ServiceManager
        SettingsManager
        NetworkInformationLib
        NetworkInformationClient
        NetworkInformationManager

        )
    add_subdirectory(${libfolder})
ENDFOREACH ()

include_directories(${ONE_SOURCE_DIR})

