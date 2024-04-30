
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was QWindowKitConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)

find_dependency(QT NAMES Qt6 Qt5 COMPONENTS Core Gui REQUIRED)
find_dependency(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/QWindowKitTargets.cmake")
