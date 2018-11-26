cmake_minimum_required(VERSION 3.2)
project(RxCommand)

add_definitions(-DMAKE_DLL_RxCommand)

SET(LIB_NAME RxCommand)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData RxTransport RxObserver DataReactor)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

set(SOURCE_FILES
        Strategy/AggregateCompositionStrategy.h
        Strategy/CommandStrategyPolicy.h
        Strategy/CompositionStrategy.h
        Strategy/ComputationStrategy.h
        Strategy/ExecutionStrategy.h
        Strategy/IndividualCompositionStrategy.h
        Strategy/ParallelComputationStrategy.cpp
        Strategy/ParallelComputationStrategy.h
        Strategy/SequentialComputationStrategy.cpp
        Strategy/SequentialComputationStrategy.h
        ReactorConfig.h
        ReactorConfig.cpp
        MementoCommand.h
        Command.h
        CommandBase.h
        CommandController.cpp
        CommandController.h
        CommandControllerAction.h
        CommandControllerBase.h
        CommandControllerSubscription.h
        Commands.h
        Commands.cpp
        CommandsHandle.cpp
        CommandsHandle.h
        CommandSubscription.h
        MementoCommandController.h
        CommandFactory.cpp
        CommandFactory.h
        CommandControllerPolicy.cpp
        CommandControllerPolicy.h
        CommandPolicy.cpp
        CommandPolicy.h
        Export.h
        IncludeExtLibs.h
        IncludeLibs.h
        Strategy/AsyncCompositionStrategy.h
        Strategy/CompositionAlgorithms.h
        Strategy/ComputationAlgorithms.cpp
        Strategy/ComputationAlgorithms.h
        AsyncCompletionController.cpp
        AsyncCompletionController.h)

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
