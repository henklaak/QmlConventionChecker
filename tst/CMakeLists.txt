include(CTest)

qt_add_executable(QmlAstUnitTest)

target_sources(QmlAstUnitTest
    PRIVATE
    QmlAstUnitTest.cpp
)

target_link_libraries(QmlAstUnitTest
    PRIVATE
    QmlAstLib
    Qt6::Core
    Qt6::Qml
    Qt6::Test
)

add_test(
    NAME QmlAstUnitTest
    COMMAND QmlAstUnitTest
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

configure_file(valid.qml ${CMAKE_CURRENT_BINARY_DIR}/valid.qml COPYONLY)
configure_file(invalid1.qml ${CMAKE_CURRENT_BINARY_DIR}/invalid1.qml COPYONLY)
configure_file(invalid2.qml ${CMAKE_CURRENT_BINARY_DIR}/invalid2.qml COPYONLY)
configure_file(invalid3.qml ${CMAKE_CURRENT_BINARY_DIR}/invalid3.qml COPYONLY)
configure_file(invalid4.qml ${CMAKE_CURRENT_BINARY_DIR}/invalid4.qml COPYONLY)
configure_file(invalid5.qml ${CMAKE_CURRENT_BINARY_DIR}/invalid5.qml COPYONLY)
configure_file(invalid6.qml ${CMAKE_CURRENT_BINARY_DIR}/invalid6.qml COPYONLY)
configure_file(invalid7.qml ${CMAKE_CURRENT_BINARY_DIR}/invalid7.qml COPYONLY)
