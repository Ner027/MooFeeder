cmake_minimum_required(VERSION 3.21)
project(YRM1003)

set(CMAKE_CXX_STANDARD 23)

add_library(YRM1003_LIB serial/src/serial.c yrm100x/src/yrm100x.c)

add_executable(YRM1003 main.c)
target_link_libraries(YRM1003 YRM1003_LIB)

enable_testing()

file(GLOB TEST_FILES "adhoc/src/*.c")

foreach (file ${TEST_FILES})
    get_filename_component(FILE_NAME ${file} NAME_WE)
    add_executable(${FILE_NAME} ${file})
    target_link_libraries(${FILE_NAME} YRM1003_LIB)
    add_test(${FILE_NAME} ${FILE_NAME})
    message(${FILE_NAME})
endforeach ()
