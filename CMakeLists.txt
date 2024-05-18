cmake_minimum_required(VERSION 3.5)

project(AmcaxDaemon VERSION 0.1 LANGUAGES CXX)

if(MSVC)
    add_compile_options(/utf-8)
    add_compile_options(/bigobj)
endif()

if(DEFINED CMAKE_PREFIX_PATH)
    message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
elseif(DEFINED ENV{CMAKE_PREFIX_PATH})
    set(CMAKE_PREFIX_PATH $ENV{CMAKE_PREFIX_PATH})
    message("CMAKE_PREFIX_PATH not defined, set to env: ${CMAKE_PREFIX_PATH}")
else()
    set(CMAKE_PREFIX_PATH "C:\\Qt\\5.15.2\\msvc2019_64\\lib\\cmake\\")
    message("CMAKE_PREFIX_PATH not defined, set to default: ${CMAKE_PREFIX_PATH}")
endif()

# Turn on automoc/autouic, must have
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SARIBBON_USE_FRAMELESS_LIB ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(./Resources/Version.rc.in ./Resources/Version.rc)

# find depended qt libraries
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
        Core
        Gui
        Widgets
        OpenGL
        Sql
        OpenGLWidgets
        REQUIRED
    )
else()
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
        Core
        Gui
        Widgets
        OpenGL
        Sql
        REQUIRED
    )
endif()

if(DEFINED CMAKE_BUILD_TYPE)
    message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
elseif(DEFINED ENV{CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE $ENV{CMAKE_BUILD_TYPE})
    message("CMAKE_BUILD_TYPE set to env: ${CMAKE_BUILD_TYPE}")
else()
    set(CMAKE_BUILD_TYPE "Release")
    message("CMAKE_BUILD_TYPE set to default: ${CMAKE_BUILD_TYPE}")
endif()

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/AMCAX/lib/cmake")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH})

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(AMCAX_DLL_PATH "AMCAX/bin/Debug/")
else()
    set(AMCAX_DLL_PATH "AMCAX/bin/Release/")
endif()

# find depended libraries
find_package(AMCAXCommon REQUIRED)
find_package(AMCAXPart REQUIRED)
find_package(AMCAXStep REQUIRED)
find_package(AMCAXSubD REQUIRED)
find_package(AMCAXTMSpline REQUIRED)
find_package(AMCAXOCCTIO REQUIRED)

qt_add_resources(QRC_FILES Resources/Resource.qrc)

# SARibbon FreeGLUT
add_subdirectory(third_party)
include_directories("third_party/FreeGLUT/include")
include_directories("third_party/SARibbon/src/SARibbonBar")

FILE(GLOB_RECURSE SOURCE_FILE "Src/*.cpp" "Src/*.h")
FILE(GLOB WIN32_RESOURCE_FILE "Resources/Version.rc")

add_compile_definitions(NOMINMAX)
add_compile_definitions(_USE_MATH_DEFINES)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AmcaxDaemon
        MANUAL_FINALIZATION
        ${QRC_FILES}
        ${SOURCE_FILE}
        ${WIN32_RESOURCE_FILE}
    )

    target_link_libraries(AmcaxDaemon PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

# Define target properties for Android with Qt 6 as:
# set_property(TARGET AmcaxDaemon APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(AmcaxDaemon SHARED
            ${SOURCE_FILE}
        )

    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(AmcaxDaemon
            ${QRC_FILES}
            ${SOURCE_FILE}
            ${WIN32_RESOURCE_FILE}
        )
    endif()
endif()

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
 
	target_link_libraries(AmcaxDaemon PRIVATE OpenGL GLU)
 
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
 
	target_link_libraries(AmcaxDaemon PRIVATE OpenGL32 GLU32)
 
else()
 
	message("current platform: unkonw ")
 
endif()

target_link_libraries(AmcaxDaemon PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::GuiPrivate
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::Sql
    AMCAXCommon
    AMCAXPart
    AMCAXStep
    AMCAXSubD
    AMCAXTMSpline
    freeglut
    SARibbonBar
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.AmcaxDaemon)
endif()

set_target_properties(AmcaxDaemon PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS AmcaxDaemon
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AmcaxDaemon)
endif()

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
 
    file(COPY ${AMCAX_DLL_PATH}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )
 
endif()

