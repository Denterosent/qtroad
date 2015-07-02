#-------------------------------------------------
#
# Project created by QtCreator 2015-05-02T13:39:57
# qtroad = Rapid-Objekt-Analysing-Diagramm, created with QT
#-------------------------------------------------

QT       += core gui
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtroad
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Parser.cpp\
ChartsBase.cpp \
    StructureChartDrawer.cpp

HEADERS  += MainWindow.hpp \
    StructureChart.hpp \
    ClassChart.hpp \
    ChartsBase.hpp \
    Parser.hpp \
    StructureChartDrawer.hpp

FORMS    += MainWindow.ui

CONFIG += c++11
