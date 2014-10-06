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

#pragma once

#include <QtCore>
#include <QHostAddress>

#include <QQmlError>

class QTcpSocket;
class LiveNodeEngine;
class IpcServer;
class IpcClient;

class RemoteReceiver : public QObject
{
    Q_OBJECT
public:
    explicit RemoteReceiver(QObject *parent = 0);
    void listen(int port);
    void registerNode(LiveNodeEngine *node);
    void setWorkspaceWriteable(bool on);
    void setWorkspace(const QString &path);
    QString workspace() const;
    void setPin(const QString& pin);
    QString pin() const;

    void setMaxConnections(int max);
Q_SIGNALS:
    void activateDocument(const QString& document);
    void reload();
    void clientConnected(const QHostAddress& address);
    void clientDisconnected(const QHostAddress& address);
    void pinOk(bool ok);
    void xOffsetChanged(int offset);
    void yOffsetChanged(int offset);
    void rotationChanged(int rotation);

private Q_SLOTS:
    void handleCall(const QString& method, const QByteArray& content);
    void writeDocument(const QString& document, const QByteArray& content);

    void appendToLog(const QList<QQmlError> &errors);

    void onClientConnected(QTcpSocket *socket);
private:
    IpcServer *m_server;
    LiveNodeEngine *m_node;

    QDir m_workspace;
    bool m_workspaceWriteable;

    QString m_pin;
    bool m_connectionAcknowledged;

    QTcpSocket* m_socket;
    IpcClient* m_client;
};

