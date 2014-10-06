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

#include "imageadapter.h"
#include <QImageReader>
#include <QDebug>
#include <QFileInfo>
#include <QQmlContext>

ImageAdapter::ImageAdapter(QObject *parent) :
    QObject(parent)
{
}

bool ImageAdapter::canPreview(const QString &path) const
{
    QString format = QImageReader::imageFormat(path);
    if(!format.isEmpty()) {
        if (format == "pcx") {
            if (QFileInfo(path).suffix() == "pcx")
                return true;
        } else {
            return true;
        }
    }

    return false;
}

QImage ImageAdapter::preview(const QString &path, const QSize &requestedSize)
{
    QImage img(path);

    if (requestedSize.isValid())
        return img.scaled(requestedSize, Qt::KeepAspectRatio);
    return img;
}

bool ImageAdapter::canAdapt(const QUrl &url) const
{
    return !QImageReader::imageFormat(url.toLocalFile()).isEmpty();
}

QUrl ImageAdapter::adapt(const QUrl &url, QQmlContext *context)
{
    context->setContextProperty("imageViewerBackgroundColor", "black");
    context->setContextProperty("imageViewerSource", url);

    return QUrl("qrc:/livert/imageviewer_qt5.qml");
}
