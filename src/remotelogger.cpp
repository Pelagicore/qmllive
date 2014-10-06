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
#include "remotelogger.h"

/*!
 * \class RemoteLogger
 * \brief Installs a qt messageHandler and sends the logs over udp
 * \group qmllive
 *
 * \sa Logger, LogReceiver
 */

/*!
 * Standard constructor using \a parent as parent
 */
RemoteLogger::RemoteLogger(QObject *parent) :
    Logger(parent) ,
    m_socket(new QUdpSocket(this)) ,
    m_port(45454)
{
    connect(this, SIGNAL(message(int,QString)), this, SLOT(broadcast(int,QString)));
}

/*!
 * Sets the \a address where the log messages will be sent to
 * \sa setPort()
 */
void RemoteLogger::setHostAddress(const QHostAddress &address)
{
    m_host = address;
}

/*!
 * Sets the \a port where the log messages will be sent to
 * \sa setHostAddress()
 */
void RemoteLogger::setPort(int port)
{
    m_port = port;
}

void RemoteLogger::broadcast(int type, const QString &msg, const QUrl &url, int line, int column)
{
    QByteArray datagram;
    QStringList data;
    data << QString::number(type) << msg << url.toString() << QString::number(line) << QString::number(column);

    datagram = data.join("%%%").toUtf8();
    m_socket->writeDatagram(datagram.data(), datagram.size(),
                            m_host, m_port);
}

void RemoteLogger::appendToLog(const QList<QQmlError> &errors)
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

        broadcast(type, err.description(), err.url(), err.line(), err.column());
    }
}
