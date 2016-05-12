cmake_minimum_required(VERSION 2.8.12)

project(QmlAst)

find_package(Qt5 COMPONENTS Core Qml Test)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (CMAKE_AUTOUIC ON)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)
add_subdirectory(tst)

# Test
include(CTest)
