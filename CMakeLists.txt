cmake_minimum_required(VERSION 3.5)
project(AmcaxDaemon VERSION 1.0.0.2)
set(CMAKE_CXX_STANDARD 17)

configure_file(./Resources/Version.rc.in ./Resources/Version.rc)

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

# find depended qt libraries
find_package(Qt5 COMPONENTS Widgets Core Gui OpenGL sql REQUIRED)

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

if(DEFINED CMAKE_BUILD_TYPE)
    message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
elseif(DEFINED ENV{CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE $ENV{CMAKE_BUILD_TYPE})
    message("CMAKE_BUILD_TYPE set to env: ${CMAKE_BUILD_TYPE}")
else()
    set(CMAKE_BUILD_TYPE "Release")
    message("CMAKE_BUILD_TYPE set to default: ${CMAKE_BUILD_TYPE}")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/AMCAX/lib/cmake")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third_party/FreeGLUT/x64-Debug/lib/cmake")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH})

    set(AMCAX_DLL_PATH "AMCAX/bin/Debug/")
    set(FREEGLUT_DLL_PATH "third_party/FreeGLUT/x64-Debug/bin/")

    # SARibbon
    SET(SARIBBONBAR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/SARibbon/lib/SARibbonBard.lib")
    SET(QWKCORE_DLL "third_party/SARibbon/bin/QWKCored.dll")
    SET(QWKWidgets_DLL "third_party/SARibbon/bin/QWKWidgetsd.dll")
    SET(SARIBBONBAR_DLL "third_party/SARibbon/bin/SARibbonBard.dll")
else()
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/AMCAX/lib/cmake")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third_party/FreeGLUT/x64-Release/lib/cmake")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH})

    set(AMCAX_DLL_PATH "AMCAX/bin/Release/")
    set(FREEGLUT_DLL_PATH "third_party/FreeGLUT/x64-Release/bin/")

    # SARibbon
    SET(SARIBBONBAR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/SARibbon/lib/SARibbonBar.lib")
    SET(QWKCORE_DLL "third_party/SARibbon/bin/QWKCore.dll")
    SET(QWKWidgets_DLL "third_party/SARibbon/bin/QWKWidgets.dll")
    SET(SARIBBONBAR_DLL "third_party/SARibbon/bin/SARibbonBar.dll")
endif()

include_directories("third_party/SARibbon/include/SARibbonBar")

# find depended libraries
find_package(AMCAXCommon REQUIRED)
find_package(AMCAXPart REQUIRED)
find_package(AMCAXStep REQUIRED)
find_package(AMCAXSubD REQUIRED)
find_package(AMCAXTMSpline REQUIRED)
find_package(AMCAXOCCTIO REQUIRED)
find_package(FreeGLUT REQUIRED)

# find_package(SARibbonBar REQUIRED)
qt5_add_resources(QRC_FILES Resources/Resource.qrc)

FILE(GLOB_RECURSE SOURCE_FILE "Src/*.cpp" "Src/*.h")
FILE(GLOB WIN32_RESOURCE_FILE "Resources/Version.rc")

add_compile_definitions(NOMINMAX)
add_compile_definitions(_USE_MATH_DEFINES)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE ${QRC_FILES} ${SOURCE_FILE} ${WIN32_RESOURCE_FILE})

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
    Qt5::Sql
    opengl32
    glu32
    AMCAXCommon
    AMCAXPart
    AMCAXStep
    AMCAXSubD
    AMCAXTMSpline
    FreeGLUT::freeglut
    ${SARIBBONBAR}
)

message("CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${AMCAX_DLL_PATH} ${FREEGLUT_DLL_PATH} ${QWKCORE_DLL}
    ${QWKWidgets_DLL} ${SARIBBONBAR_DLL}
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)