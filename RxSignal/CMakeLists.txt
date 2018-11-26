cmake_minimum_required(VERSION 3.2)

SET(LIB_NAME RxSignal)
SET(EXTRA_LIBS BaseLib RxConcurrent)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

project(${LIB_NAME})
add_definitions(-DMAKE_DLL_${LIB_NAME})

set(SOURCE_FILES
        IncludeLibs.h
        Export.h
        IncludeExtLibs.h
        CommonDefines.h
        Serialize.h
        Serialize.cpp
        ObservableType.h
        Signaller.cpp
        Signaller.h
        Signaller0.h
        Signaller1.h
        Signaller2.h
        Signaller3.h
        SignallerAlgorithms.h
        SignallerAlgorithms.cpp
        SignalSlotBase.h
        Templates/SignalMethods.h
        Observer/CollectionObserver.h
        Observer/FilterObserver.h
        Observer/IncludeLibs.h
        Observer/LifecycleObserver.h
        Observer/QueueObserver.h
        Observer/RaceObserver.h
        Observer/RxObserver.h
        Observer/RxScheduleObserver.h
        Subject/CollectionSubject.h
        Subject/FilterSubject.h
        Subject/IncludeLibs.h
        Subject/LifecycleSubject.h
        Subject/QueueSubject.h
        Subject/RaceSubject.h
        Subject/RxObserverSubject.h
        Subject/RxScheduleSubject.h
        Collection/RingBufferQueueObservable.h
        Collection/RingBufferQueueMap.h
        )

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
