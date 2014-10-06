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

#include "directorypreviewadapter.h"
#include <QFileInfo>
#include <QDir>
#include <QQmlContext>
#include <QQmlEngine>

DirectoryPreviewAdapter::DirectoryPreviewAdapter(QObject *parent) :
    QObject(parent)
{
}

bool DirectoryPreviewAdapter::canPreview(const QString &path) const
{
    Q_UNUSED(path);

    return false;
}

QImage DirectoryPreviewAdapter::preview(const QString &path, const QSize &requestedSize)
{
    Q_UNUSED(path);
    Q_UNUSED(requestedSize);

    return QImage();
}


bool DirectoryPreviewAdapter::isFullScreen() const
{
    return true;
}

bool DirectoryPreviewAdapter::canAdapt(const QUrl &url) const
{
    QFileInfo info(url.toLocalFile());
    return info.isDir();
}
QUrl DirectoryPreviewAdapter::adapt(const QUrl &url, QQmlContext *context)
{
    QDir dir(url.toLocalFile());

    context->setContextProperty("files", dir.entryList(QDir::Files | QDir::NoDotDot | QDir::NoDot | QDir::NoSymLinks));
    context->setContextProperty("path", url.toLocalFile());
    context->setContextProperty("adapter", this);

    if (availableFeatures().testFlag(QtQuickControls))
        return QUrl("qrc:/livert/folderview_qt5_controls.qml");

    return QUrl("qrc:/livert/folderview_qt5.qml");
}

