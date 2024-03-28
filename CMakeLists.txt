cmake_minimum_required(VERSION 3.5)
project(AmcaxDaemon VERSION 1.0.0.2)
set(CMAKE_CXX_STANDARD 17)

configure_file(./Resources/Version.rc.in ./Resources/Version.rc)

if (MSVC)
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

#Turn on automoc/autouic, must have
set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
 
#find depended qt libraries 
find_package(Qt5 COMPONENTS Widgets Core Gui OpenGL REQUIRED)

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
    set(AMCAX_DLL_PATH "AMCAX/bin/Debug")
    set(FREEGLUT_DLL_PATH "third_party/FreeGLUT/x64-Debug/bin")
else()
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/AMCAX/lib/cmake")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third_party/FreeGLUT/x64-Release/lib/cmake")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH})
    set(AMCAX_DLL_PATH "AMCAX/bin/Release")
    set(FREEGLUT_DLL_PATH "third_party/FreeGLUT/x64-Release/bin")
endif()

message("CMAKE_PREFIX_PATH is now : ${CMAKE_PREFIX_PATH}")
#find depended libraries 
find_package(AMCAXCommon REQUIRED)
find_package(AMCAXPart REQUIRED)
find_package(AMCAXStep REQUIRED)
find_package(AMCAXSubD REQUIRED)
find_package(AMCAXTMSpline REQUIRED)
find_package(AMCAXOCCTIO REQUIRED)
find_package(FreeGLUT REQUIRED)



qt5_add_resources(QRC_FILES Resources/Resource.qrc)

 
FILE(GLOB Command_File "Src/Command/*.cpp" "Src/Command/*.h") 

FILE(GLOB Core_FILE "Src/Core/*.cpp" "Src/Core/*.h") 

FILE(GLOB Object_FILE "Src/Object/*.cpp" "Src/Object/*.h") 

FILE(GLOB ObjectDraw_FILE "Src/ObjectDraw/*.cpp" "Src/ObjectDraw/*.h") 

FILE(GLOB Operate_FILE "Src/Operation/*.cpp" "Src/Operation/*.h") 

FILE(GLOB Windows_FILE "Src/Windows/*.cpp" "Src/Windows/*.h") 

FILE(GLOB QGLViewer_FILE "Src/Windows/QGLViewer/*.cpp" "Src/Windows/QGLViewer/*.h") 

FILE(GLOB Record_FILE "Src/Record/*.h" "Src/Record/*.cpp")

FILE(GLOB Utils_FILE "Src/Utils/*.h" "Src/Utils/*.cpp")

FILE(GLOB WIN32_RESOURCE_FILE "Resources/Version.rc")

add_compile_definitions(NOMINMAX)
add_compile_definitions(_USE_MATH_DEFINES)


add_executable(${PROJECT_NAME}  
    ${QRC_FILES}
    ${Command_FILE}
    ${Core_FILE}
    ${Object_FILE}
    ${ObjectDraw_FILE}
    ${Operate_FILE}
    ${Windows_FILE}
    ${QGLViewer_FILE}
    ${Record_FILE}
    ${Utils_FILE}
    "Src/Main.cpp")

source_group(Command FILES ${Command_FILE})
source_group(Core FILES ${Core_FILE})
source_group(Object FILES ${Object_FILE})
source_group(ObjectDraw FILES ${ObjectDraw_FILE})
source_group(Operate FILES ${Operate_FILE})
source_group(Widgets FILES ${Windows_FILE})
source_group(QGLViewer FILES ${QGLViewer_FILE})
source_group(Record FILES ${Record_FILE})
source_group(Utils FILES ${Utils_FILE})

target_sources(${PROJECT_NAME} PRIVATE ${WIN32_RESOURCE_FILE})


target_link_libraries(${PROJECT_NAME} 
    Qt5::Widgets 
    Qt5::Core 
    Qt5::Gui 
    Qt5::OpenGL
    opengl32
    glu32
    AMCAXCommon
    AMCAXPart
    AMCAXStep
    AMCAXSubD
    AMCAXTMSpline
    FreeGLUT::freeglut
)

message("CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")

execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${AMCAX_DLL_PATH}/ ${FREEGLUT_DLL_PATH}/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)