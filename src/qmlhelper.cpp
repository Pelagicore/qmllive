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

#include "qmlhelper.h"


/*!
 * \class QmlHelper
 * \brief Provides a set of helper functions to setup your qml viewer
 * \group qmllive
 */

/*!
 * \brief Standard constructor using \a parent as parent
 */
QmlHelper::QmlHelper(QObject *parent) :
    QObject(parent)
{
}

/*!
 * Loads dummy data from a "dummydata" folder in the workspace folder
 * \a view defines the View where you want to export the dummy data to
 * The "dummydata" will be searched in the "dummydata" sub directory of \a workspace
 */
void QmlHelper::loadDummyData(QQuickView *view, const QString &workspace)
{
    Q_ASSERT(view);
    QDir dir(workspace + "/dummydata", "*.qml");
    foreach(QString entry, dir.entryList()) {

        QQmlComponent comp(view->engine(), dir.filePath(entry));
        QObject* obj = comp.create();

        if(comp.isError()) {
            foreach(const QQmlError error, comp.errors()) {
                qWarning() << error;
            }
        }
        if(obj) {
            qWarning() << "loaded dummy data: " << dir.filePath(entry);
            entry.chop(4);
            view->rootContext()->setContextProperty(entry, obj);
            obj->setParent(view);
        }
    }
}

/*!
 * \fn void QmlHelper::addImportPath(QQuickView *view, const QString &path)
 * Adds an import path \a path to your qml viewer \a view
 */
void QmlHelper::addImportPath(QQuickView *view, const QString &path)
{
    Q_ASSERT(view);
    view->engine()->addImportPath(path);
}




