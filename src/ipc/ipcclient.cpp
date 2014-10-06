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

#include "ipcclient.h"
#include <QElapsedTimer>
#include <QPointer>

#ifdef QMLLIVE_IPC_DEBUG
#define DEBUG qDebug()
#else
#define DEBUG if (0) qDebug()
#endif

class Package : public QObject {
public:
    QUuid m_uuid;
    QString m_method;
    QByteArray m_data;
    int m_tries;
    qint64 m_bytes;
};

/*!
 * \class IpcClient
 * \brief Client to send remote calls to an IpcServer
 * \group ipc
 *
 * The IPC system uses the normalized signal/slot signature to
 * identify a message call. The arguments are passed as a QByteArray to the
 * send function.
 *
 * Here is a simple example:
 * \code{.cpp}
 *  IpcClient *client = new IpcClient(this);
 *  client->connectToServer("127.0.0.1", 10234);
 *  QString text = "Hello";
 *  QByteArray content;
 *  QDataStream out(&bytes, QIODevice::WriteOnly);
 *  out << text;
 *  QUuid uuid = client->send("echo(QString)", content);
 *  client->waitForSent(uuid);
 * \endcode
 *
 * Don't use the waitFor*-Methods in your gui applications. They will block
 * the eventloop. Instead react on the signals when the packages are sent or
 * an error happened.
 */

/*!
 * \brief Constructs an IpcClient with parent \a parent to send commands to an IpcServer.
 */
IpcClient::IpcClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_current(0)
    , m_written(0)
    , m_connection(new IpcConnection(m_socket))
{
    connect(m_socket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_socket, SIGNAL(connected()), this, SLOT(processQueue()));
    connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(m_socket, SIGNAL(bytesWritten(qint64)), this , SLOT(onBytesWritten(qint64)));

    connect(m_connection, SIGNAL(received(QString,QByteArray)), this, SIGNAL(received(QString,QByteArray)));
}

