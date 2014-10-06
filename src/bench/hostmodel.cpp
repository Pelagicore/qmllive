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

#include "hostmodel.h"
#include <QDebug>
#include <QIcon>
#include <QSettings>
#include <QMimeData>

HostModel::HostModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int HostModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_hosts.count();
}

int HostModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 5;
}

QVariant HostModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_hosts.count())
        return QVariant();

    Host* host = m_hosts.at(index.row());

    switch (index.column()) {
    case 0: {
        switch (role) {
        case NameRole: return host->name();
        case AddressRole: return host->address();
        }
        break;
    }
    case 1: if (role == Qt::DisplayRole) return host->name(); break;
    case 2: if (role == Qt::DisplayRole) return host->productVersion(); break;
    case 3: if (role == Qt::DisplayRole) return host->systemName(); break;
    case 4: if (role == Qt::DisplayRole) return QString("%1:%2").arg(host->address()).arg(host->port()); break;
    }

    return QVariant();
}

QVariant HostModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
        return QVariant();

    switch (section) {
    case 0: if (role == Qt::DisplayRole) return QVariant();
    case 1: if (role == Qt::DisplayRole) return QString("Name");
    case 2: if (role == Qt::DisplayRole) return QString("Version");
    case 3: if (role == Qt::DisplayRole) return QString("System");
    case 4: if (role == Qt::DisplayRole) return QString("Ip");
    }

    return QVariant();
}

void HostModel::addHost(Host *host)
{
    Q_ASSERT(host);

    beginInsertRows(QModelIndex(), m_hosts.count(), m_hosts.count());
    m_hosts.append(host);
    connect(host, SIGNAL(nameChanged(QString)), this, SLOT(onHostChanged()));
    connect(host, SIGNAL(addressChanged(QString)), this, SLOT(onHostChanged()));
    connect(host, SIGNAL(portChanged(int)), this, SLOT(onHostChanged()));
    connect(host, SIGNAL(followTreeSelectionChanged(bool)), this, SLOT(onHostChanged()));
    connect(host, SIGNAL(currentFileChanged(QString)), this, SLOT(onHostChanged()));
    connect(host, SIGNAL(xOffsetChanged(int)), this, SLOT(onHostChanged()));
    connect(host, SIGNAL(yOffsetChanged(int)), this, SLOT(onHostChanged()));
    connect(host, SIGNAL(rotationChanged(int)), this, SLOT(onHostChanged()));
    connect(host, SIGNAL(onlineChanged(bool)), this, SLOT(onHostChanged()));

    endInsertRows();
}

void HostModel::removeHost(Host *host)
{
    int idx = m_hosts.indexOf(host);

    if (idx == -1)
        return;

    beginRemoveRows(QModelIndex(), idx, idx);
    Host *item = m_hosts.takeAt(idx);
    delete item;
    endRemoveRows();
}

void HostModel::removeHost(int index)
{
    if (index < 0 || index >= m_hosts.count())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    Host *item = m_hosts.takeAt(index);
    delete item;
    endRemoveRows();
}

int HostModel::indexOf(Host *host)
{
    return m_hosts.indexOf(host);
}

void HostModel::clear()
{
    beginResetModel();
    qDeleteAll(m_hosts);
    m_hosts.clear();
    endResetModel();
}

QList<Host *> HostModel::findByAutoDiscoveryId(QUuid id) const
{
    QList<Host* > hosts;
    foreach(Host* host, m_hosts) {
        if (host->autoDiscoveryId() == id)
            hosts.append(host);
    }

    return hosts;
}

Host *HostModel::hostAt(int index) const
{
    if (index < 0 || index >= m_hosts.count())
        return 0;

    return m_hosts.at(index);
}

void HostModel::onHostChanged()
{
    Host* host = qobject_cast<Host*>(sender());

    if (host) {
        int idx = m_hosts.indexOf(host);
        if (idx != -1) {
            emit dataChanged(index(idx, 0), index(idx, 3));
        }
    }
}

void HostModel::restoreFromSettings(QSettings *s)
{
    Q_ASSERT(s);

    beginResetModel();
    qDeleteAll(m_hosts);
    m_hosts.clear();

    s->beginGroup("AllHosts");
    int size = s->beginReadArray("host");

    for(int i=0; i < size; i++) {
        s->setArrayIndex(i);
        Host* host = new Host(Host::AutoDiscovery, this);
        host->restoreFromSettings(s);
        m_hosts.append(host);
    }

    s->endArray();
    s->endGroup();

    endResetModel();
}

void HostModel::saveToSettings(QSettings *s)
{
    Q_ASSERT(s);

    s->beginGroup("AllHosts");
    s->beginWriteArray("host", m_hosts.size());

    int i=0;
    foreach(Host* host, m_hosts) {
        s->setArrayIndex(i++);
        host->saveToSettings(s);
    }

    s->endArray();
    s->endGroup();
}

bool HostModel::removeRows ( int row, int count, const QModelIndex & parent)
{
    if (row < 0 || row >= m_hosts.count() || parent.isValid())
        return false;

    beginRemoveRows(QModelIndex(), row, row + count -1);
    for(int i=0; i<count; i++) {
        Host *item = m_hosts.takeAt(row + i);
        delete item;
    }
    endRemoveRows();

    return true;
}

bool HostModel::dropMimeData(const QMimeData *data,
    Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("application/x-qabstractitemmodeldatalist"))
        return false;

    if (column > 0)
        return false;

    if (row < 0)
        return false;

    if (parent.isValid())
        return false;

    QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    while (!stream.atEnd())
    {
        int sourceRow, sourceCol;
        QMap<int,  QVariant> roleDataMap;
        stream >> sourceRow >> sourceCol >> roleDataMap;

        if (sourceRow == row -1 || sourceRow == row)
            return false;

        beginMoveRows(QModelIndex(), sourceRow, sourceRow, QModelIndex(), row);
        m_hosts.move(sourceRow, (row - sourceRow) > 0 ? row - 1 : row);
        endMoveRows();

        return true;
    }

    return false;
}

Qt::DropActions HostModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::ItemFlags HostModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}
