set(applicationname QmlAst)
file(GLOB_RECURSE sources *.cpp *.h)

#

add_executable(
    ${applicationname}
    ${sources})

qt5_use_modules(
    ${applicationname}
    Core Qml)

install(
    TARGETS ${applicationname}
    RUNTIME DESTINATION bin)

#

add_library(
    ${applicationname}Lib
    ${sources})

target_include_directories(
    ${applicationname}Lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

qt5_use_modules(
    ${applicationname}Lib
    Core Qml)
