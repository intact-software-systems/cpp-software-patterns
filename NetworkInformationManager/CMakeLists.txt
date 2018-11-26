cmake_minimum_required(VERSION 3.3.2)
project(NetworkInformationManager)

set(SOURCE_FILES
    IncludeExtLibs.h
    main.cpp
#    pl_vivaldi.cc
#    pl_vivaldi.h
    VivaldiManager.cpp
    VivaldiManager.h)

#add_definitions(-DMAKE_DLL_NetworkInformationManager)

SET(EXTRA_LIBS BaseLib NetworkLib MicroMiddleware IntactMiddleware NetworkInformationLib GraphLib)
include_directories(${SOURCE_FILES})
add_executable(NetworkInformationManager ${SOURCE_FILES})
target_link_libraries(NetworkInformationManager ${EXTRA_LIBS})

