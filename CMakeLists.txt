cmake_minimum_required(VERSION 3.23) # FILESETS
project(gold VERSION 0.2.0)

add_executable(sketch examples/ui-example.cpp)
target_include_directories(sketch PRIVATE
    include
    include/imgui
    include/imgui/backends)

set_target_properties(sketch PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE)

find_package(ion 0.12.0 EXACT REQUIRED PATHS lib/cmake)
target_link_libraries(sketch PRIVATE ion::ion)