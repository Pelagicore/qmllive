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

#include "host.h"
#include <QDebug>
#include <QSettings>

Host::Host(Type type, QObject *parent) :
    QObject(parent),
    m_port(10234),
    m_xOffset(0),
    m_yOffset(0),
    m_rotation(0),
    m_type(type),
    m_online(false),
    m_followTreeSelection(false)
{
}

Host::Host(const Host &host, QObject *parent) :
    QObject(parent),
    m_name(host.name()),
    m_address(host.address()),
    m_port(host.port()),
    m_currentFile(host.currentFile()),
    m_xOffset(host.xOffset()),
    m_yOffset(host.yOffset()),
    m_rotation(host.rotation()),
    m_type(host.type()),
    m_online(host.online()),
    m_followTreeSelection(host.followTreeSelection()),
    m_autoDiscoveryId(host.autoDiscoveryId()),
    m_productVersion(host.productVersion()),
    m_systemName(host.systemName())
{
}

QString Host::name() const
{
    return m_name;
}

QString Host::address() const
{
    return m_address;
}

QString Host::currentFile() const
{
    return m_currentFile;
}

int Host::xOffset() const
{
    return m_xOffset;
}

int Host::yOffset() const
{
    return m_yOffset;
}

int Host::rotation() const
{
    return m_rotation;
}

void Host::setName(QString arg)
{
    if (m_name != arg) {
        m_name = arg;
        emit nameChanged(arg);
    }
}

void Host::setAddress(QString arg)
{
    if (m_address != arg) {
        m_address = arg;
        emit addressChanged(arg);
    }
}

void Host::setCurrentFile(QString arg)
{
    m_currentFile = arg;
    emit currentFileChanged(arg);
}

void Host::setXOffset(int arg)
{
    if (m_xOffset != arg) {
        m_xOffset = arg;
        emit xOffsetChanged(arg);
    }
}

void Host::setYOffset(int arg)
{
    if (m_yOffset != arg) {
        m_yOffset = arg;
        emit yOffsetChanged(arg);
    }
}

void Host::setRotation(int arg)
{
    if (m_rotation != arg) {
        m_rotation = arg;
        emit rotationChanged(arg);
    }
}

void Host::setOnline(bool arg)
{
    if (m_online != arg) {
        m_online = arg;
        emit onlineChanged(arg);
    }
}

void Host::setFollowTreeSelection(bool arg)
{
    if (m_followTreeSelection != arg) {
        m_followTreeSelection = arg;
        emit followTreeSelectionChanged(arg);
    }
}

void Host::setAutoDiscoveryId(QUuid arg)
{
    if (m_autoDiscoveryId != arg) {
        m_autoDiscoveryId = arg;
        emit autoDiscoveryIdChanged(arg);
    }
}

void Host::setProductVersion(QString arg)
{
    m_productVersion = arg;
}

void Host::setSystemName(QString arg)
{
    m_systemName = arg;
}

void Host::setPort(int arg)
{
    if (m_port != arg) {
        m_port = arg;
        emit portChanged(arg);
    }
}

Host::Type Host::type() const
{
    return m_type;
}

bool Host::online() const
{
    return m_online;
}

bool Host::followTreeSelection() const
{
    return m_followTreeSelection;
}

void Host::saveToSettings(QSettings *s)
{
    Q_ASSERT(s);

    s->setValue("name", name());
    s->setValue("address", address());
    s->setValue("port", port());
    s->setValue("type", type());
    s->setValue("followTreeSelection", followTreeSelection());
    s->setValue("xOffset", xOffset());
    s->setValue("yOffset", yOffset());
    s->setValue("rotation", rotation());
    s->setValue("currentFile", currentFile());
    s->setValue("autoDiscoveryId", autoDiscoveryId().toString());
    s->setValue("systemName", systemName());
    s->setValue("productVersion", productVersion());
}

void Host::restoreFromSettings(QSettings *s)
{
    Q_ASSERT(s);

    setName(s->value("name").toString());
    setAddress(s->value("address").toString());
    setPort(s->value("port").toInt());
    m_type = static_cast<Host::Type>(s->value("type").toInt());
    setFollowTreeSelection(s->value("followTreeSelection").toBool());
    setXOffset(s->value("xOffset").toInt());
    setYOffset(s->value("yOffset").toInt());
    setRotation(s->value("rotation").toInt());
    setCurrentFile(s->value("currentFile").toString());
    setAutoDiscoveryId(QUuid(s->value("autoDiscoveryId").toString()));
    setSystemName(s->value("systemName").toString());
    setProductVersion(s->value("productVersion").toString());
}

QString Host::productVersion() const
{
    return m_productVersion;
}

QString Host::systemName() const
{
    return m_systemName;
}

QUuid Host::autoDiscoveryId() const
{
    return m_autoDiscoveryId;
}

int Host::port() const
{
    return m_port;
}
