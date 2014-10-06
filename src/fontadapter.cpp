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

#include "fontadapter.h"
#include <QDebug>
#include <QQmlContext>

FontAdapter::FontAdapter(QObject *parent) :
    QObject(parent),
    fontId(-1)
{
    fontExtensions.append(".ttf");
    fontExtensions.append(".otf");
}

void FontAdapter::cleanUp()
{
    base.removeAllApplicationFonts();
}

bool FontAdapter::canPreview(const QString &path) const
{
    Q_UNUSED(path);

    return false;
}

QImage FontAdapter::preview(const QString &path, const QSize &requestedSize)
{
    Q_UNUSED(path);
    Q_UNUSED(requestedSize);

    return QImage();
}

bool FontAdapter::canAdapt(const QUrl &url) const
{
    QString path = url.toLocalFile();

    foreach(const QString& extension, fontExtensions) {
        if (path.endsWith(extension))
            return true;
    }

    return false;
}

bool FontAdapter::isFullScreen() const
{
    return true;
}

QUrl FontAdapter::adapt(const QUrl &url, QQmlContext *context)
{
    fontId = base.addApplicationFont(url.toLocalFile());

    QStringList families = base.applicationFontFamilies(fontId);

    QVariantList styles;
    foreach (QString family, families) {
        QVariantMap style;
        QVariantList weights;
        foreach (QString styleName, base.styles(family)) {
            QVariantMap newWeight;
            int styleWeight = base.weight(family, styleName);

            newWeight.insert("name", styleName);
            newWeight.insert("weight", styleWeight);

            bool added = false;
            for (int i=0; i < weights.count(); i++) {
                if (styleWeight <= weights.at(i).toMap().value("weight").toInt()) {
                    added = true;
                    weights.insert(i, newWeight);
                    break;
                }
            }

            if (!added)
                weights.append(newWeight);
        }

        style.insert("family", family);
        style.insert("weights", weights);
        styles.append(style);
    }

    context->setContextProperty("styles", styles);

    if (availableFeatures().testFlag(QtQuickControls))
        return QUrl("qrc:/livert/fontviewer_qt5_controls.qml");

    return QUrl("qrc:/livert/fontviewer_qt5.qml");
}
