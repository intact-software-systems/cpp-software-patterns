cmake_minimum_required(VERSION 3.2)
project(RxDiscovery)

add_definitions(-DMAKE_DLL_RxDiscovery)

SET(LIB_NAME RxDiscovery)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData RxTransport)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

set(SOURCE_FILES
        CommonDefines.h
        Export.h
        IncludeExtLibs.h
        IncludeLibs.h
        IncludeLibs.cpp
        ResourceManager.h)

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
