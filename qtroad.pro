#-------------------------------------------------
#
# Project created by QtCreator 2015-05-02T13:39:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtroad
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Parser.cpp\
ChartsBase.cpp

HEADERS  += MainWindow.hpp \
    StructureChart.hpp \
    ClassChart.hpp \
    ChartsBase.hpp \
    Parser.hpp

FORMS    += MainWindow.ui

CONFIG += c++11
