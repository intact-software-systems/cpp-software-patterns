cmake_minimum_required(VERSION 3.3)
SET(LIB_NAME RxActorTest)

project(${LIB_NAME})

SET(EXTRA_LIBS BaseLib RxConcurrent RxSignal RxData RxTransport RxActor)
SET(SOURCE_PATH ${ONE_SOURCE_DIR}/test/${LIB_NAME})

set(SOURCE_FILES RxActorTest.cpp)

include_directories(${ONE_SOURCE_DIR})
add_executable(${LIB_NAME} MACOSX_BUNDLE ${SOURCE_FILES})

target_link_libraries(${LIB_NAME} ${EXTRA_LIBS} ${GTEST_LIBS})

