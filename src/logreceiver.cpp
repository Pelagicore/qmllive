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

#include <QUdpSocket>
#include <QStringList>
#include <QUrl>
#include "logreceiver.h"

/*!
 * \class LogReceiver
 * \brief Connects to a port and waits for log messages sent via udp
 * \group qmllive
 *
 * \sa Logger, RemoteLogger
 */

/*!
 * Standard constructor using \a parent as parent
 */
LogReceiver::LogReceiver(QObject *parent) :
    QObject(parent),
    m_socket(new QUdpSocket(this))
{
    setPort(45454);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

/*!
 * Sets the \a port on which we wait for incomming logs
 */
void LogReceiver::setPort(int port)
{
    m_port = port;
}

void LogReceiver::setAddress(const QString &address)
{
    m_address = QHostAddress(address);
}

void LogReceiver::connectToServer()
{
    m_socket->disconnectFromHost();
    m_socket->bind(m_address, m_port, QUdpSocket::ShareAddress);
}

/*!
 * The port on which we wait for incomming logs
 */
int LogReceiver::port() const
{
    return m_port;
}

QString LogReceiver::address() const
{
    return m_address.toString();
}

void LogReceiver::processPendingDatagrams()
{
    while (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());
        m_socket->readDatagram(datagram.data(), datagram.size());

        QStringList data = QString::fromUtf8(datagram).split("%%%");
        if (data.count() != 5) {
            qWarning("Invalid Log package received");
            return;
        }

        emit message(data.at(0).toInt(), data.at(1), QUrl(data.at(2)) ,data.at(3).toInt(), data.at(4).toInt());
    }
}
