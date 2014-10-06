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

#include "ipcserver.h"
#include "ipcconnection.h"

#ifdef QMLLIVE_IPC_DEBUG
#define DEBUG qDebug()
#else
#define DEBUG if (0) qDebug()
#endif

/*!
 * \class IpcServer
 * \brief The IpcServer listens on a port and creates an IpcConnection for an incoming
 * connection.
 * \group ipc
 *
 * The IPC server receives a method call from the client an notifies the user through
 * the IpcServer::received signal.
 *
 * \code{.cpp}
 * m_server = new IpcServer(this);
 * connect(
 *   m_server, SIGNAL(received(QString,QByteArray)),
 *   this, SLOT(handleCall(QString, QByteArray))
 * );
 * m_server->listen(10234);
 *
 * ...
 *
 * MyHandler::handleCall(const QString& method, const QByteArray& contnt) {
 *   if(method == "echo(QString)") {
 *     QString text;
 *     QDataStream in(content);
 *     in >> text;
 *     qDebug() << "call received: " << method << ": " << text;
 *   }
 * }
 * \endcode
 */

/*!
 * \brief Standard constructor using \a parent as parent object
 */
IpcServer::IpcServer(QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
{
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

/*!
 * \brief listens to incomong connections on \a port
 */
void IpcServer::listen(int port)
{
    DEBUG << "IpcServer::listen: " << port;
    m_server->listen(QHostAddress::Any, port);
}

/*!
 * \brief Creates a IpcConnection on incoming connection
 */
void IpcServer::newConnection()
{
    DEBUG << "IpcServer::newConnection";
    if(m_server->hasPendingConnections()) {
        QTcpSocket* socket = m_server->nextPendingConnection();
        emit clientConnected(socket->peerAddress());
        emit clientConnected(socket);
        IpcConnection* connection = new IpcConnection(socket, this);
        connect(connection, SIGNAL(connectionClosed()), this, SLOT(onConnectionClosed()));
        connect(connection, SIGNAL(received(QString,QByteArray)), this, SIGNAL(received(QString,QByteArray)));
    }
}

/*!
 * \fn void IpcServer::received(const QString& method, const QByteArray& content);
 * \brief signals a ipc call has arrived
 *
 * A ipc call requesting \a method and using \a content a the parameters for the method
 */


void IpcServer::onConnectionClosed()
{
    IpcConnection* connection = qobject_cast<IpcConnection*>(sender());

    emit clientDisconnected(connection->socket()->peerAddress());
    emit clientDisconnected(connection->socket());

    if(connection->parent() == this)
        delete connection;
}

void IpcServer::setMaxConnections(int num)
{
    m_server->setMaxPendingConnections(num);
}
