cmake_minimum_required(VERSION 3.5)
project(AmcaxDaemon)
set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    add_compile_options(/utf-8)
    add_compile_options(/bigobj) 
endif()

#Turn on automoc/autouic, must have
set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTOUIC ON)

#set(CMAKE_PREFIX_PATH "D:/QtFile/5.15.2/msvc2019_64")
 
#find depended qt libraries 
find_package(Qt5Widgets REQUIRED)  
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)

SET(AMCAX_PATH "AMCAX/include")
SET(AMCAX_LIB_debug "AMCAX/lib/Debug")
SET(AMCAX_LIB_release "AMCAX/lib/Release")

include_directories(${AMCAX_PATH})

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

FILE(GLOB WIN32_RESOURCE_FILE "Resources/resource.h" "Resources/resource.rc")

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

target_sources(${PROJECT_NAME} PRIVATE "Resources/resource.rc")


target_link_libraries(${PROJECT_NAME} 
    Qt5::Widgets 
    Qt5::Core 
    Qt5::Gui 
    Qt5::OpenGL
    opengl32
    glu32
)

FIND_LIBRARY(AMCAXCommon_release AMCAXCommon ${AMCAX_LIB_release})
FIND_LIBRARY(AMCAXCommon_debug AMCAXCommon ${AMCAX_LIB_debug})
FIND_LIBRARY(AMCAXPart_release AMCAXPart ${AMCAX_LIB_release})
FIND_LIBRARY(AMCAXPart_debug AMCAXPart ${AMCAX_LIB_debug})

target_link_libraries(${PROJECT_NAME} 
        debug ${AMCAXCommon_debug} ${AMCAXPart_debug} 
    optimized ${AMCAXCommon_release} ${AMCAXPart_release} 
)
