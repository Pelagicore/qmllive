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
#include <QAbstractSocket>

class LiveHubEngine;
class IpcClient;

class RemotePublisher : public QObject
{
    Q_OBJECT
public:
    explicit RemotePublisher(QObject *parent = 0);
    void connectToServer(const QString& hostName, int port);
    QString errorToString(QAbstractSocket::SocketError error);
    QAbstractSocket::SocketState state() const;

    void registerHub(LiveHubEngine *hub);
Q_SIGNALS:
    void connected();
    void disconnected();
    void sentSuccessfully(const QUuid& uuid);
    void sendingError(const QUuid& uuid, QAbstractSocket::SocketError socketError);
    void connectionError(QAbstractSocket::SocketError error);
    void needsPinAuthentication();
    void pinOk(bool ok);
    void remoteLog(int type, const QString &msg, const QUrl &url = QUrl(), int line = -1, int column = -1);

public Q_SLOTS:
    void setWorkspace(const QString &path);
    void disconnectFromServer();
    QUuid activateDocument(const QString& document);
    QUuid sendDocument(const QString& document);
    QUuid checkPin(const QString& pin);
    QUuid setXOffset(int offset);
    QUuid setYOffset(int offset);
    QUuid setRotation(int rotation);

private Q_SLOTS:
    void handleCall(const QString &method, const QByteArray &content);
    QUuid sendWholeDocument(const QString &document);

    void onSentSuccessfully(const QUuid& uuid);
    void onSendingError(const QUuid& uuid, QAbstractSocket::SocketError socketError);

private:
    IpcClient *m_ipc;
    LiveHubEngine *m_hub;
    QDir m_workspace;

    QHash<QUuid, QString> m_packageHash;
};
