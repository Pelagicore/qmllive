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

#include <QtCore>
#include <QtQml>
#include <QtQuick>

class QmlHelper : public QObject
{
    Q_OBJECT
public:
    explicit QmlHelper(QObject *parent = 0);

    static void loadDummyData(QQuickView* view, const QString& workspace);
    static void addImportPath(QQuickView* view, const QString& path);

Q_SIGNALS:

public Q_SLOTS:

};
