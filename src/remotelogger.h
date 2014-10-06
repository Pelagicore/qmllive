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

#include "logger.h"
#include <QHostAddress>
#include <QtQuick>

class QUdpSocket;
class RemoteLogger : public Logger
{
    Q_OBJECT
public:
    explicit RemoteLogger(QObject *parent = 0);

public Q_SLOTS:
    void setHostAddress(const QHostAddress &address);
    void setPort(int port);
    void appendToLog(const QList<QQmlError> &errors);

private Q_SLOTS:
    void broadcast(int type, const QString &msg, const QUrl &url = QUrl(), int line = -1, int column = -1);

private:
    QUdpSocket* m_socket;
    int m_port;
    QHostAddress m_host;
};
