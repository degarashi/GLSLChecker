#-------------------------------------------------
#
# Project created by QtCreator 2014-05-16T16:25:29
#
#-------------------------------------------------

QT       += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tinyhl
TEMPLATE = lib
CONFIG += staticlib

SOURCES += glctxnotify.cpp \
	    glsl.cpp \
	    syntaxhighlighter.cpp
HEADERS += glctxnotify.h \
	    glsl.h \
	    syntaxhighlighter.h
QMAKE_CXXFLAGS += -std=c++11

unix {
    target.path = $$PWD/../build_lib
    INSTALLS += target
}
