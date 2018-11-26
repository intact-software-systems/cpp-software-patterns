cmake_minimum_required(VERSION 3.3.2)
project(IntactMiddleware)

set(SOURCE_FILES
    IntactSettings/HostSettings.cpp
    IntactSettings/HostSettings.h
    IntactSettings/IncludeExtLibs.h
    IntactSettings/IncludeLibs.h
    IntactSettings/IntactSettings.cpp
    IntactSettings/IntactSettings.h
    IntactSettings/SettingsListener.cpp
    IntactSettings/SettingsListener.h
    IntactSettings/SettingsManagerBase.cpp
    IntactSettings/SettingsManagerBase.h
#    RouteManagerLib/RouteManagerBase.cpp
#    RouteManagerLib/RouteManagerBase.h
    Export.h
    GroupConnectionManager.cpp
    GroupConnectionManager.h
    GroupManagerBase.cpp
    GroupManagerBase.h
    GroupManagerConnection.cpp
    GroupManagerConnection.h
    IncludeExtLibs.h
    IncludeLibs.h
    INetMulticastManager.cpp
    INetMulticastManager.h
    INetNATManager.cpp
    INetNATManager.h
    IntactComponent.cpp
    IntactComponent.h
    IntactManagerBase.cpp
    IntactManagerBase.h
    LookupServerBase.cpp
    LookupServerBase.h
    MembershipManagerBase.cpp
    MembershipManagerBase.h
    MembershipManagerConnection.cpp
    MembershipManagerConnection.h
    MiddlewareSettings.cpp
    MiddlewareSettings.h
    MulticastManager.cpp
    MulticastManager.h
    ProcessFactoryBase.cpp
    ProcessFactoryBase.h
    RpcMulticastManager.cpp
    RpcMulticastManager.h
    ServiceConnectionManager.cpp
    ServiceConnectionManager.h
    ServiceManagerBase.cpp
    ServiceManagerBase.h
    ServiceManagerConnection.cpp
    ServiceManagerConnection.h
    SystemManagerBase.cpp
    SystemManagerBase.h
    SystemResourceMonitor.cpp
    SystemResourceMonitor.h)

add_definitions(-DMAKE_DLL_IntactMiddleware)

SET(LIB_NAME IntactMiddleware)
SET(EXTRA_LIBS BaseLib NetworkLib MicroMiddleware IntactObjects IntactInfra)
#SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

include_directories(${SOURCE_FILES})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
