cmake_minimum_required(VERSION 3.3.2)
project(NetworkInformationLib)

set(SOURCE_FILES
    Export.h
    IncludeExtLibs.h
    IncludeLibs.h
    IVivaldiClientBase.h
    IVivaldiGroupBase.h
    IVivaldiManagerBase.h
    vivaldi.cpp
    vivaldi.h
    VivaldiBase.h
    VivaldiClientBase.h
    VivaldiDefines.h
    VivaldiGroupBase.h
    VivaldiInformation.cpp
    VivaldiInformation.h
    VivaldiManagerBase.h)


add_definitions(-DMAKE_DLL_NetworkInformationLib)

SET(LIB_NAME NetworkInformationLib)
SET(EXTRA_LIBS BaseLib NetworkLib MicroMiddleware IntactMiddleware GraphLib)
#SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

include_directories(${SOURCE_FILES})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
