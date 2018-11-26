cmake_minimum_required(VERSION 3.2)
project(RxFSM)

add_definitions(-DMAKE_DLL_RxFSM)

SET(LIB_NAME RxFSM)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData RxTransport RxObserver DataReactor RxCommand RxStreamer)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

set(SOURCE_FILES
        CommonDefines.h
        State.h
        Transition.h
        StateMachineController.h
        StateAction.h
        StateCommand.h
        StateTransitions.h
        States.h
        Export.h
        IncludeExtLibs.h
        IncludeLibs.h
        IncludeLibs.cpp)

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
#add_library(one::${LIB_NAME} ALIAS ${LIB_NAME})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
