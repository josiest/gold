cmake_minimum_required(VERSION 3.13)
project(gold)

# relase versioning
set(gold_MAJOR_VERSION 0)
set(gold_MINOR_VERSION 0)
set(gold_PATCH_VERSION 0)

# pre-release versioning
set(gold_PRE_MAJOR_VERSION 2)
set(gold_PRE_MINOR_VERSION 0)
set(gold_PRE_PATCH_VERSION 3)

set(gold_VERSION
"${gold_MAJOR_VERSION}.${gold_MINOR_VERSION}.${gold_PATCH_VERSION}-\
${gold_PRE_MAJOR_VERSION}.${gold_PRE_MINOR_VERSION}.${gold_PRE_PATCH_VERSION}")

# make cache variables for install destinations
include(GNUInstallDirs)

# compiler flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# specify the file interface
set(gold_SRCEXT cc)
set(gold_HEXT h)
set(gold_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# define the library
add_subdirectory(src)

# --- create the library package ---

# includehelper functions for creating config files
include(CMakePackageConfigHelpers)

# set versions
set_property(TARGET gold PROPERTY VERSION ${gold_VERSION})
set_property(TARGET gold
             PROPERTY INTERFACE_gold_MAJOR_VERSION ${gold_MAJOR_VERSION})

# must be compatible with dependencies of any depender
set_property(TARGET gold
             APPEND PROPERTY COMPATIBLE_INTERFACE_STRING gold_MAJOR_VERSION)

# generate the version file for the config file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/goldConfigVersion.cmake
    VERSION ${gold_VERSION}
    COMPATIBILITY AnyNewerVersion)

# generate the package configuration file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/goldConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gold)

# install the generated files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/goldConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/goldConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gold)

# install the include files
install(DIRECTORY ${gold_INCLUDE_DIR}/gold
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN *.${gold_HEXT})

# install cmake sdl2 modules
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2/
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gold/Modules
        FILES_MATCHING PATTERN *.cmake)

if (UNIX)
    set(CMAKE_INSTALL_PREFIX /usr/local)
elseif(WIN32)
    set(CMAKE_INSTALL_PREFIX "C:\\Program Files")
endif()
