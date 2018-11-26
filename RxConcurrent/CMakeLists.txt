cmake_minimum_required(VERSION 3.2)

SET(LIB_NAME RxConcurrent)
SET(EXTRA_LIBS BaseLib)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

project(${LIB_NAME})
add_definitions(-DMAKE_DLL_${LIB_NAME})

set(SOURCE_FILES
        IncludeLibs.h
        Export.h
        IncludeExtLibs.h
        CommonDefines.h
        CommonDefines.cpp
        ThreadPool/IncludeLibs.h
        ThreadPool/ScheduledTask.cpp
        ThreadPool/ScheduledTask.h
        ThreadPool/ScheduledTasks.cpp
        ThreadPool/ScheduledTasks.h
        ThreadPool/TaskPolicy.h
        ThreadPool/TaskPolicy.cpp
        ThreadPool/ThreadPool.cpp
        ThreadPool/ThreadPool.h
        ThreadPool/ThreadPoolFactory.cpp
        ThreadPool/ThreadPoolFactory.h
        ThreadPool/ThreadPoolBase.h
        ThreadPool/ThreadPoolPolicy.cpp
        ThreadPool/ThreadPoolPolicy.h
        ThreadPool/RxThreadPool.cpp
        ThreadPool/RxThreadPool.h
        ThreadPool/RxThreadPoolFactory.cpp
        ThreadPool/RxThreadPoolFactory.h
        ThreadPool/Worker.cpp
        ThreadPool/Worker.h
        ThreadPool/Workers.cpp
        ThreadPool/Workers.h
        Strategy/ExecutionStrategy.h
        Strategy/ExecutionStrategy.cpp
        Strategy/MonitorWorkers.h
        Strategy/MonitorWorkers.cpp
        FlowController.h
        FlowControllerAction.h
        FlowControllerPolicy.cpp
        FlowControllerPolicy.h
        Memento.h
        Memento.cpp
        MementoSharedType.h
        MementoContextObjectPtr.h
        Condition.cpp
        Condition.h
        Scheduler.h
        ActionFactory.h
        FutureBase.h
        FutureFactory.h
        FutureTask.h
        FutureTasks.cpp
        FutureTasks.h
        GuardCondition.cpp
        GuardCondition.h
        IncludeLibs.h
        ScheduledFutureTask.h
        Schedulables.h
        StateMachineCollections.h
        StateMachineMethods.h
        StateMachineObjects.h
        StrategyFutureFactory.h
        WaitSet.cpp
        WaitSet.h
        Composer1.h
        Composer1.cpp)

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
