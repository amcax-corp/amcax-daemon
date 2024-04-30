cmake_minimum_required(VERSION 3.19)

#[[
    NOTICE
    --------
    Since Qt official CMake modules sets private header directory variables when you call `find_package(Qt)`
    only if the Qt targets hasn't been defined, if we place `find_package(Qt)` in a function, the variable
    will be cleared while the target remains after the function returns, as a result, we can never get the
    private header directory variables again.

    Therefore, never wrap `find_package(Qt)` in a function, use macro instead, any macros that wraps it also
    shouldn't be wrapped in any function.
]]#

set(QMSETUP_MODULES_DIR ${CMAKE_CURRENT_LIST_DIR})

if(WIN32)
    set(QMSETUP_SHARED_LIBRARY_CATEGORY bin)
    set(QMSETUP_NULL_FILE "NUL")
    set(QMSETUP_REGEX_ABSOLUTE_PATH "^[a-zA-Z]:|\\\\")
else()
    set(QMSETUP_SHARED_LIBRARY_CATEGORY lib)
    set(QMSETUP_NULL_FILE "/dev/null")
    set(QMSETUP_REGEX_ABSOLUTE_PATH "^/")
endif()

set(QMSETUP_IGNORE_STDOUT > ${QMSETUP_NULL_FILE})
set(QMSETUP_IGNORE_STDERR 2> ${QMSETUP_NULL_FILE})
set(QMSETUP_IGNORE_STDOUT_STDERR > ${QMSETUP_NULL_FILE} 2>&1)

if(TARGET qmsetup::corecmd)
    get_target_property(QMSETUP_CORECMD_EXECUTABLE qmsetup::corecmd LOCATION)
else()
    set(QMSETUP_CORECMD_EXECUTABLE)
endif()

if(NOT DEFINED QMSETUP_FIND_QT_ORDER)
    set(QMSETUP_FIND_QT_ORDER Qt6 Qt5)
endif()

include_guard(DIRECTORY)

#[[
    Include modules of this library.

    qm_import(<module...>)
]] #
macro(qm_import)
    foreach(_module ${ARGN})
        if(NOT _module MATCHES ".+\\.cmake")
            set(_module "${_module}.cmake")
        endif()

        set(_module_path "${QMSETUP_MODULES_DIR}/modules/${_module}")

        if(NOT EXISTS "${_module_path}")
            message(FATAL_ERROR "qm_import: module \"${_module}\" not found.")
        endif()

        include("${_module_path}")
    endforeach()
endmacro()

#[[
    Include all modules of this library.

    qm_import_all()
]] #
macro(qm_import_all)
    file(GLOB _tmp_modules "${QMSETUP_MODULES_DIR}/modules/*")

    foreach(_module IN LISTS _tmp_modules)
        include("${_module}")
    endforeach()

    unset(_tmp_modules)
endmacro()

