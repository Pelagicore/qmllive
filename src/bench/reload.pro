TEMPLATE = app
TARGET = livereload

equals(QT_MAJOR_VERSION, "5"): CONFIG += qt5

QT = gui core
qt5:QT += quick widgets core-private
else:QT += declarative

mac: CONFIG -= app_bundle

qt5:DEFINES += USING_QT5
else:DEFINES += USING_QT4

SOURCES += \
    main.cpp \
    watcher.cpp \
    mainwindow.cpp

qt5:SOURCES += windowwidget.cpp

HEADERS += \
    watcher.h \
    mainwindow.h

qt5:HEADERS += windowwidget.h

RESOURCES += \
    reload.qrc

OTHER_FILES += \
    error.qml \
    error2.qml
