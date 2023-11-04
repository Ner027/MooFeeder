cmake_minimum_required(VERSION 3.26)
project(PThreadsWrapper)

set(CMAKE_CXX_STANDARD 23)

add_executable(PThreadsWrapper
        cmutex.cpp
        cuniquelock.cpp
        cthread.cpp
        main.cpp
        cloramac.cpp)
