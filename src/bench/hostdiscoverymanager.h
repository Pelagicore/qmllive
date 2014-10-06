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

#include <QObject>
#include <QtCore>

class HostModel;
class Host;
class HostDiscoveryManager : public QObject
{
    Q_OBJECT
public:
    explicit HostDiscoveryManager(QObject *parent = 0);

    void rescan();

    void setKnownHostsModel(HostModel* model);
    HostModel *knownHostsModel() const;

    HostModel *discoveredHostsModel() const;

private slots:
    void deviceAdded(const QUuid &uuid, const QString &type, int version, const QString &domain);
    void deviceChanged(const QUuid &uuid, const QString &type, int version, const QString &domain);
    void deviceRemoved(const QUuid &uuid, const QString &type, int version, const QString &domain);

    void serviceAdded(const QUuid &uuid, const QString &type, int version, const QString &domain);
    void serviceChanged(const QUuid &uuid, const QString &type, int version, const QString &domain);
    void serviceRemoved(const QUuid &uuid, const QString &type, int version, const QString &domain);
    void serviceAlive(const QUuid &uuid, const QString &type, int version, const QString &domain);

    void updateHostFromExtraValues(Host *host, QMap<QString, QStringList> extraValues);
    bool checkExtraValues(QMap<QString, QStringList> extraValues);

private:
    HostModel* m_discoverymodel;
    HostModel* m_knownhostsmodel;
};

