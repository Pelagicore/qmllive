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
#include <QUrl>
#include <QImage>

class QDeclarativeContext;
class QQmlContext;
//! [0]
class ContentAdapterInterface
{
public:

    enum Feature {
        QtQuickControls = 0x1
    };
    Q_DECLARE_FLAGS(Features, Feature)

    virtual ~ContentAdapterInterface() {}

    virtual void cleanUp() {}

    virtual bool canPreview(const QString& path) const = 0;
    virtual QImage preview(const QString& path, const QSize &requestedSize) = 0;

    virtual bool canAdapt(const QUrl& url) const = 0;
    virtual bool isFullScreen() const { return false; }

    void setAvailableFeatures(ContentAdapterInterface::Features features) { m_features = features; }
    ContentAdapterInterface::Features availableFeatures() { return m_features; }

    virtual QUrl adapt(const QUrl& url, QQmlContext* context) = 0;

private:
    Features m_features;
};
//! [0]

Q_DECLARE_OPERATORS_FOR_FLAGS(ContentAdapterInterface::Features)
Q_DECLARE_INTERFACE(ContentAdapterInterface, "com.pelagicore.qmllive.ContentAdapterInterface/1.0")

