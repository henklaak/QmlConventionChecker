qt_add_executable(QmlAst)

target_sources(QmlAst
    PRIVATE
    main.cpp
    checker.cpp checker.h
    checkingvisitor.cpp checkingvisitor.h
    qmlprivates.h)

target_include_directories(QmlAst
    PRIVATE ${Qt6Qml_PRIVATE_INCLUDE_DIRS}
)
target_link_libraries(QmlAst
    PRIVATE
    Qt6::Core
    Qt6::Qml)

install(
    TARGETS QmlAst
    RUNTIME DESTINATION bin)

# Library used to unittest code

add_library(
    QmlAstLib
    main.cpp
    checker.cpp checker.h
    checkingvisitor.cpp checkingvisitor.h
    qmlprivates.h)

target_include_directories(
    QmlAstLib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(QmlAstLib
    PRIVATE ${Qt6Qml_PRIVATE_INCLUDE_DIRS}
)


target_link_libraries(QmlAstLib
    PRIVATE
    Qt6::Core
    Qt6::Qml)

