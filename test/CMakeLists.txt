cmake_minimum_required(VERSION 3.3)
project(test)

include_directories(/usr/local/include)
SET(GTEST_LIBS /usr/local/lib/libgtest.a /usr/local/lib/libgtest_main.a)

FOREACH (libfolder DataReactorTest RxTransportTest RxActorTest)
    add_subdirectory(${libfolder})
ENDFOREACH ()

