QT       += testlib

QT       -= gui

TARGET = tst_testsync
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../../src/sync/sync.pri)

SOURCES += tst_testsync.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(../../base.pri)
