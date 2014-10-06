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

#include "remotereceiver.h"
#include "ipc/ipcserver.h"
#include "ipc/ipcclient.h"
#include "livenodeengine.h"

#include <QTcpSocket>

#ifdef QMLLIVE_DEBUG
#define DEBUG qDebug()
#else
#define DEBUG if (0) qDebug()
#endif

RemoteReceiver::RemoteReceiver(QObject *parent)
    : QObject(parent)
    , m_server(new IpcServer(this))
    , m_node(0)
    , m_connectionAcknowledged(false)
    , m_socket(0)
    , m_client(0)
{
    connect(m_server, SIGNAL(received(QString,QByteArray)), this, SLOT(handleCall(QString,QByteArray)));
    connect(m_server, SIGNAL(clientConnected(QTcpSocket*)), this, SLOT(onClientConnected(QTcpSocket*)));
    connect(m_server, SIGNAL(clientConnected(QHostAddress)), this, SIGNAL(clientConnected(QHostAddress)));
    connect(m_server, SIGNAL(clientDisconnected(QHostAddress)), this, SIGNAL(clientDisconnected(QHostAddress)));
}

void RemoteReceiver::listen(int port)
{
    m_server->listen(port);
}

/*!
 * Sets the current workspace to \a path. Documents location will be adjusted based on
 * this workspace path.
 */
void RemoteReceiver::setWorkspace(const QString &path)
{
    m_workspace = QDir(path);
}

QString RemoteReceiver::workspace() const
{
    return m_workspace.absolutePath();
}

void RemoteReceiver::setPin(const QString &pin)
{
    m_pin = pin;
}

QString RemoteReceiver::pin() const
{
    return m_pin;
}

void RemoteReceiver::setMaxConnections(int max)
{
    m_server->setMaxConnections(max);
}

/*!
 * Sets whether the workspace is writeable to \a on. Otherwise incoming write document requests will be ignored.
 */
void RemoteReceiver::setWorkspaceWriteable(bool on)
{
    m_workspaceWriteable = on;
}


void RemoteReceiver::handleCall(const QString &method, const QByteArray &content)
{
    DEBUG << "RemoteReceiver::handleIpcCall: " << method;

    if(method == "checkPin(QString)") {
        QString pin;
        QDataStream in(content);
        in >> pin;
        if (m_pin == pin && m_client) {
            m_connectionAcknowledged = true;
            emit pinOk(true);
            m_client->send("pinOK(bool)", QByteArray::number(1));
        } else if (m_client) {
            emit pinOk(false);
            m_client->send("pinOK(bool)", QByteArray::number(0));
        }
    }

    if (!m_connectionAcknowledged) {
        qWarning() << "Connecting without Pin Authentication is not allowed";
        return;
    }

    if(method == "setXOffset(int)") {
        int offset;
        QDataStream in(content);
        in >> offset;
        emit xOffsetChanged(offset);
    } else if(method == "setYOffset(int)") {
        int offset;
        QDataStream in(content);
        in >> offset;
        emit yOffsetChanged(offset);
    } else if(method == "setRotation(int)") {
        int rotation;
        QDataStream in(content);
        in >> rotation;
        emit rotationChanged(rotation);
    } else if(method == "sendDocument(QString,QByteArray)") {
        QString document;
        QByteArray data;
        QDataStream in(content);
        in >> document;
        in >> data;
        writeDocument(document, data);
    } else if(method == "activateDocument(QString)") {
        QString document;
        QDataStream in(content);
        in >> document;
        qDebug() << "\tactivate document: " << document;
        emit activateDocument(document);
    } else if(method == "ping()") {
        if (m_client)
            m_client->send("pong()", QByteArray());
    }
}


void RemoteReceiver::registerNode(LiveNodeEngine *node)
{
    if(m_node) { disconnect(m_node); }
    m_node = node;
    connect(this, SIGNAL(activateDocument(QString)), m_node, SLOT(setActiveDocument(QString)));
}

void RemoteReceiver::writeDocument(const QString &document, const QByteArray &content)
{
    if(!m_workspaceWriteable) { return; }
    QString filePath = m_workspace.absoluteFilePath(document);
    QString dirPath = QFileInfo(filePath).absoluteDir().absolutePath();
    m_workspace.mkpath(dirPath);
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Unable to save file: " << file.errorString();
        return;
    }
    file.write(content);
    file.close();

    if (m_node)
        m_node->delayReload();
}

void RemoteReceiver::onClientConnected(QTcpSocket *socket)
{
    if (m_client)
        delete m_client;

    m_client = new IpcClient(socket);

    m_socket = socket;

    if (!m_pin.isEmpty()) {
        m_client->send("needsPinAuthentication()", QByteArray());
        m_connectionAcknowledged = false;
    } else {
        m_connectionAcknowledged = true;
    }
}

void RemoteReceiver::appendToLog(const QList<QQmlError> &errors)
{
    foreach (const QQmlError &err, errors) {
        if (!err.isValid())
            continue;

        QtMsgType type = QtDebugMsg;

        if (err.description().contains(QString::fromLatin1("error"), Qt::CaseInsensitive) ||
            err.description().contains(QString::fromLatin1("is not installed"), Qt::CaseInsensitive) ||
            err.description().contains(QString::fromLatin1("is not a type"), Qt::CaseInsensitive))
            type = QtCriticalMsg;
        else if (err.description().contains(QString::fromLatin1("warning"), Qt::CaseInsensitive))
            type = QtWarningMsg;

        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out << type;
        out << err.description();
        out << err.url();
        out << err.line();
        out << err.column();

        m_client->send("qmlLog(QtMsgType, QString, QUrl, int, int)", bytes);
    }
}
