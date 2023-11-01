cmake_minimum_required(VERSION 3.26)
project(SerializableObjects)

find_package(cpprestsdk REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(SerializableObjects
        json.hpp
        CSerializableObject/CSerializableObject.cpp
        CControlBox.cpp
        http.hpp
        main.cpp)

target_link_libraries(SerializableObjects cpprestsdk::cpprest)
