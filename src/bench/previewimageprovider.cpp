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

#include "previewimageprovider.h"
#include "contentadapterinterface.h"

#include <QDebug>
#include <QFileIconProvider>
#include <QPainter>


class CacheImage {
public:
    QImage image;
    QDateTime time;
};

typedef QHash<QString, CacheImage> ImageHash;
typedef QHash<QString, QImage> QImageHash;

Q_GLOBAL_STATIC(ImageHash, imageCache)
Q_GLOBAL_STATIC(QImageHash, iconCache)

PreviewImageProvider::PreviewImageProvider(QObject *engine)
    : QQuickImageProvider(QQuickImageProvider::Image, QQmlImageProviderBase::ForceAsynchronousImageLoading)
    , m_engine(engine)
    , m_ignoreCache(false)
{
}

QImage PreviewImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_ASSERT(size);
    QFileInfo info(id);
    info.refresh();
    QList<ContentAdapterInterface*> plugins;
    QString hashKey = id + QString("%1x%2").arg(QString::number(requestedSize.width())).arg(QString::number(requestedSize.height()));

    {
        QMutexLocker m(&m_mutex);
        if (!m_ignoreCache && imageCache()->contains(hashKey)) {
            CacheImage i = imageCache()->value(hashKey);
            if (info.lastModified() <= i.time) {
                *size = i.image.size();
                return i.image;
            }
        }

        plugins = m_plugins;
    }

    foreach(ContentAdapterInterface* plugin, plugins) {
        if (plugin->canPreview(id)) {
            QImage img = plugin->preview(id, requestedSize);
            *size = img.size();
            CacheImage i;
            i.image = img;
            i.time = info.lastModified();
            imageCache()->insert(hashKey, i);

            return img;
        }
    }


    QImage img;
    QString type = info.suffix();
    QSize iconSize = requestedSize;
    if (!iconSize.isValid() || iconSize.isNull())
        iconSize = QSize(512, 512);

    if (iconCache()->contains(type)) {
        img = iconCache()->value(type);
        *size = img.size();
    } else if(m_engine){
        QMetaObject::invokeMethod(m_engine, "convertIconToImage",
                                  Qt::BlockingQueuedConnection,
                                  Q_RETURN_ARG(QImage, img),
                                  Q_ARG(QFileInfo, info),
                                  Q_ARG(QSize, iconSize));
        iconCache()->insert(type, img);
        *size = iconSize;
    }

    return img;
}

void PreviewImageProvider::setPlugins(QList<ContentAdapterInterface *> plugins)
{
    QMutexLocker m(&m_mutex);
    m_plugins = plugins;
}

void PreviewImageProvider::setIgnoreCache(bool enabled)
{
    QMutexLocker m(&m_mutex);
    m_ignoreCache = enabled;
}

bool PreviewImageProvider::ignoreCache()
{
    QMutexLocker m(&m_mutex);
    return m_ignoreCache;
}
