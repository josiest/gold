cmake_minimum_required(VERSION 3.10)
project(simple-gold-example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

find_package(gold REQUIRED)

file(GLOB sources *.cc)
add_executable(example ${sources})

include_directories(gold::gold SDL2::Core)
target_link_libraries(example gold::gold)
