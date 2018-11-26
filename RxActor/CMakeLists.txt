cmake_minimum_required(VERSION 3.2)
project(RxActor)

add_definitions(-DMAKE_DLL_RxActor)

SET(LIB_NAME RxActor)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData RxTransport RxDiscovery)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

set(SOURCE_FILES
        CommonDefines.h
        Export.h
        IncludeExtLibs.h
        IncludeLibs.h
        IncludeLibs.cpp
        Actor.h
        Supervisor.cpp
        Supervisor.h
        SupervisorAlgorithms.cpp
        SupervisorAlgorithms.h
        ActorProxy.cpp
        ActorProxy.h
        ActorPath.cpp
        ActorPath.h
        ActorSystem.cpp
        ActorSystem.h
        ActorPolicy.cpp
        ActorPolicy.h
        ActorData.h
        SupervisorPolicy.cpp
        SupervisorPolicy.h
        SupervisorData.cpp
        SupervisorData.h
        ActorController.h
        Processor.h
        ActorControllerData.h
        ActorControllerPolicy.cpp
        ActorControllerPolicy.h
        ActorComposer.h
        )

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
