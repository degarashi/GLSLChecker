#-------------------------------------------------
#
# Project created by QtCreator 2014-04-30T13:44:48
#
#-------------------------------------------------

QT       += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GLSLChecker
TEMPLATE = app

INCLUDEPATH += libtinyhl/
QMAKE_LIBDIR += $$PWD/build_lib
LIBS += -ltinyhl
SOURCES += main.cpp \
	    mainwindow.cpp
HEADERS  += mainwindow.h
FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11
CONFIG(debug, debug|release) {
    DEFINES += DEBUG _DEBUG
}
CONFIG(release, debug|release) {
    DEFINES += NDEBUG
}
