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

#include "contentadapterinterface.h"
#include <QStringList>
#include <QFontDatabase>

class QDeclarativeContext;
class FontAdapter : public QObject, public ContentAdapterInterface
{
    Q_OBJECT
    Q_INTERFACES(ContentAdapterInterface)
public:
    explicit FontAdapter(QObject *parent = 0);

    void cleanUp();

    bool canPreview(const QString& path) const;
    QImage preview(const QString& path, const QSize &requestedSize);

    bool canAdapt(const QUrl& url) const;

    bool isFullScreen() const;

    virtual QUrl adapt(const QUrl& url, QQmlContext* context);

private:
    QStringList fontExtensions;
    QFontDatabase base;
    int fontId;
};
