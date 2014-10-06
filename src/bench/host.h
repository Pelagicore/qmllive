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
#include <QUuid>
#include <QMetaType>

class QSettings;
class Host : public QObject
{
    Q_OBJECT
public:
    enum Type {
        AutoDiscovery,
        Manual
    };

    //If you add properties here, you maybe also have to change the hostform.ui and the hostform.h (HostChanges)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)
    Q_PROPERTY(int xOffset READ xOffset WRITE setXOffset NOTIFY xOffsetChanged)
    Q_PROPERTY(int yOffset READ yOffset WRITE setYOffset NOTIFY yOffsetChanged)
    Q_PROPERTY(int rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(bool followTreeSelection READ followTreeSelection WRITE setFollowTreeSelection NOTIFY followTreeSelectionChanged)
    Q_PROPERTY(QUuid autoDiscoveryId READ autoDiscoveryId WRITE setAutoDiscoveryId NOTIFY autoDiscoveryIdChanged)
    Q_PROPERTY(QString productVersion READ productVersion WRITE setProductVersion)
    Q_PROPERTY(QString systemName READ systemName WRITE setSystemName)

    explicit Host(Type type = Manual, QObject *parent = 0);
    Host(const Host& host, QObject *parent = 0);

    QString name() const;
    QString address() const;
    int port() const;
    QString currentFile() const;
    int xOffset() const;
    int yOffset() const;
    int rotation() const;
    Type type() const;

    bool online() const;
    bool followTreeSelection() const;
    QUuid autoDiscoveryId() const;
    QString productVersion() const;
    QString systemName() const;


    void saveToSettings(QSettings *s);
    void restoreFromSettings(QSettings *s);

signals:

    void nameChanged(QString arg);
    void addressChanged(QString arg);
    void portChanged(int arg);
    void currentFileChanged(QString arg);
    void xOffsetChanged(int arg);
    void yOffsetChanged(int arg);
    void rotationChanged(int arg);
    void onlineChanged(bool arg);
    void followTreeSelectionChanged(bool arg);
    void autoDiscoveryIdChanged(QUuid arg);


public slots:

    void setName(QString arg);
    void setAddress(QString arg);
    void setPort(int arg);
    void setCurrentFile(QString arg);
    void setXOffset(int arg);
    void setYOffset(int arg);
    void setRotation(int arg);
    void setOnline(bool arg);
    void setFollowTreeSelection(bool arg);
    void setAutoDiscoveryId(QUuid arg);
    void setProductVersion(QString arg);
    void setSystemName(QString arg);

private:

    QString m_name;
    QString m_address;
    int m_port;
    QString m_currentFile;
    int m_xOffset;
    int m_yOffset;
    int m_rotation;
    Type m_type;
    bool m_online;
    bool m_followTreeSelection;
    QUuid m_autoDiscoveryId;
    QString m_productVersion;
    QString m_systemName;
};

Q_DECLARE_METATYPE( Host* )

