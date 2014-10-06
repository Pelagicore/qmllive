/*
  Copyright (c) 2014, Pelagicore AG.
  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui>
#include <QCryptographicHash>
#include <QtQuick>
#include <QtWidgets>

#include "livenodeengine.h"
#include "remotereceiver.h"
#include "logger.h"
#include "qmlhelper.h"


struct Options
{
    Options()
        : ipcPort(10234)
        , allowUpdates(true)
        , fullscreen(false)
        , transparent(false)
        , frameless(false)
        , stayontop(false)
    {}
    int ipcPort;
    bool allowUpdates;
    QString activeDocument;
    QString workspace;
    QString pluginPath;
    QStringList importPaths;
    bool fullscreen;
    bool transparent;
    bool frameless;
    bool stayontop;
};

static Options options;

static void usage()
{
    qWarning("Usage qmlliveruntime [options] <workspace>");
    qWarning(" ");
    qWarning(" options:");
    qWarning("  -ipcport <port> ....................the port the ipc shall listen on");
    qWarning("  -no-file-updates ...................do not write to files on changes");
    qWarning("  -pluginpath ........................path to qmllive plugins");
    qWarning("  -importpath ........................path to the qml import path");
    qWarning("  -fullscreen ........................shows in fullscreen mode");
    qWarning("  -transparent .......................Make the window transparent");
    qWarning("  -frameless .........................run with no window frame");
    qWarning("  -stayontop .........................keep viewer window on top");
    qWarning(" ");
    exit(1);
}

static void parseArguments(const QStringList& arguments)
{
    for (int i = 1; i < arguments.count(); ++i) {
        bool lastArg = (i == arguments.count() - 1);
        QString arg = arguments.at(i);
        if(arg == QLatin1String("-ipcport")) {
            if (lastArg || arguments.at(++i).startsWith(QLatin1Char('-'))) usage();
            options.ipcPort = arguments.at(i).toInt();
        } else if(arg == QLatin1String("-pluginpath")) {
            if (lastArg || arguments.at(++i).startsWith(QLatin1Char('-'))) usage();
            options.pluginPath = arguments.at(i);
        } else if(arg == QLatin1String("-importpath")) {
            if (lastArg || arguments.at(++i).startsWith(QLatin1Char('-'))) usage();
            options.importPaths.append(QDir(arguments.at(i)).absolutePath());
            continue;
        } else if(arg == QLatin1String("-no-file-updates")) {
            options.allowUpdates = false;
        } else if(arg == QLatin1String("-fullscreen")) {
            options.fullscreen = true;
        } else if(arg == QLatin1String("-transparent")) {
            options.transparent = true;
        } else if(arg == QLatin1String("-frameless")) {
            options.frameless = true;
        } else if(arg == QLatin1String("-stayontop")) {
            options.stayontop = true;
        } else if (!arg.startsWith(QLatin1Char('-'))) {
            options.workspace = arg;
        } else if (true || arg == QLatin1String("-help")) {
            usage();
        }
    }
}

class RuntimeLiveNodeEngine : public LiveNodeEngine {

    virtual QQuickView* initView()
    {
        QQuickView* view = new QQuickView();
        if (options.transparent) {
            QSurfaceFormat surfaceFormat;
            surfaceFormat.setAlphaBufferSize(8);
            view->setFormat(surfaceFormat);
            view->setClearBeforeRendering(true);
            view->setColor(QColor(Qt::transparent));
        }

        if (options.stayontop) {
            view->setFlags(view->flags() | Qt::X11BypassWindowManagerHint);
            view->setFlags(view->flags() | Qt::WindowStaysOnTopHint);
        }

        if (options.frameless)
            view->setFlags(view->flags() | Qt::FramelessWindowHint);

        if (options.fullscreen)
            view->setWindowState(Qt::WindowFullScreen);

        return view;
    }
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("QmlLiveRuntime");
    app.setOrganizationDomain("pelagicore.com");
    app.setOrganizationName("Pelagicore");
    parseArguments(app.arguments());

    QQuickView view;

    QStringList defaultImports = view.engine()->importPathList();

    RuntimeLiveNodeEngine engine;
    engine.setUpdateMode(LiveNodeEngine::RecreateView);
    engine.setWorkspace(options.workspace);
    engine.setPluginPath(options.pluginPath);
    engine.setImportPaths(options.importPaths + defaultImports);
    engine.loadDocument(QUrl("qrc:/qml/qmlsplash/splash-qt5.qml"));
    RemoteReceiver receiver;
    receiver.listen(options.ipcPort);
    receiver.setWorkspace(options.workspace);
    receiver.setWorkspaceWriteable(options.allowUpdates);
    receiver.registerNode(&engine);

    engine.connect(&engine, SIGNAL(logErrors(QList<QQmlError>)), &receiver, SLOT(appendToLog(QList<QQmlError>)));

    receiver.connect(&receiver, SIGNAL(xOffsetChanged(int)), &engine, SLOT(setXOffset(int)));
    receiver.connect(&receiver, SIGNAL(yOffsetChanged(int)), &engine, SLOT(setYOffset(int)));
    receiver.connect(&receiver, SIGNAL(rotationChanged(int)), &engine, SLOT(setRotation(int)));

    QMap<QString, QString> extraSyslog;
    extraSyslog.insert("LOCATION", QString("qmllive://${HOST}:%1").arg(options.ipcPort));

    int ret = app.exec();

    return ret;
}
