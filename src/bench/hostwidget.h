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

#include <QWidget>
#include <QPointer>
#include <remotepublisher.h>

class QLabel;
class QGroupBox;
class QProgressBar;
class QPushButton;
class QToolButton;
class QMenu;
class Host;
class HostWidget : public QWidget
{
    Q_OBJECT
public:

    explicit HostWidget(QWidget *parent = 0);

    void setHost(Host* host);
    void setLiveHubEngine(LiveHubEngine* engine);
    void setCurrentFile(const QString currentFile);
    bool followTreeSelection() const;

signals:
    void connected();
    void openHostConfig(Host*);
    void remoteLog(int type, const QString &msg, const QUrl &url = QUrl(), int line = -1, int column = -1);

public slots:
    void publishWorkspace();
    void refresh();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    bool eventFilter(QObject *, QEvent *);

private slots:
    void updateName(const QString& name);
    void updateIp(const QString& ip);
    void updatePort(int port);
    void updateFile(const QString& file);
    void setUpdateFile(const QString& file);
    void updateOnlineState(bool online);

    void connectToServer();
    void connectAndSendFile();

    void onConnected();
    void onDisconnected();
    void onConnectionError(QAbstractSocket::SocketError error);

    void sendDocument(const QString &document);

    void sendXOffset(int offset);
    void sendYOffset(int offset);
    void sendRotation(int rotation);

    void onSentSuccessfully(const QUuid &uuid);
    void onSendingError(const QUuid &uuid, QAbstractSocket::SocketError socketError);
    void resetProgressBar();

    void showPinDialog();
    void onPinOk(bool ok);

    void publishAll();
    void onEditHost();

    void resizeEvent( QResizeEvent * event );
private:

    QGroupBox* m_groupBox;
    QLabel* m_ipLabel;
    QLabel* m_fileLabel;
    QLabel* m_statusLabel;
    QProgressBar* m_sendProgress;
    QToolButton* m_menuButton;
    QMenu* m_menu;
    QAction* m_publishAction;
    QAction* m_followTreeSelectionAction;
    QAction* m_refreshAction;
    QAction* m_connectDisconnectAction;
    QAction* m_editHostAction;

    QPointer<Host> m_host;

    RemotePublisher m_publisher;
    QPointer<LiveHubEngine> m_engine;

    QUuid m_activateId;
    QList<QUuid> m_changeIds;
    QUuid m_xOffsetId;
    QUuid m_yOffsetId;
    QUuid m_rotationId;
};

