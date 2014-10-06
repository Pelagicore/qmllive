TEMPLATE = app
TARGET = previewGenerator
DESTDIR = ../../bin

mac: {
    CONFIG -= app_bundle
    DESTDIR = ../../bin/qmllivebench.app/Contents/MacOS/
}

QT = gui core quick widgets

SOURCES += \
    main.cpp
