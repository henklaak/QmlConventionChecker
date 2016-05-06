QT += core qml qml-private
QT -= gui

CONFIG += c++11

TARGET = QmlAst
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    checkingvisitor.cpp

HEADERS += \
    checkingvisitor.h

OTHER_FILES = *.qml
