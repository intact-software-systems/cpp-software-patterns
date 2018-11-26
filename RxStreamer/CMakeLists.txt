cmake_minimum_required(VERSION 3.2)
project(RxStreamer)

add_definitions(-DMAKE_DLL_RxStreamer)

SET(LIB_NAME RxStreamer)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData RxTransport RxObserver DataReactor RxCommand)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/${LIB_NAME})

set(SOURCE_FILES
        DataAccess.h
        DataAccessController.h
        DataAccessController.cpp
        RxDataAccessController.h
        RxDataAccessController.cpp
        DataAccessGroup.h
        DataAccessGroup.cpp
        DataAccessGroupChain.h
        DataAccessGroupChain.cpp
        DataAccessGroupChain.cpp
        DataAccessGroupIterator.h
        DataAccessGroupIterator.cpp
        DataAccessGroupChainIterator.h
        DataAccessGroupChainIterator.cpp
        RxActionChain.h
        RxActionChain.cpp
        RxDataAccessGroup.h
        RxDataAccessGroup.cpp
        DataAccessStrategyPolicy.h
        Streamer.h
        Streamer.cpp
        StreamerGroup.h
        StreamerGroup.cpp
        ReactorFactory.cpp
        ReactorFactory.h
        AccessFactory.h
        AccessFactory.cpp
        DataAccessPolicy.h
        DataAccessPolicy.cpp
        DataAccessControllerPolicy.h
        DataAccessControllerPolicy.cpp
        DataAccessGroupPolicy.h
        DataAccessGroupPolicy.cpp
        Export.h
        IncludeExtLibs.h
        IncludeLibs.h
        )

include_directories(${ONE_SOURCE_DIR})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
#add_library(one::${LIB_NAME} ALIAS ${LIB_NAME})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})
