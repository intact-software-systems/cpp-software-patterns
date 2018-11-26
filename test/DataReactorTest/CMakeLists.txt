cmake_minimum_required(VERSION 3.2)
project(DataReactorTest)

SET(LIB_NAME DataReactorTest)
SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData RxTransport RxObserver DataReactor RxCommand RxStreamer RxFSM RxMonitor)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/test/${LIB_NAME})

set(SOURCE_FILES
    main.cpp
    HandlerTest.h
    HandlerTest.cpp)

include_directories(${ONE_SOURCE_DIR})
#add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
add_executable(${LIB_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${EXTRA_LIBS})

