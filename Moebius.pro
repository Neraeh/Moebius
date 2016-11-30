QT += core
QT -= gui

CONFIG += c++11

TARGET = Moebius
DESTDIR = ../build
CONFIG += console communi
CONFIG -= app_bundle
COMMUNI += core
INCLUDEPATH += Communi/IrcCore/

QMAKE_CFLAGS_RELEASE += -march=native -O3 -pipe -fomit-frame-pointer
QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE
QMAKE_LFLAGS_RELEASE += -s

TEMPLATE = app

SOURCES += main.cpp \
    moebius.cpp \
    logger.cpp \
    commands.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    moebius.h \
    logger.h \
    commands.h
