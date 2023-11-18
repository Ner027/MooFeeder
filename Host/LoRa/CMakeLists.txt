cmake_minimum_required(VERSION 3.26)
project(LoRa)

set(CMAKE_CXX_STANDARD 23)

add_executable(LoRa
        main.cpp
        serial/src/serial.c
        rn2483/src/rn4283.c
        strlib/src/strlib.c
        ptwrap/src/cmutex.cpp
        ptwrap/src/cthread.cpp
        ptwrap/src/cuniquelock.cpp
        mac/server/src/cloramac.cpp
        mac/client/src/mac_client.cpp
        queue/inc/queue.hpp
)
