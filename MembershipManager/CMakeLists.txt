cmake_minimum_required(VERSION 3.3.2)
project(MembershipManager)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    ClientDynamicsManager.cpp
    ClientDynamicsManager.h
    Export.h
    GroupManager.cpp
    GroupManager.h
    GroupManagerConnection.cpp
    GroupManagerConnection.h
    IncludeExtLibs.h
    IncludeLibs.h
    MembershipManager.cpp
    MembershipManager.h
    MembershipManagerConnection.cpp
    MembershipManagerConnection.h)

add_executable(MembershipManager ${SOURCE_FILES})