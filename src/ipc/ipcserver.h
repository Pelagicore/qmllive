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
#include <QtNetwork>

class IpcServer : public QObject
{
    Q_OBJECT
public:
    explicit IpcServer(QObject *parent = 0);
    void listen(int port);
    void setMaxConnections(int num);
private Q_SLOTS:
    void newConnection();
Q_SIGNALS:
    void received(const QString& method, const QByteArray& content);
    void clientConnected(const QHostAddress& address);
    void clientConnected(QTcpSocket* socket);
    void clientDisconnected(QTcpSocket* socket);
    void clientDisconnected(const QHostAddress& address);

private Q_SLOTS:
    void onConnectionClosed();

private:
    QTcpServer *m_server;
};

