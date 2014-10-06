TARGET = qmlliveruntime
DESTDIR = ../../bin

QT *= widgets quick
mac: CONFIG -= app_bundle

SOURCES += main.cpp

win32: RC_FILE = ../../icons/appicon.rc

include(../widgets/widgets.pri)
include(../src.pri)

RESOURCES += \
    qml.qrc




