QT          += testlib

QT          -= gui
QT          += network

TARGET =    tst_testipc
CONFIG      += console
CONFIG      -= app_bundle

TEMPLATE =  app

include(../../src/src.pri)

SOURCES += tst_testipc.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../base.pri)

