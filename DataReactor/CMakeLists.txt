cmake_minimum_required(VERSION 3.2)
project(DataReactor)

add_definitions(-DMAKE_DLL_DataReactor)

SET(LIB_NAME DataReactor)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData RxTransport RxObserver)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

set(SOURCE_FILES
        Condition/StateCondition.h
        Handler/RxHandler.h
        Handler/RxHandlerChain.h
        Observer/DataObserver.h
        Observer/KeyStateTriggerObserver.h
        Observer/StateObserver.h
        Observer/StateTriggerObserver.h
        Reactor/AsyncScheduler.h
        Reactor/RxScheduler.h
        Reactor/RxTaskExecutor.h
        Reactor/SyncScheduler.h
        Subject/KeyStateTriggerSubject.h
        Subject/StateTriggerSubject.h
        Export.h
        IncludeExtLibs.h
        IncludeLibs.h
        IncludeLibs.cpp)

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
#add_library(one::${LIB_NAME} ALIAS ${LIB_NAME})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
