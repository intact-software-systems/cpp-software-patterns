cmake_minimum_required(VERSION 3.2)

SET(LIB_NAME RxObserver)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData RxTransport)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

project(${LIB_NAME})
add_definitions(-DMAKE_DLL_${LIB_NAME})

set(SOURCE_FILES
        IncludeLibs.h
        Export.h
        IncludeExtLibs.h
        EventBase.h
        EventFilterCriterion.h
        EventPolicy.cpp
        EventPolicy.h
        EventResult.cpp
        EventResult.h
        EventSource.h
        Observer.cpp
        Observer.h
        ObserverBase.h
        ObserverEvent.cpp
        ObserverEvent.h
        ObserverNoOp.h
        ObserverPolicy.cpp
        ObserverPolicy.h
        ObserverProxy.h
        Observers.cpp
        Observers.h
        Subject.cpp
        Subject.h
        Subject0.h
        Subject1.h
        Subject2.h
        SubjectBase.cpp
        SubjectBase.h
        SubjectDescription.cpp
        SubjectDescription.h
        SubjectFactory.cpp
        SubjectFactory.h
        SubjectGroup.h
        SubjectId.cpp
        SubjectId.h
        SubjectPolicy.cpp
        SubjectPolicy.h
        Subjects.cpp
        Subjects.h
        SubjectsProxy.h
        Subscription.h
        SubscriptionBase.h
        Subscriptions.h
        FlowControlStrategy.cpp
        FlowControlStrategy.h
        SubjectStrategy.cpp
        SubjectStrategy.h
        RxObserver.h
        RxObserverSubject.h
        KeyValueRxObserver.h
        KeyValueRxEventsSubject.h)

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
