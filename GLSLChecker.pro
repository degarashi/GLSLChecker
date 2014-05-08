#-------------------------------------------------
#
# Project created by QtCreator 2014-04-30T13:44:48
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GLSLChecker
TEMPLATE = app

SOURCES += main.cpp\
	    mainwindow.cpp \
	    glctxnotify.cpp \
    syntaxhighlighter.cpp \
    glsl.cpp
HEADERS  += mainwindow.h \
	    glctxnotify.h \
    syntaxhighlighter.h \
    glsl.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11 -DDEBUG -D_DEBUG
