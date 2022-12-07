cmake_minimum_required(VERSION 3.23) # FILESETS
project(gold VERSION 0.2.0)

add_library(gold STATIC
    src/layout.cpp
    src/render.cpp)
target_sources(gold PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
    include/gold/layout.hpp
    include/gold/layout.tcc
    include/gold/background_color.hpp
    include/gold/size.hpp
    include/gold/utility.hpp
    include/gold/render.hpp)
set_target_properties(gold PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE)

add_executable(sketch examples/ui-example.cpp)
target_include_directories(sketch PRIVATE
    include
    include/imgui
    include/imgui/backends)

set_target_properties(sketch PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE)

find_package(ion 0.12.0 EXACT REQUIRED PATHS lib/cmake)
find_package(EnTT REQUIRED)
target_link_libraries(sketch
    PRIVATE ion::ion gold
    INTERFACE EnTT::EnTT)