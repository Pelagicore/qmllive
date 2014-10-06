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

#include <QObject>
#include <QUrl>
#include <QHostAddress>

class QUdpSocket;
class LogReceiver : public QObject
{
    Q_OBJECT
public:
    explicit LogReceiver(QObject *parent = 0);

    int port() const;
    QString address() const;

public Q_SLOTS:
    void setPort(int port);
    void setAddress(const QString& address);
    void connectToServer();

Q_SIGNALS:
    void message(int type, const QString &msg, const QUrl &url, int line, int column);

private Q_SLOTS:
    void processPendingDatagrams();

private:
    QUdpSocket* m_socket;
    QHostAddress m_address;
    int m_port;
};
