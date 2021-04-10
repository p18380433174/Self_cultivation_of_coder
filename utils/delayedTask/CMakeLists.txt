cmake_minimum_required(VERSION 2.8)

project(test)

set (source_file "delayedTask.cpp")

set(CMAKE_CXX_FLAGS "-g ${CMAKE_CXX_FLAGS}")

add_executable(main ${source_file})

target_link_libraries(main pthread)