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

#include <QAbstractListModel>
#include "host.h"

class QSettings;
class HostModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum HostRoles {
        NameRole = Qt::UserRole,
        AddressRole
    };

    explicit HostModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void addHost(Host* host);
    void removeHost(Host* host);
    void removeHost(int index);
    int indexOf(Host* host);
    void clear();

    QList<Host*> findByAutoDiscoveryId(QUuid id) const;

    Host* hostAt(int index) const;

    void restoreFromSettings(QSettings* s);
    void saveToSettings(QSettings* s);

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
protected:
    Qt::DropActions supportedDropActions() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

private slots:
    void onHostChanged();
private:
    QList<Host*> m_hosts;
};

