cmake_minimum_required(VERSION 3.29)

project(QmlAst
        VERSION 2024.06.30
        DESCRIPTION "QmlAst"
        LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Qml Test)

qt_standard_project_setup()

add_subdirectory(src)
add_subdirectory(tst)

# Test
