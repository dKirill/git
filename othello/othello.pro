#-------------------------------------------------
#
# Project created by QtCreator 2014-03-22T13:38:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = othello
TEMPLATE = app


SOURCES += main.cpp\
    gui.cpp \
    game.cpp

HEADERS  += \
    game.h \
    gui.h

QMAKE_CXXFLAGS += -std=c++11

OTHER_FILES += \
    problems.txt