IpcClient::IpcClient(QTcpSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
    , m_current(0)
    , m_written(0)
    , m_connection(0)
{
    connect(m_socket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(m_socket, SIGNAL(bytesWritten(qint64)), this , SLOT(onBytesWritten(qint64)));
}

QAbstractSocket::SocketState IpcClient::state() const
{
    return m_socket->state();
}

/*!
 * Sets the Ip-Address to \a hostName and port to \a port to be used for a ipc call.
 */
void IpcClient::connectToServer(const QString &hostName, int port)
{
    m_socket->connectToHost(hostName, port);
}

/*!
 * Send call to server given by destination
 *
 * Expects the \a method to be in the form of "echo(QString)" and uses \a data as the content of the arguments
 * Returns a QUuid which identifies this Package
 *
 * \sa sentSuccessfully(), sendingError()
 */
QUuid IpcClient::send(const QString &method, const QByteArray &data)
{
    Package* pkg = new Package;
    pkg->m_method = method;
    pkg->m_data = data;
    pkg->m_uuid = QUuid::createUuid();
    pkg->m_bytes = 0;
    pkg->m_tries = 0;
    m_queue.enqueue(pkg);

    QTimer::singleShot(0, this, SLOT(processQueue()));

    return pkg->m_uuid;
}

/*!
 * Waits until the client is connected to the server
 * \a msecs specfies the time how long to wait
 * Returns true when the client is connected
 *
 * This call blocks your eventloop()
 *
 * \sa waitForDisconnected(), waitForSent()
 */
bool IpcClient::waitForConnected(int msecs)
{
    return m_socket->waitForConnected(msecs);
}

/*!
 * Waits until the client is disconnected from the server
 * \a msecs specfies the time how long to wait
 * Returns true when the client is disconnected
 *
 * This call blocks your eventloop()
 *
 * \sa waitForConnected(), waitForSent()
 */
bool IpcClient::waitForDisconnected(int msecs)
{
    return m_socket->waitForDisconnected(msecs);
}

/*!
 * Waits until the Package identified by \a uuid is sent
 * \arg msecs specfies the time how long to wait
 * Returns true when the Package is sent successfully
 *
 * This call blocks your eventloop()
 *
 * \sa waitForConnected(), waitForDisconnected()
 */
bool IpcClient::waitForSent(const QUuid uuid, int msecs)
{
    QPointer<Package> waitForPackage = 0;
    if (m_current && m_current->m_uuid == uuid) {
        waitForPackage = m_current;
    } else {
        foreach(Package* pkg, m_queue) {
            if (pkg->m_uuid == uuid) {
                waitForPackage = pkg;
                break;
            }
        }
    }

    if (!waitForPackage)
        return false;

    QElapsedTimer stopWatch;
    stopWatch.start();

    bool sent = false;
    while (!sent && (msecs == -1 || stopWatch.elapsed() < msecs)) {
        if (!m_socket->waitForBytesWritten(msecs - stopWatch.elapsed()))
            return false;

        if (!waitForPackage)
            return m_lastSuccess == uuid;
    }

    return false;
}

QString IpcClient::errorToString(QAbstractSocket::SocketError error)
{
    switch (error) {

    case QAbstractSocket::ConnectionRefusedError:
        return QString("The connection was refused by the peer (or timed out).");
    case QAbstractSocket::RemoteHostClosedError:
        return QString("The remote host closed the connection.");
    case QAbstractSocket::HostNotFoundError:
        return QString("The host address was not found.");
    case QAbstractSocket::SocketAccessError:
        return QString("You don't have the required privileges.");
    case QAbstractSocket::SocketResourceError:
        return QString("The local system ran out of resources (e.g., too many sockets).");
    case QAbstractSocket::SocketTimeoutError:
        return QString("The socket operation timed out.");
    case QAbstractSocket::DatagramTooLargeError:
        return QString("The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).");
    case QAbstractSocket::NetworkError:
        return QString("An error occurred with the network (e.g., the network cable was accidentally plugged out).");
    case QAbstractSocket::AddressInUseError:
        return QString("Address already in use.");
    case QAbstractSocket::SocketAddressNotAvailableError:
        return QString("Address not available.");
    case QAbstractSocket::UnsupportedSocketOperationError:
        return QString("Unsupported Socket.");
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        return QString("The socket is using a proxy, and the proxy requires authentication.");
    case QAbstractSocket::SslHandshakeFailedError:
        return QString("The SSL/TLS handshake failed, so the connection was closed");
    case QAbstractSocket::UnfinishedSocketOperationError:
        return QString("The last operation attempted has not finished yet (still in progress in the background).");
    case QAbstractSocket::ProxyConnectionRefusedError:
        return QString("Could not contact the proxy server because the connection to that server was denied.");
    case QAbstractSocket::ProxyConnectionClosedError:
        return QString("The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established).");
    case QAbstractSocket::ProxyConnectionTimeoutError:
        return QString("The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.");
    case QAbstractSocket::ProxyNotFoundError:
        return QString("The proxy address was not found.");
    case QAbstractSocket::ProxyProtocolError:
        return QString("The connection negotiation with the proxy server because the response from the proxy server could not be understood.");
    case QAbstractSocket::UnknownSocketError:
        return QString("Unknown Error");
    case QAbstractSocket::OperationError:
        return QString("An operation was attempted while the socket was in a state that did not permit it.");
    case QAbstractSocket::SslInternalError:
        return QString("The SSL library being used reported a internal error, this is probably the result of a bad installation or misconfiguration of the library.");
    case QAbstractSocket::SslInvalidUserDataError:
        return QString("Invalid data(certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library.");
    case QAbstractSocket::TemporaryError:
        return QString("A temporary error occurred(e.g., operation would block and socket is non-blocking).");
    }

    return QString("No Error Description for this Error");
}

/*!
 * Disconnects from the Server
 */
void IpcClient::disconnectFromServer()
{
    m_socket->disconnectFromHost();
}

void IpcClient::processQueue()
{
    if (m_current)
        return;

    if (!m_queue.isEmpty()) {
        m_current = m_queue.head();
        m_current->m_tries++;

        if (m_current->m_tries >= 5) {
            DEBUG << "Tried to sent the package" << m_current->m_tries << "times, but didn't succeed";
            m_queue.dequeue();
            onError(QAbstractSocket::ConnectionRefusedError);
            QTimer::singleShot(0, this, SLOT(processQueue()));
            return;
        }

        int size = sendPackage(m_current->m_method, m_current->m_data);

        if (size != -1) {
            m_queue.dequeue();
            m_current->m_bytes = size;
        } else {
            QTimer::singleShot(1000, this, SLOT(processQueue()));
            m_current = 0;
        }
    }
}

void IpcClient::onBytesWritten(qint64 written)
{
    Q_ASSERT(m_current);

    m_written += written;
    if (m_written < m_current->m_bytes)
        return;

    m_written = m_written - m_current->m_bytes;
    emit sentSuccessfully(m_current->m_uuid);
    m_lastSuccess = m_current->m_uuid;
    delete m_current;
    m_current = 0;

    processQueue();
}

void IpcClient::onError(QAbstractSocket::SocketError socketError)
{
    if (m_current) {
        emit sendingError(m_current->m_uuid, socketError);
        delete m_current;
        m_current = 0;

        QTimer::singleShot(0, this, SLOT(processQueue()));
    }

    if ((m_socket->state() != QAbstractSocket::ConnectedState &&
        m_socket->state() != QAbstractSocket::BoundState) ||
        socketError == QAbstractSocket::RemoteHostClosedError) {
        emit connectionError(socketError);
    }
}

qint64 IpcClient::sendPackage(const QString &method, const QByteArray &data)
{
    DEBUG << "IpcClient::send: " << method;

    if (!m_socket->isValid() || m_socket->state() != QAbstractSocket::ConnectedState) {
        DEBUG << "Tried to write on a Unconnected Socket. Try again later";
        return -1;
    }

    m_socket->write(QString("Method:%1\n").arg(method).toLatin1());
    m_socket->write(QString("Content-Length:%1\n").arg(data.length()).toLatin1());
    m_socket->write(QString("\n").toLatin1());
    m_socket->write(data);

    return m_socket->bytesToWrite();
}

/*!
 * \fn void IpcClient::connected();
 * Emitted once when the connection to the server is established
 */

/*!
 * \fn void IpcClient::disconnected();
 * Emitted once when the connection to the server is terminated
 */

/*!
 * \fn void IpcClient::connectionError(QAbstractSocket::SocketError socketError);
 * Emitted when an error happens when connecting or disconnecting from the server
 * \a socketError describes what error happened
 */

/*!
 * \fn void IpcClient::sentSuccessfully(const QUuid& uuid);
 * Emitted when the Package identified by \a uuid was successfully sent.
 */

/*!
 * \fn void IpcClient::sendingError(const QUuid& uuid, QAbstractSocket::SocketError socketError);
 * Emitted when an error happens when sending the Package identified by \a uuid.
 * \a socketError describes what error happened
 */

