cmake_minimum_required(VERSION 2.8.12)

project(QmlAst)

find_package(Qt5 COMPONENTS Core Qml Test)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (CMAKE_AUTOUIC ON)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DQT_VERSION_MAJOR=${Qt5_VERSION_MAJOR})
add_definitions(-DQT_VERSION_MINOR=${Qt5_VERSION_MINOR})
add_definitions(-DQT_VERSION_PATCH=${Qt5_VERSION_PATCH})

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Werror -pedantic")

add_subdirectory(src)
add_subdirectory(tst)

# Test
include(CTest)
