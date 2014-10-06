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

#include "hostdiscoverymanager.h"
#include "hostmodel.h"

HostDiscoveryManager::HostDiscoveryManager(QObject *parent) :
    QObject(parent) ,
    m_discoverymodel(new HostModel(this)),
    m_knownhostsmodel(0)
{
}

void HostDiscoveryManager::rescan()
{
}

void HostDiscoveryManager::setKnownHostsModel(HostModel *model)
{
    m_knownhostsmodel = model;
}

HostModel *HostDiscoveryManager::knownHostsModel() const
{
    return m_knownhostsmodel;
}

HostModel *HostDiscoveryManager::discoveredHostsModel() const
{
    return m_discoverymodel;
}

void HostDiscoveryManager::deviceAdded(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);
}

void HostDiscoveryManager::deviceChanged(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);
}

void HostDiscoveryManager::deviceRemoved(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);
}

void HostDiscoveryManager::serviceAdded(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);}

void HostDiscoveryManager::serviceChanged(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);}

void HostDiscoveryManager::serviceRemoved(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);}

void HostDiscoveryManager::updateHostFromExtraValues(Host* host, QMap<QString, QStringList> extraValues)
{
    Q_UNUSED(host);
    Q_UNUSED(extraValues);
}

bool HostDiscoveryManager::checkExtraValues(QMap<QString, QStringList> extraValues)
{
    Q_UNUSED(extraValues);
    return true;
}

void HostDiscoveryManager::serviceAlive(const QUuid &uuid, const QString &type, int version, const QString &domain)
{
    Q_UNUSED(uuid);
    Q_UNUSED(type);
    Q_UNUSED(version);
    Q_UNUSED(domain);
}
