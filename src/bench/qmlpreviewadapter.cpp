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

#include "qmlpreviewadapter.h"
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QLocalSocket>
#include <QDebug>

Q_GLOBAL_STATIC(QProcess, proc)
Q_GLOBAL_STATIC(QString, serverName)


QmlPreviewAdapter::QmlPreviewAdapter(QObject *parent) :
    QObject(parent)
{
}

bool QmlPreviewAdapter::canPreview(const QString &path) const
{
    return path.endsWith(".qml");
}

QImage QmlPreviewAdapter::preview(const QString &path, const QSize &requestedSize)
{
    QImage img;

    if (!requestedSize.isValid()) {
        qWarning() << "preview for" << path << "with invalid size" << requestedSize << "requested";
        return QImage();
    }

#ifdef Q_OS_WINDOWS
    static const QString suffix = ".exe";
#else
    static const QString suffix;
#endif

    static const QString program = QCoreApplication::applicationDirPath() + QDir::separator() + "previewGenerator" + suffix;
    static const QStringList arguments("QmlLiveBench");

    if (proc()->state() != QProcess::Running) {
        QStringList env = QProcess::systemEnvironment();
        {
            QMutexLocker m(&m_mutex);
            env.append(QString("QT_PLUGIN_PATH=%1").arg(m_pluginPaths.join(":")));
            env.append(QString("QML_IMPORT_PATH=%1").arg(m_importPaths.join(":")));
            env.append(QString("QML2_IMPORT_PATH=%1").arg(m_importPaths.join(":")));
        }
        proc()->setEnvironment(env);
        proc()->start(program, arguments);
        if (!proc()->waitForStarted()) {
            qWarning() << "Failed to start" << program;
            return QImage();
        }

        proc()->waitForReadyRead();
        QByteArray data = proc()->readAll();
        if (!data.startsWith("ready#")) {
            qWarning() << "previewGenerator did not send the \"ready\" token:" << proc()->readAllStandardError();
            return QImage();
        }
        QString server = QString::fromUtf8(QByteArray::fromHex(data.mid(6)));
        *serverName() = server;
    }

    QLocalSocket socket;

    socket.connectToServer(*serverName());
    if (!socket.waitForConnected(3000)) {
        qWarning() << "Could not connect to PreviewGenerator";
        return QImage();
    }

    //qWarning() << "QL: sending request [" << requestedSize << "]" << path;

    {
        QDataStream ds(&socket);
        ds << requestedSize << path;
    }
    socket.waitForBytesWritten();

    //qWarning() << "QL: sent";

    QByteArray data;
    bool allData = false;
    while (!allData) {
        int bytes = socket.bytesAvailable();
        //qWarning() << "QL: can read" << bytes << "bytes";

        if (proc()->state() != QProcess::Running) {
            qWarning() << "previewGenerator stopped unexpectedly";
            break;
        }

        if (!bytes) {
            //qWarning() << "QL: waiting for more data (have" << data.size() << "already)";
            socket.waitForReadyRead(5000);
            continue;
        }

        data += socket.read(bytes);

        if (data.endsWith("\nEND")) {
            allData = true;
            data.chop(4);
            //qWarning() << "QL: received end marker after"  << data.size() << "bytes";
        }
    }
    socket.close();

    img.loadFromData(data, "PNG");

    if (img.size().isNull()) {
        qWarning() << "Failed to generate Preview:" << proc()->readAllStandardError();

        img = QImage("://livert/no.png");
    }

    return img;
}

bool QmlPreviewAdapter::canAdapt(const QUrl &url) const
{
    Q_UNUSED(url);

    return false;
}

QUrl QmlPreviewAdapter::adapt(const QUrl &url, QQmlContext *context)
{
    Q_UNUSED(url);
    Q_UNUSED(context);

    return QUrl();
}

void QmlPreviewAdapter::setImportPaths(QStringList importPaths)
{
    QMutexLocker m(&m_mutex);
    m_importPaths = importPaths;
}

void QmlPreviewAdapter::setPluginPaths(QStringList pluginPaths)
{
    QMutexLocker m(&m_mutex);
    m_importPaths = pluginPaths;
}
