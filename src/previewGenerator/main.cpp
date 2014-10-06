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

#include <QApplication>
#include <QQuickView>
#include <QQmlEngine>
#include <QElapsedTimer>
#include <QDir>
#include <QBuffer>
#include <QLocalServer>
#include <QLocalSocket>
#include <QDebug>

#include <stdio.h>

void handlePreview(QLocalSocket *socket);

class PreviewServer : public QObject
{
    Q_OBJECT
public:
    PreviewServer()
        : m_server(new QLocalServer())
    {
        connect(m_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    }

    bool listen()
    {
        return m_server->listen("QmlLivePreviewGenerator");
    }

    QString serverName() const
    {
        return m_server->serverName();
    }

protected slots:
    void onNewConnection()
    {
        while (m_server->hasPendingConnections()) {
            QLocalSocket *socket = m_server->nextPendingConnection();

            handlePreview(socket);

            delete socket;
        }
    }
private:
    QLocalServer *m_server;
};


FILE *dbgf = 0;

int main (int argc, char** argv)
{
    QApplication app(argc, argv);

    if (app.arguments().count() != 2 ||
       (app.arguments().at(1) != "QmlLiveBench") )
        qFatal("This application is managed by QmlLive and is not supposed to be manually started");

    PreviewServer preview;
    if (!preview.listen()) {
        qFatal("Failed to listen to local socket \"QmlLivePreviewGenerator\"");
    }

    printf("ready#%s\n", preview.serverName().toUtf8().toHex().constData());
    fflush(stdout);

    //dbgf = fopen("C:\\Qt\\pg.log", "w");
    return app.exec();
}

void handlePreview(QLocalSocket *socket)
{
    if (dbgf) { fprintf(dbgf, "PG: waiting on input\n"); fflush(dbgf); }

    socket->waitForReadyRead();

    QDataStream ds (socket);
    QSize expectedSize;
    QString path;

    ds >> expectedSize >> path;

    if (dbgf) { fprintf(dbgf, "PG: received [%dx%d] %s\n", expectedSize.width(), expectedSize.height(), qPrintable(path)); fflush(dbgf); }

    //QML Import paths and plugin paths will be set by environment variables
    QQuickView *view = new QQuickView();
    view->setFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint |
                   Qt::CustomizeWindowHint | Qt::WindowDoesNotAcceptFocus); // | Qt::WindowStaysOnBottomHint);
    view->setPosition(-9999999, -9999999);
    view->setSource(QUrl::fromLocalFile(path));

    if (view->errors().isEmpty()) {
        view->show();

        //Wait until the Window is exposed;
        int timeout = 3000;
        QElapsedTimer timer;
        timer.start();
        while (!view->isExposed()) {
            int remaining = timeout - int(timer.elapsed());
            if (remaining <= 0)
                break;
            QCoreApplication::processEvents(QEventLoop::AllEvents, remaining);
            //Garbage collection (Delivers all DeferredDeleteEvents which are in the pipe)
            QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
        }

        if (view->width() <= 0 || view->height() <= 0) {
            view->setWidth(500);
            view->setHeight(500);
        }

        if (view->rootObject()) {
            QImage img = view->grabWindow();
            if (!img.isNull()) {
                img = img.scaled(expectedSize, Qt::KeepAspectRatio);
                img.save(socket, "PNG");

                if (dbgf) { fprintf(dbgf, "PG: sending image\n"); fflush(dbgf); }
            }
        }
    }
    delete view;

    socket->write("\nEND");
    socket->flush();

    if (dbgf) { fprintf(dbgf, "PG: sent END and flushed\n"); fflush(dbgf); }
}


#include "main.moc"
