# --- define the libarary ---

# define the source files
file(GLOB gold_SOURCES *.${gold_SRCEXT})

# define the library and its alias
add_library(gold STATIC ${gold_SOURCES})
add_library(gold::gold ALIAS gold)
target_include_directories(gold PUBLIC
    $<BUILD_INTERFACE:${gold_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# add dependent libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(yaml-cpp REQUIRED)
target_link_libraries(gold SDL2::Core SDL2::TTF yaml-cpp)

# --- export the library ---

# specify the library targets
install(TARGETS gold
        EXPORT goldTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/gold
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# install the export details
install(EXPORT goldTargets
        FILE goldTargets.cmake
        NAMESPACE gold::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gold)
