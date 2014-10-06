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

class QDeclarativeContext;
class DirectoryPreviewAdapter : public QObject, public ContentAdapterInterface
{
    Q_OBJECT
    Q_INTERFACES(ContentAdapterInterface)
public:
    explicit DirectoryPreviewAdapter(QObject *parent = 0);

    bool canPreview(const QString& path) const;
    QImage preview(const QString& path, const QSize &requestedSize);

    bool isFullScreen() const;

    bool canAdapt(const QUrl& url) const;
    virtual QUrl adapt(const QUrl& url, QQmlContext* context);

Q_SIGNALS:

    void loadDocument(const QString& path);
};
