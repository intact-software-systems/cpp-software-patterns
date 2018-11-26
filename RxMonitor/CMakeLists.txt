cmake_minimum_required(VERSION 3.2)
project(RxMonitor)

add_definitions(-DMAKE_DLL_RxMonitor)

SET(LIB_NAME RxMonitor)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData RxTransport RxObserver DataReactor RxCommand RxStreamer)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

set(SOURCE_FILES
        CommandMonitor.cpp
        CommandMonitor.h
        Strategy/CommandMonitorStrategy.h
        Strategy/CommandMonitorStrategy.cpp
        Export.h
        IncludeExtLibs.h
        IncludeLibs.h)

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
#add_library(one::${LIB_NAME} ALIAS ${LIB_NAME})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
