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

#include "ipcconnection.h"

#ifdef QMLLIVE_IPC_DEBUG
#define DEBUG qDebug()
#else
#define DEBUG if (0) qDebug()
#endif

/**
 * \class IpcConnection
 * \brief Handles a single connection from the IpcServer
 * \group ipc
 */

/**
 * \brief Constructs a IpcConnection with \a socket and a \a parent
 */
IpcConnection::IpcConnection(QTcpSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
    , m_headerComplete(false)
    , m_maxContentSize(1024*1024*10)
{
    DEBUG << "IpcConnection()";

    connect(m_socket, SIGNAL(disconnected()), this, SLOT(close()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(closeWithError()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readData()));
}

/**
 * \brief Close the connection and delete it
 */
void IpcConnection::close()
{
    DEBUG << "IpcConnection::close()";
    emit connectionClosed();
}

/**
 * \brief Report errors and close the connection
 */
void IpcConnection::closeWithError()
{
    DEBUG << "IpcConnection::closeWithError: " << m_socket->errorString();
    emit error(m_socket->errorString());
    close();
}

/**
 * \brief handles incoming data
 */
void IpcConnection::readData()
{
    while(m_socket->bytesAvailable()) {
        if(!m_headerComplete) {

            //Not enough bytesAvailable() try again later.
            if (!m_socket->canReadLine())
                return;

            while(m_socket->canReadLine()) {
                QString line = m_socket->readLine().trimmed();
                DEBUG << "\treceived header: " << line;
                if(line.isEmpty()) {
                    DEBUG << "\theader complete";
                    if(m_headers.contains("Method") || m_headers.contains("Content-Length")) {
                        m_headerComplete = true;
                    } else { // we can't recover
                        qWarning() << "\tincomplete header";
                        reset();
                    }
                    break;
                }
                QStringList parts = line.split(":");
                if(parts.count() != 2) {
                    qWarning() << "invalid header line: " << line;
                    break;
                }
                m_headers.insert(parts.at(0).trimmed(), parts.at(1).trimmed());
            }
        }
        if(m_headerComplete) {
            int bufferSize = m_headers.value("Content-Length").toInt();
            if(bufferSize > m_maxContentSize) {
                qWarning() << "content to large to be received. max size: " << m_maxContentSize;
                reset();
                return;
            }

            DEBUG << "receive content (bytes): " << bufferSize;
            if(m_socket->bytesAvailable() < bufferSize) {
                DEBUG << "content wait for more data";
                return;
            } else {
                QByteArray content;
                content.resize(bufferSize);
                if(m_socket->read(content.data(), bufferSize) != bufferSize) {
                    qWarning() << "error reading content from stream";
                }
                QString method = m_headers.value("Method");
                reset();
                emit received(method, content);
            }
        }
    }
}

/**
 * \brief Max bytes we are able to receive. Defaults to 10Mbytes.
 * Returns the max content size
 */
qint64 IpcConnection::maxContentSize() const
{
    return m_maxContentSize;
}

void IpcConnection::reset()
{
    m_headerComplete = false;
    m_headers.clear();
}

QTcpSocket *IpcConnection::socket() const
{
    return m_socket;
}
