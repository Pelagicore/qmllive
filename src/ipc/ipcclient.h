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

#include <QTcpSocket>
#include <QUuid>
#include <QQueue>
#include "ipcconnection.h"

class Package;
class IpcClient : public QObject
{
    Q_OBJECT
public:
    explicit IpcClient(QObject *parent = 0);
    IpcClient(QTcpSocket* socket, QObject *parent = 0);

    QAbstractSocket::SocketState state() const;

    void connectToServer(const QString& hostName, int port);
    QUuid send(const QString& method, const QByteArray& data);

    bool waitForConnected(int msecs = 30000);
    bool waitForDisconnected(int msecs = 30000);
    bool waitForSent(const QUuid uuid, int msecs = 30000);

    QString errorToString(QAbstractSocket::SocketError error);

Q_SIGNALS:
    void connected();
    void disconnected();
    void connectionError(QAbstractSocket::SocketError socketError);

    void sentSuccessfully(const QUuid& uuid);
    void sendingError(const QUuid& uuid, QAbstractSocket::SocketError socketError);

    void received(const QString& method, const QByteArray& content);

public Q_SLOTS:
    void disconnectFromServer();

private Q_SLOTS:
    void processQueue();
    void onBytesWritten(qint64 written);
    void onError(QAbstractSocket::SocketError socketError);

private:
    qint64 sendPackage(const QString& method, const QByteArray& data);

    QTcpSocket *m_socket;
    QQueue<Package*> m_queue;
    Package* m_current;
    qint64 m_written;
    QUuid m_lastSuccess;

    IpcConnection* m_connection;
};

