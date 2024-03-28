#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "FreeGLUT::freeglut" for configuration "RelWithDebInfo"
set_property(TARGET FreeGLUT::freeglut APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(FreeGLUT::freeglut PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/freeglut.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/freeglut.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS FreeGLUT::freeglut )
list(APPEND _IMPORT_CHECK_FILES_FOR_FreeGLUT::freeglut "${_IMPORT_PREFIX}/lib/freeglut.lib" "${_IMPORT_PREFIX}/bin/freeglut.dll" )

# Import target "FreeGLUT::freeglut_static" for configuration "RelWithDebInfo"
set_property(TARGET FreeGLUT::freeglut_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(FreeGLUT::freeglut_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C;RC"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/freeglut_static.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS FreeGLUT::freeglut_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_FreeGLUT::freeglut_static "${_IMPORT_PREFIX}/lib/freeglut_static.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