#[[
    Skip CMAKE_AUTOMOC for sources files or ones in directories.

    qm_skip_automoc(<file/dir...>)
]] #
function(qm_skip_automoc)
    foreach(_item ${ARGN})
        get_filename_component(_item ${_item} ABSOLUTE)

        if(IS_DIRECTORY ${_item})
            file(GLOB _src ${_item}/*.h ${_item}/*.hh ${_item}/*.hpp ${_item}/*.hxx ${_item}/*.c ${_item}/*.cc ${_item}/*.cpp ${_item}/*.cxx ${_item}/*.m ${_item}/*.mm)
            set_source_files_properties(
                ${_src} PROPERTIES SKIP_AUTOMOC ON
            )
        elseif(EXISTS ${_item})
            set_source_files_properties(
                ${_item} PROPERTIES SKIP_AUTOMOC ON
            )
        endif()
    endforeach()
endfunction()

#[[
    Find Qt libraries. Don't wrap it in any functions.

    qm_find_qt(<modules...>)
#]]
macro(qm_find_qt)
    foreach(_module ${ARGN})
        if(NOT QT_VERSION_MAJOR)
            find_package(QT NAMES ${QMSETUP_FIND_QT_ORDER} COMPONENTS ${_module} REQUIRED)
        endif()

        if(NOT TARGET Qt${QT_VERSION_MAJOR}::${_module})
            find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${_module} REQUIRED)
        endif()
    endforeach()
endmacro()

#[[
    Link Qt libraries. Don't wrap it in any functions.

    qm_link_qt(<target> <scope> <modules...>)
#]]
macro(qm_link_qt _target _scope)
    foreach(_module ${ARGN})
        qm_find_qt(${_module})
        target_link_libraries(${_target} ${_scope} Qt${QT_VERSION_MAJOR}::${_module})
    endforeach()
endmacro()

#[[
    Include Qt private header directories. Don't wrap it in any functions.

    qm_include_qt_private(<target> <scope> <modules...>)
#]]
macro(qm_include_qt_private _target _scope)
    foreach(_module ${ARGN})
        qm_find_qt(${_module})
        target_include_directories(${_target} ${_scope} ${Qt${QT_VERSION_MAJOR}${_module}_PRIVATE_INCLUDE_DIRS})
    endforeach()
endmacro()

#[[
    Helper to link libraries and include directories of a target. Don't wrap it in any functions.

    qm_configure_target(<target>
        [SOURCES          <files>]
        [LINKS            <libs>]
        [LINKS_PRIVATE    <libs>]
        [INCLUDE_PRIVATE  <dirs>]

        [DEFINES          <defs>]
        [DEFINES_PRIVATE  <defs>]

        [CCFLAGS          <flags>]
        [CCFLAGS_PRIVATE  <flags>]

        [QT_LINKS            <modules>]
        [QT_LINKS_PRIVATE    <modules>]
        [QT_INCLUDE_PRIVATE  <modules>]

        [SKIP_AUTOMOC   <dir/file...>]
    )
]] #
macro(qm_configure_target _target)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs
        SOURCES LINKS LINKS_PRIVATE
        QT_LINKS QT_LINKS_PRIVATE QT_INCLUDE_PRIVATE
        INCLUDE_PRIVATE
        DEFINES DEFINES_PRIVATE
        CCFLAGS CCFLAGS_PUBLIC
        SKIP_AUTOMOC
    )
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    target_sources(${_target} PRIVATE ${FUNC_SOURCES})
    target_link_libraries(${_target} PUBLIC ${FUNC_LINKS})
    target_link_libraries(${_target} PRIVATE ${FUNC_LINKS_PRIVATE})
    target_compile_definitions(${_target} PUBLIC ${FUNC_DEFINES})
    target_compile_definitions(${_target} PRIVATE ${FUNC_DEFINES_PRIVATE})
    target_compile_options(${_target} PUBLIC ${FUNC_CCFLAGS_PUBLIC})
    target_compile_options(${_target} PRIVATE ${FUNC_CCFLAGS})
    qm_link_qt(${_target} PUBLIC ${FUNC_QT_LINKS})
    qm_link_qt(${_target} PRIVATE ${FUNC_QT_LINKS_PRIVATE})
    target_include_directories(${_target} PRIVATE ${FUNC_INCLUDE_PRIVATE})
    qm_include_qt_private(${_target} PRIVATE ${FUNC_QT_INCLUDE_PRIVATE})
    qm_skip_automoc(${FUNC_SKIP_AUTOMOC})
endmacro()

#[[
    Helper to define export macros.

    qm_export_defines(<target>
        [PREFIX     <prefix>]
        [STATIC     <token>]
        [LIBRARY    <token>]
    )
]] #
function(qm_export_defines _target)
    set(options)
    set(oneValueArgs PREFIX STATIC LIBRARY)
    set(multiValueArgs)
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT FUNC_PREFIX)
        string(TOUPPER ${_target} _prefix)
    else()
        set(_prefix ${FUNC_PREFIX})
    endif()

    qm_set_value(_static_macro FUNC_STATIC ${_prefix}_STATIC)
    qm_set_value(_library_macro FUNC_LIBRARY ${_prefix}_LIBRARY)

    get_target_property(_type ${_target} TYPE)

    if("${_type}" STREQUAL "STATIC_LIBRARY")
        target_compile_definitions(${_target} PUBLIC ${_static_macro})
    endif()

    target_compile_definitions(${_target} PRIVATE ${_library_macro})
endfunction()

#[[
    Attach windows RC file to a target.

    qm_add_win_rc(<target>
        [NAME           name]
        [VERSION        version]
        [DESCRIPTION    desc]
        [COPYRIGHT      copyright]
        [ICON           ico]
        [OUTPUT         output]
    )
]] #
function(qm_add_win_rc _target)
    if(NOT WIN32)
        return()
    endif()

    _qm_check_target_type_helper(${_target} _ "EXECUTABLE" "SHARED_LIBRARY")

    set(options)
    set(oneValueArgs NAME VERSION DESCRIPTION COPYRIGHT ICON OUTPUT)
    set(multiValueArgs)
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    qm_set_value(_name FUNC_NAME ${_target})
    qm_set_value(_version FUNC_VERSION PROJECT_VERSION "0.0.0.0")
    qm_set_value(_desc FUNC_DESCRIPTION ${_name})
    qm_set_value(_copyright FUNC_COPYRIGHT ${_name})

    qm_parse_version(_ver ${_version})
    set(RC_VERSION ${_ver_1},${_ver_2},${_ver_3},${_ver_4})

    set(RC_APPLICATION_NAME ${_name})
    set(RC_VERSION_STRING ${_version})
    set(RC_DESCRIPTION ${_desc})
    set(RC_COPYRIGHT ${_copyright})

    if(NOT FUNC_ICON)
        set(RC_ICON_COMMENT "//")
        set(RC_ICON_PATH)
    else()
        get_filename_component(RC_ICON_PATH ${FUNC_ICON} ABSOLUTE)
    endif()

    qm_set_value(_out_path FUNC_OUTOUT "${CMAKE_CURRENT_BINARY_DIR}/${_name}_res.rc")
    configure_file("${QMSETUP_MODULES_DIR}/windows/WinResource.rc.in" ${_out_path} @ONLY)
    target_sources(${_target} PRIVATE ${_out_path})
endfunction()

#[[
    Attach windows RC file to a target, enhanced edition.

    qm_add_win_rc_enhanced(<target>
        [NAME              name]
        [VERSION           version]
        [DESCRIPTION       description]
        [COPYRIGHT         copyright]
        [COMMENTS          comments]
        [COMPANY           company]
        [INTERNAL_NAME     internal name]
        [TRADEMARK         trademark]
        [ORIGINAL_FILENAME original filename]
        [ICONS             icon file paths]
        [OUTPUT            output]
    )
]] #
function(qm_add_win_rc_enhanced _target)
    if(NOT WIN32)
        return()
    endif()

    _qm_check_target_type_helper(${_target} _type "EXECUTABLE" "SHARED_LIBRARY")

    set(options)
    set(oneValueArgs
        NAME VERSION DESCRIPTION COPYRIGHT COMMENTS COMPANY
        INTERNAL_NAME TRADEMARK ORIGINAL_FILENAME OUTPUT
    )
    set(multiValueArgs ICONS)
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    qm_set_value(_name FUNC_NAME ${_target})
    qm_set_value(_version FUNC_VERSION PROJECT_VERSION "0.0.0.0")
    qm_set_value(_desc FUNC_DESCRIPTION ${_name})
    qm_set_value(_copyright FUNC_COPYRIGHT ${_name})
    qm_set_value(_comments FUNC_COMMENTS "")
    qm_set_value(_company FUNC_COMPANY "")
    qm_set_value(_internal_name FUNC_INTERNAL_NAME "")
    qm_set_value(_trademark FUNC_TRADEMARK "")
    qm_set_value(_original_filename FUNC_ORIGINAL_FILENAME "")

    qm_parse_version(_ver ${_version})
    set(RC_VERSION ${_ver_1},${_ver_2},${_ver_3},${_ver_4})

    set(RC_APPLICATION_NAME ${_name})
    set(RC_VERSION_STRING ${_version})
    set(RC_DESCRIPTION ${_desc})
    set(RC_COPYRIGHT ${_copyright})
    set(RC_COMMENTS ${_comments})
    set(RC_COMPANY ${_company})
    set(RC_INTERNAL_NAME ${_internal_name})
    set(RC_TRADEMARK ${_trademark})
    set(RC_ORIGINAL_FILENAME ${_original_filename})

    set(_file_type)

    if("${_type}" STREQUAL "EXECUTABLE")
        set(_file_type "VFT_APP")
    else()
        set(_file_type "VFT_DLL")
    endif()

    set(RC_FILE_TYPE ${_file_type})

    set(_icons)

    if(FUNC_ICONS)
        set(_index 1)

        foreach(_icon IN LISTS FUNC_ICONS)
            get_filename_component(_icon_path ${_icon} ABSOLUTE)
            string(APPEND _icons "IDI_ICON${_index}    ICON    \"${_icon_path}\"\n")
            math(EXPR _index "${_index} +1")
        endforeach()
    endif()

    set(RC_ICONS ${_icons})

    qm_set_value(_out_path FUNC_OUTOUT "${CMAKE_CURRENT_BINARY_DIR}/${_name}_res.rc")
    configure_file("${QMSETUP_MODULES_DIR}/windows/WinResource2.rc.in" ${_out_path} @ONLY)
    target_sources(${_target} PRIVATE ${_out_path})
endfunction()

#[[
    Attach windows manifest file to a target.

    qm_add_win_manifest(<target>
        [UTF8]
        [NAME           name]
        [VERSION        version]
        [DESCRIPTION    desc]
        [OUTPUT         output]
    )
]] #
function(qm_add_win_manifest _target)
    if(NOT WIN32)
        return()
    endif()

    _qm_check_target_type_helper(${_target} _ "EXECUTABLE")

    set(options UTF8)
    set(oneValueArgs NAME VERSION DESCRIPTION OUTPUT)
    set(multiValueArgs)
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    qm_set_value(_name FUNC_NAME ${_target})
    qm_set_value(_version FUNC_VERSION PROJECT_VERSION "0.0.0.0")
    qm_set_value(_desc FUNC_DESCRIPTION ${_name})

    qm_crop_version(_version ${_version} 4)

    set(MANIFEST_IDENTIFIER ${_name})
    set(MANIFEST_VERSION ${_version})
    set(MANIFEST_DESCRIPTION ${_desc})

    set(MANIFEST_UTF8)

    if(FUNC_UTF8)
        set(MANIFEST_UTF8 "<activeCodePage xmlns=\"http://schemas.microsoft.com/SMI/2019/WindowsSettings\">UTF-8</activeCodePage>")
    endif()

    qm_set_value(_out_path FUNC_OUTOUT "${CMAKE_CURRENT_BINARY_DIR}/${_name}_manifest.manifest")
    configure_file("${QMSETUP_MODULES_DIR}/windows/WinManifest.manifest.in" ${_out_path} @ONLY)
    target_sources(${_target} PRIVATE ${_out_path})
endfunction()

#[[
    Add Mac bundle info.

    qm_add_mac_bundle(<target>
        [NAME           <name>]
        [VERSION        <version>]
        [DESCRIPTION    <desc>]
        [COPYRIGHT      <copyright>]
        [ICON           <file>]
    )
]] #
function(qm_add_mac_bundle _target)
    if(NOT APPLE)
        return()
    endif()

    _qm_check_target_type_helper(${_target} _ "EXECUTABLE")

    set(options)
    set(oneValueArgs NAME VERSION DESCRIPTION COPYRIGHT ICON)
    set(multiValueArgs)
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    qm_set_value(_app_name FUNC_NAME ${_target})
    qm_set_value(_app_version FUNC_VERSION PROJECT_VERSION "0.0.0.0")
    qm_set_value(_app_desc FUNC_DESCRIPTION ${_app_name})
    qm_set_value(_app_copyright FUNC_COPYRIGHT ${_app_name})

    qm_parse_version(_app_version ${_app_version})

    # configure mac plist
    set_target_properties(${_target} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME ${_app_name}
        MACOSX_BUNDLE_EXECUTABLE_NAME ${_app_name}
        MACOSX_BUNDLE_INFO_STRING ${_app_desc}
        MACOSX_BUNDLE_GUI_IDENTIFIER ${_app_name}
        MACOSX_BUNDLE_BUNDLE_VERSION ${_app_version}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${_app_version_1}.${_app_version_2}
        MACOSX_BUNDLE_COPYRIGHT ${_app_copyright}
    )

    if(FUNC_ICON)
        # And this part tells CMake where to find and install the file itself
        set_source_files_properties(${FUNC_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
        )

        # NOTE: Don't include the path in MACOSX_BUNDLE_ICON_FILE -- this is
        # the property added to Info.plist
        get_filename_component(_icns_name ${FUNC_ICON} NAME)

        # configure mac plist
        set_target_properties(${_target} PROPERTIES
            MACOSX_BUNDLE_ICON_FILE ${_icns_name}
        )

        # ICNS icon MUST be added to executable's sources list, for some reason
        # Only apple can do
        target_sources(${_target} PRIVATE ${FUNC_ICON})
    endif()
endfunction()

#[[
    Generate Windows shortcut after building target.

    qm_create_win_shortcut(<target> <dir>
        [OUTPUT_NAME <name]
    )
]] #
function(qm_create_win_shortcut _target _dir)
    if(NOT WIN32)
        return()
    endif()

    set(options)
    set(oneValueArgs OUTPUT_NAME)
    set(multiValueArgs)
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    qm_set_value(_output_name FUNC_OUTPUT_NAME $<TARGET_FILE_BASE_NAME:${_target}>)

    set(_vbs_name ${CMAKE_CURRENT_BINARY_DIR}/${_target}_shortcut_$<CONFIG>.vbs)
    set(_vbs_temp ${_vbs_name}.in)

    set(_lnk_path "${_dir}/${_output_name}.lnk")

    set(SHORTCUT_PATH ${_lnk_path})
    set(SHORTCUT_TARGET_PATH $<TARGET_FILE:${_target}>)
    set(SHORTCUT_WORKING_DIRECOTRY $<TARGET_FILE_DIR:${_target}>)
    set(SHORTCUT_DESCRIPTION $<TARGET_FILE_BASE_NAME:${_target}>)
    set(SHORTCUT_ICON_LOCATION $<TARGET_FILE:${_target}>)

    configure_file(
        "${QMSETUP_MODULES_DIR}/windows/WinCreateShortcut.vbs.in"
        ${_vbs_temp}
        @ONLY
    )
    file(GENERATE OUTPUT ${_vbs_name} INPUT ${_vbs_temp})

    add_custom_command(
        TARGET ${_target} POST_BUILD
        COMMAND cscript ${_vbs_name} ${QMSETUP_IGNORE_STDOUT}
        BYPRODUCTS ${_lnk_path}
        VERBATIM
    )
endfunction()

#[[
    Parse version and create seq vars with specified prefix.

    qm_parse_version(<prefix> <version>)
]] #
function(qm_parse_version _prefix _version)
    string(REPLACE "." ";" _version_list ${_version})
    list(LENGTH _version_list _version_count)
    list(PREPEND _version_list 0) # Add placeholder

    foreach(_i RANGE 1 4)
        if(_i LESS_EQUAL _version_count)
            list(GET _version_list ${_i} _item)
        else()
            set(_item 0)
        endif()

        set(${_prefix}_${_i} ${_item} PARENT_SCOPE)
    endforeach()
endfunction()

#[[
    Get shorter version number.

    qm_crop_version(<VAR> <version> <count>)
]] #
function(qm_crop_version _var _version _count)
    qm_parse_version(FUNC ${_version})

    set(_list)

    foreach(_i RANGE 1 ${_count})
        list(APPEND _list ${FUNC_${_i}})
    endforeach()

    string(JOIN "." _short_version ${_list})
    set(${_var} ${_short_version} PARENT_SCOPE)
endfunction()

#[[
    Tell if there are any generator expressions in the string.

    qm_has_genex(<VAR> <string>)
]] #
function(qm_has_genex _out _str)
    string(GENEX_STRIP "${_str}" _no_genex)

    if("${_str}" STREQUAL "${_no_genex}")
        set(_res off)
    else()
        set(_res on)
    endif()

    set(${_out} ${_res} PARENT_SCOPE)
endfunction()

#[[
    Tell if the given paths are same in canonical form.

    qm_paths_equal(<VAR> <path1> <path2>)
]] #
function(qm_paths_equal _out _path1 _path2)
    # cmake_path(NORMAL_PATH) is introduced in CMake 3.20, we don't use it
    # We call `get_filename_component` twice to normalize the paths
    get_filename_component(_path1 ${_path1} ABSOLUTE)
    get_filename_component(_path1 ${_path1} REALPATH)

    get_filename_component(_path2 ${_path2} ABSOLUTE)
    get_filename_component(_path2 ${_path2} REALPATH)

    if(_path2 STREQUAL _path2)
        set(${_out} on PARENT_SCOPE)
    else()
        set(${_out} off PARENT_SCOPE)
    endif()
endfunction()

#[[
    Set value if valid, otherwise use default.

    qm_set_value(<key> <maybe_value...> <default>)
]] #
function(qm_set_value _key)
    set(_args "${ARGN}")
    list(POP_BACK _args _default)

    foreach(_item ${_args})
        if(${_item})
            set(${_key} ${${_item}} PARENT_SCOPE)
            return()
        endif()
    endforeach()

    set(${_key} ${_default} PARENT_SCOPE)
endfunction()

#[[
    Collect targets of given types recursively in a directory.

    qm_collect_targets(<list> [DIRECTORY directory]
                              [EXECUTABLE] [SHARED] [STATIC] [UTILITY])

    If one or more types are specified, return targets matching the types.
    If no type is specified, return all targets.
]] #
function(qm_collect_targets _var)
    set(options EXECUTABLE SHARED STATIC UTILITY)
    set(oneValueArgs DIR)
    set(multiValueArgs)
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(FUNC_DIRECTORY)
        set(_dir ${FUNC_DIRECTORY})
    else()
        set(_dir ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(_tmp_targets)

    macro(get_targets_recursive _targets _dir)
        get_property(_subdirs DIRECTORY ${_dir} PROPERTY SUBDIRECTORIES)

        foreach(_subdir ${_subdirs})
            get_targets_recursive(${_targets} ${_subdir})
        endforeach()

        get_property(_current_targets DIRECTORY ${_dir} PROPERTY BUILDSYSTEM_TARGETS)
        list(APPEND ${_targets} ${_current_targets})
    endmacro()

    # Get targets
    get_targets_recursive(_tmp_targets ${_dir})
    set(_targets)

    if(NOT FUNC_EXECUTABLE AND NOT FUNC_SHARED AND NOT FUNC_STATIC AND NOT FUNC_UTILITY)
        set(_targets ${_tmp_targets})
    else()
        # Filter targets
        foreach(_item ${_tmp_targets})
            get_target_property(_type ${_item} TYPE)

            if("${_type}" STREQUAL "EXECUTABLE")
                if(FUNC_EXECUTABLE)
                    list(APPEND _targets ${_item})
                endif()
            elseif("${_type}" STREQUAL "SHARED_LIBRARY")
                if(FUNC_SHARED)
                    list(APPEND _targets ${_item})
                endif()
            elseif("${_type}" STREQUAL "STATIC_LIBRARY")
                if(FUNC_STATIC)
                    list(APPEND _targets ${_item})
                endif()
            elseif("${_type}" STREQUAL "UTILITY")
                if(FUNC_UTILITY)
                    list(APPEND _targets ${_item})
                endif()
            endif()
        endforeach()
    endif()

    set(${_var} ${_targets} PARENT_SCOPE)
endfunction()

#[[
    Get subdirectories' names or paths.

    qm_get_subdirs(<list>
        [DIRECTORY dir]
        [EXCLUDE names...]
        [REGEX_INCLUDE exps...]
        [REGEX_EXLCUDE exps...]
        [RELATIVE path]
        [ABSOLUTE]
    )

    If `DIRECTORY` is not specified, consider `CMAKE_CURRENT_SOURCE_DIR`.
    If `RELATIVE` is specified, return paths evaluated as a relative path to it.
    If `ABSOLUTE` is specified, return absolute paths.
    If neither of them is specified, return names.
]] #
function(qm_get_subdirs _var)
    set(options ABSOLUTE)
    set(oneValueArgs DIRECTORY RELATIVE)
    set(multiValueArgs EXCLUDE REGEX_EXLCUDE)
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(FUNC_DIRECTORY)
        get_filename_component(_dir ${FUNC_DIRECTORY} ABSOLUTE)
    else()
        set(_dir ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    file(GLOB _subdirs LIST_DIRECTORIES true RELATIVE ${_dir} "${_dir}/*")

    if(FUNC_EXCLUDE)
        foreach(_exclude_dir ${FUNC_EXCLUDE})
            list(REMOVE_ITEM _subdirs ${_exclude_dir})
        endforeach()
    endif()

    if(FUNC_REGEX_INCLUDE)
        foreach(_exp ${FUNC_REGEX_INCLUDE})
            list(FILTER _subdirs INCLUDE REGEX ${_exp})
        endforeach()
    endif()

    if(FUNC_REGEX_EXCLUDE)
        foreach(_exp ${FUNC_REGEX_EXCLUDE})
            list(FILTER _subdirs EXCLUDE REGEX ${_exp})
        endforeach()
    endif()

    set(_res)

    if(FUNC_RELATIVE)
        get_filename_component(_relative ${FUNC_RELATIVE} ABSOLUTE)
    else()
        set(_relative)
    endif()

    foreach(_sub ${_subdirs})
        if(IS_DIRECTORY ${_dir}/${_sub})
            if(FUNC_ABSOLUTE)
                list(APPEND _res ${_dir}/${_sub})
            elseif(_relative)
                file(RELATIVE_PATH _rel_path ${_relative} ${_dir}/${_sub})
                list(APPEND _res ${_rel_path})
            else()
                list(APPEND _res ${_sub})
            endif()
        endif()
    endforeach()

    set(${_var} ${_res} PARENT_SCOPE)
endfunction()

# ----------------------------------
# Private functions
# ----------------------------------
macro(_qm_check_target_type_helper _target _type)
    set(_tmp_target_type_list ${ARGN})
    get_target_property(_tmp_target_type ${_target} TYPE)

    if(NOT "${_tmp_target_type}" IN_LIST _tmp_target_type_list)
        return()
    endif()

    set(${_type} ${_tmp_target_type})
    unset(_tmp_target_type)
    unset(_tmp_target_type_list)
endmacro()
