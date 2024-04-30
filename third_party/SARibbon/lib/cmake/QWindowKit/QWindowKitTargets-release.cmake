#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "QWindowKit::Core" for configuration "Release"
set_property(TARGET QWindowKit::Core APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(QWindowKit::Core PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/QWKCore.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/QWKCore.dll"
  )

list(APPEND _cmake_import_check_targets QWindowKit::Core )
list(APPEND _cmake_import_check_files_for_QWindowKit::Core "${_IMPORT_PREFIX}/lib/QWKCore.lib" "${_IMPORT_PREFIX}/bin/QWKCore.dll" )

# Import target "QWindowKit::Widgets" for configuration "Release"
set_property(TARGET QWindowKit::Widgets APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(QWindowKit::Widgets PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/QWKWidgets.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/QWKWidgets.dll"
  )

list(APPEND _cmake_import_check_targets QWindowKit::Widgets )
list(APPEND _cmake_import_check_files_for_QWindowKit::Widgets "${_IMPORT_PREFIX}/lib/QWKWidgets.lib" "${_IMPORT_PREFIX}/bin/QWKWidgets.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
