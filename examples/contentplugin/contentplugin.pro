TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../../src
HEADERS       = mycontentadapterplugin.h
SOURCES       = mycontentadapterplugin.cpp
TARGET        = myContentAdapterPlugin
DESTDIR       = ../../bin/plugins

RESOURCES += res.qrc \
    res.qrc

OTHER_FILES += \
    plugin.qml
