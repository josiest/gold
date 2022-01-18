cmake_minimum_required(VERSION 3.10)
project(gui)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB sources src/*.cpp)
add_library(gui STATIC ${sources})

include_directories(include)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
target_link_libraries(gui SDL2::Core SDL2::TTF)
