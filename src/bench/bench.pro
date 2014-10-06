TEMPLATE = app
TARGET = qmllivebench
DESTDIR = ../../bin

QT *= gui core quick widgets core-private

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    optionsdialog.cpp \
    benchlivenodeengine.cpp \
    previewimageprovider.cpp \
    directorypreviewadapter.cpp \
    qmlpreviewadapter.cpp \
    benchquickview.cpp \
    host.cpp \
    hostmodel.cpp \
    hostwidget.cpp \
    dummydelegate.cpp \
    allhostswidget.cpp \
    hostmanager.cpp \
    hostsoptionpage.cpp \
    httpproxyoptionpage.cpp \
    importpathoptionpage.cpp \
    hostdiscoverymanager.cpp \
    autodiscoveryhostsdialog.cpp

HEADERS += \
    mainwindow.h \
    optionsdialog.h \
    benchlivenodeengine.h \
    previewimageprovider.h \
    directorypreviewadapter.h \
    qmlpreviewadapter.h \
    benchquickview.h \
    host.h \
    hostmodel.h \
    hostwidget.h \
    dummydelegate.h \
    allhostswidget.h \
    hostmanager.h \
    hostsoptionpage.h \
    importpathoptionpage.h \
    httpproxyoptionpage.h \
    hostdiscoverymanager.h \
    autodiscoveryhostsdialog.h


FORMS += \
    optionsdialog.ui \
    hostsoptionpage.ui \
    httpproxyoptionpage.ui \
    importpathoptionpage.ui \
    autodiscoveryhostsdialog.ui

# include(../base.pri)
include(../widgets/widgets.pri)
include(../src.pri)

win32: RC_FILE = ../../icons/appicon.rc
mac: ICON = ../../icons/appicon.icns
