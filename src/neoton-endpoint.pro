QT += core websockets
QT -= gui

# CONFIG += c++11

TARGET = neoton-endpoint
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ntendpoint.cpp \
    ntlog.cpp \
    ntbassplayer.cpp \
    ntclient.cpp \
    ntauth.cpp

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
    ntendpoint.h \
    ntlog.h \
    ntbassplayer.h \
    ntclient.h \
    chiptune.h \
    ntauth.h

# Neoton Endpoint uses BASS Library by Un4seen Developments.
# Ah yes, it's not a pure Open Source way... We're sorry.
# BASS provides an easy way to play HTTP streaming audio
# like no other do. So we're sorry.
# If you still want to build Neoton Endpoint, just make the
# `shared` directory near the `neoton-endpoint` dir and
# place the `bass.lib` and `bass.h` files obtained from Un4seen.
LIBS += -L../../shared -lbass
INCLUDEPATH += $$quote(../../shared)
