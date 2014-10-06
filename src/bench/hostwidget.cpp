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

#include "hostwidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QFormLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QProgressBar>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>
#include <QToolButton>
#include <QDebug>

#include "host.h"
#include "livehubengine.h"

HostWidget::HostWidget(QWidget *parent) :
    QWidget(parent)
{
    setAcceptDrops(true);

    setFixedHeight(155);

    QGridLayout* layout = new QGridLayout(this);
    m_groupBox = new QGroupBox();
    layout->addWidget(m_groupBox);

    QVBoxLayout* vbox = new QVBoxLayout(m_groupBox);
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    m_ipLabel = new QLabel(m_groupBox);
    m_fileLabel = new QLabel(m_groupBox);
    m_statusLabel = new QLabel(m_groupBox);

    formLayout->addRow("IP:", m_ipLabel);
    formLayout->addRow("File:", m_fileLabel);
    formLayout->addRow("Status:",m_statusLabel);

    m_groupBox->installEventFilter(this);

    vbox->addLayout(formLayout);

    m_sendProgress = new QProgressBar(m_groupBox);
    m_sendProgress->setMaximum(1);
    m_sendProgress->setValue(1);
    m_menuButton = new QToolButton(m_groupBox);
    m_menuButton->setText("...");
    m_menuButton->setMinimumWidth(35);
    m_menuButton->setPopupMode(QToolButton::InstantPopup);
    m_menuButton->setCheckable(true);

    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addWidget(m_sendProgress);
    hbox->addWidget(m_menuButton);

    vbox->addLayout(hbox);


    m_menu = new QMenu(this);
    m_connectDisconnectAction = m_menu->addAction("Connect", this, SLOT(connectAndSendFile()));
    m_refreshAction = m_menu->addAction("Refresh", this, SLOT(refresh()));
    m_publishAction = m_menu->addAction("Publish All", this, SLOT(publishAll()));
    m_followTreeSelectionAction = m_menu->addAction("Follow Tree Selection");
    m_followTreeSelectionAction->setCheckable(true);
    m_editHostAction = m_menu->addAction("Edit Host", this, SLOT(onEditHost()));

    m_menuButton->setMenu(m_menu);

    connect(&m_publisher, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(&m_publisher, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(&m_publisher, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(&m_publisher, SIGNAL(connectionError(QAbstractSocket::SocketError)),
            this, SLOT(onConnectionError(QAbstractSocket::SocketError)));
    connect(&m_publisher, SIGNAL(sendingError(QUuid,QAbstractSocket::SocketError)),
            this, SLOT(onSendingError(QUuid,QAbstractSocket::SocketError)));
    connect(&m_publisher, SIGNAL(sentSuccessfully(QUuid)),
            this, SLOT(onSentSuccessfully(QUuid)));
    connect(&m_publisher, SIGNAL(needsPinAuthentication()), this, SLOT(showPinDialog()));
    connect(&m_publisher, SIGNAL(pinOk(bool)), this, SLOT(onPinOk(bool)));
    connect(&m_publisher, SIGNAL(remoteLog(int,QString,QUrl,int,int)),
            this, SIGNAL(remoteLog(int,QString,QUrl,int,int)));

    onDisconnected();
}

void HostWidget::setHost(Host *host)
{
    m_host = host;

    updateName(m_host->name());
    updateIp(m_host->address());
    updateFile(m_host->currentFile());
    updateOnlineState(m_host->online());
    m_followTreeSelectionAction->setChecked(m_host->followTreeSelection());

    connect(host, SIGNAL(addressChanged(QString)), this, SLOT(updateIp(QString)));
    connect(host, SIGNAL(portChanged(int)), this, SLOT(updatePort(int)));
    connect(host, SIGNAL(onlineChanged(bool)), this, SLOT(updateOnlineState(bool)));
    connect(host, SIGNAL(currentFileChanged(QString)), this, SLOT(updateFile(QString)));
    connect(host, SIGNAL(nameChanged(QString)), this, SLOT(updateName(QString)));
    connect(host, SIGNAL(xOffsetChanged(int)), this, SLOT(sendXOffset(int)));
    connect(host, SIGNAL(yOffsetChanged(int)), this, SLOT(sendYOffset(int)));
    connect(host, SIGNAL(rotationChanged(int)), this, SLOT(sendRotation(int)));
    connect(host, SIGNAL(followTreeSelectionChanged(bool)),
            m_followTreeSelectionAction, SLOT(setChecked(bool)));

    connect(m_followTreeSelectionAction, SIGNAL(triggered(bool)), host, SLOT(setFollowTreeSelection(bool)));
}

void HostWidget::setLiveHubEngine(LiveHubEngine *engine)
{
    m_engine = engine;

    m_publisher.setWorkspace(m_engine->workspace());

    connect(m_engine.data(), SIGNAL(workspaceChanged(QString)), &m_publisher, SLOT(setWorkspace(QString)));
    connect(m_engine.data(), SIGNAL(fileChanged(QString)), this, SLOT(sendDocument(QString)));
}

void HostWidget::setCurrentFile(const QString currentFile)
{
    m_host->setCurrentFile(currentFile);
}

bool HostWidget::followTreeSelection() const
{
    return m_followTreeSelectionAction->isChecked();
}

void HostWidget::updateName(const QString &name)
{
    m_groupBox->setTitle(name);
}

void HostWidget::updateIp(const QString &ip)
{
    m_ipLabel->setText(QString("%1:%2").arg(ip).arg(m_host->port()));

    QTimer::singleShot(0, this, SLOT(connectToServer()));
}

void HostWidget::updatePort(int port)
{
    m_ipLabel->setText(QString("%1:%2").arg(m_host->address()).arg(port));

    QTimer::singleShot(0, this, SLOT(connectToServer()));
}

void HostWidget::updateFile(const QString &file)
{
    QString relFile = QDir(m_engine->workspace()).relativeFilePath(file);
    setUpdateFile(relFile);
    m_fileLabel->setToolTip(relFile);

    connectAndSendFile();
}

void HostWidget::setUpdateFile(const QString &file)
{
    QFontMetrics metrics(font());
    m_fileLabel->setText(metrics.elidedText(file, Qt::ElideLeft, m_fileLabel->width()));
}

void HostWidget::updateOnlineState(bool online)
{
    qDebug() << "updateOnline";

    bool available = online || m_host->type() == Host::Manual;

    if (available)
        QTimer::singleShot(0, this, SLOT(connectToServer()));
    else
        onDisconnected();
}

void HostWidget::connectToServer()
{
    qDebug() << "connectToServer";

    if (m_publisher.state() == QAbstractSocket::ConnectedState || m_publisher.state() == QAbstractSocket::ConnectingState) {
        return;
    }

    if (m_host->online() || m_host->type() == Host::Manual) {
        m_publisher.connectToServer(m_host->address(), m_host->port());
        m_activateId = QUuid();
        m_rotationId = QUuid();
        m_xOffsetId = QUuid();
        m_yOffsetId = QUuid();
        m_changeIds.clear();
    }
}

void HostWidget::connectAndSendFile()
{
    connectToServer();
    m_activateId = m_publisher.activateDocument(QDir(m_engine->workspace()).relativeFilePath(m_host->currentFile()));
}

void HostWidget::onConnected()
{
    m_statusLabel->setPixmap(QPixmap(":/images/icon_online.png"));
    m_statusLabel->setToolTip("Host online");

    sendXOffset(m_host->xOffset());
    sendYOffset(m_host->yOffset());
    sendRotation(m_host->rotation());

    m_connectDisconnectAction->setText("Disconnect");
    disconnect(m_connectDisconnectAction, SIGNAL(triggered()), 0, 0);
    connect(m_connectDisconnectAction, SIGNAL(triggered()), &m_publisher, SLOT(disconnectFromServer()));
}

void HostWidget::onDisconnected()
{
    m_statusLabel->setPixmap(QPixmap(":/images/icon_offline.png"));
    m_statusLabel->setToolTip("Host offline");

    m_connectDisconnectAction->setText("Connect");
    disconnect(m_connectDisconnectAction, SIGNAL(triggered()), 0, 0);
    connect(m_connectDisconnectAction, SIGNAL(triggered()), this, SLOT(connectToServer()));
}

void HostWidget::onConnectionError(QAbstractSocket::SocketError error)
{
    m_statusLabel->setPixmap(QPixmap(":/images/icon_failover.png"));
    m_statusLabel->setToolTip(m_publisher.errorToString(error));

    if (error == QAbstractSocket::RemoteHostClosedError)
        m_host->setOnline(false);

    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        onDisconnected();
}

void HostWidget::refresh()
{
    connectAndSendFile();
}

void HostWidget::publishWorkspace()
{
    connectToServer();
    connect(m_engine.data(), SIGNAL(publishFile(QString)), this, SLOT(sendDocument(QString)));
    m_engine->publishWorkspace();
    disconnect(m_engine.data(), SIGNAL(publishFile(QString)), this, SLOT(sendDocument(QString)));
}

void HostWidget::sendDocument(const QString& document)
{
    if (m_publisher.state() != QAbstractSocket::ConnectedState)
        return;

    m_changeIds.append(m_publisher.sendDocument(document));
    m_sendProgress->setMaximum(m_sendProgress->maximum() + 1);
}

void HostWidget::sendXOffset(int offset)
{
    m_xOffsetId = m_publisher.setXOffset(offset);
}

void HostWidget::sendYOffset(int offset)
{
    m_yOffsetId = m_publisher.setYOffset(offset);
}

void HostWidget::sendRotation(int rotation)
{
    m_rotationId = m_publisher.setRotation(rotation);
}

void HostWidget::onSendingError(const QUuid &uuid, QAbstractSocket::SocketError socketError)
{
    if (uuid == m_activateId) {
        m_statusLabel->setPixmap(QPixmap(":/images/icon_failover.png"));
        m_statusLabel->setToolTip(QString("Activating file failed: %1").arg(m_publisher.errorToString(socketError)));
        m_activateId = QUuid();
    } else if (uuid == m_xOffsetId) {
        m_statusLabel->setPixmap(QPixmap(":/images/icon_failover.png"));
        m_statusLabel->setToolTip(QString("Setting the X Offset failed: %1").arg(m_publisher.errorToString(socketError)));
        m_xOffsetId = QUuid();
    } else if (uuid == m_yOffsetId) {
        m_statusLabel->setPixmap(QPixmap(":/images/icon_failover.png"));
        m_statusLabel->setToolTip(QString("Setting the Y Offset failed: %1").arg(m_publisher.errorToString(socketError)));
        m_yOffsetId = QUuid();
    } else if (uuid == m_rotationId) {
        m_statusLabel->setPixmap(QPixmap(":/images/icon_failover.png"));
        m_statusLabel->setToolTip(QString("Setting the Rotation failed: %1").arg(m_publisher.errorToString(socketError)));
        m_rotationId = QUuid();
    }else if (m_changeIds.contains(uuid)) {
        m_statusLabel->setPixmap(QPixmap(":/images/icon_failover.png"));
        m_statusLabel->setToolTip(QString("Not all files were synced successfully: %1").arg(m_publisher.errorToString(socketError)));
        m_changeIds.removeAll(uuid);
        resetProgressBar();
    }
}

void HostWidget::onSentSuccessfully(const QUuid &uuid)
{
    if (uuid == m_activateId) {
        m_statusLabel->setPixmap(QPixmap(":/images/icon_online.png"));
        m_activateId = QUuid();
    } else if (uuid == m_xOffsetId) {
        m_statusLabel->setPixmap(QPixmap(":/images/icon_online.png"));
        m_xOffsetId = QUuid();
    } else if (uuid == m_yOffsetId) {
        m_statusLabel->setPixmap(QPixmap(":/images/icon_online.png"));
        m_yOffsetId = QUuid();
    } else if (uuid == m_rotationId) {
        m_statusLabel->setPixmap(QPixmap(":/images/icon_online.png"));
        m_rotationId = QUuid();
    } else if (m_changeIds.contains(uuid)) {
        m_changeIds.removeAll(uuid);
        m_sendProgress->setValue(m_sendProgress->value() + 1);
        if (m_changeIds.isEmpty()) {
            m_statusLabel->setPixmap(QPixmap(":/images/icon_online.png"));
            resetProgressBar();
        }
    }
}

void HostWidget::resetProgressBar()
{
    m_sendProgress->setValue(1);
    m_sendProgress->setMaximum(1);
}

void HostWidget::onPinOk(bool ok)
{
    if (!ok) {
        m_publisher.disconnectFromServer();
        m_statusLabel->setPixmap(QPixmap(":/images/icon_failover.png"));
        m_statusLabel->setToolTip("The Host didn't accept your pin");
        QMessageBox::warning(this, "Pin not accepted", "The Host didn't accept your pin");
    }
}

void HostWidget::publishAll()
{
    if (QMessageBox::question(this, QString("Publish %1").arg(m_engine->workspace()),
                              QString("Do you really want to publish the content of %1").arg(m_engine->workspace())) == QMessageBox::Yes) {
        publishWorkspace();
    }
}

void HostWidget::onEditHost()
{
    emit openHostConfig(m_host);
}

void HostWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    setUpdateFile(m_fileLabel->toolTip());
}

void HostWidget::showPinDialog()
{
    bool ok = false;
    int pin = QInputDialog::getInt(this, "The Host needs a Pin Authentication", "Pin", 0, 0, 9999, 1, &ok);

    if (!ok)
        m_publisher.disconnectFromServer();

    m_publisher.checkPin(QString::number(pin));
}

void HostWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list") && (m_host->online() || m_host->type() == Host::Manual)) {

        event->acceptProposedAction();
    }
}

void HostWidget::dropEvent(QDropEvent *event)
{
    QUrl url(event->mimeData()->text());

    if (url.isLocalFile())
        m_host->setCurrentFile(url.toLocalFile());
    event->acceptProposedAction();
}

bool HostWidget::eventFilter(QObject *object, QEvent *event)
{
    if ((event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress) && object == m_groupBox) {

        event->ignore();

        return true;
    }

    return false;
}
