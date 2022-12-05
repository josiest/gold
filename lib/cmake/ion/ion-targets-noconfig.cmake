#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ion::ion" for configuration ""
set_property(TARGET ion::ion APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(ion::ion PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C;CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/ion/libion.a"
  )

list(APPEND _cmake_import_check_targets ion::ion )
list(APPEND _cmake_import_check_files_for_ion::ion "${_IMPORT_PREFIX}/lib/ion/libion.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
