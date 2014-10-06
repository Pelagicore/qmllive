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
#include <QtGui>

class LiveRuntime : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal screenWidth READ screenWidth WRITE setScreenWidth NOTIFY screenWidthChanged)
    Q_PROPERTY(qreal screenHeight READ screenHeight WRITE setScreenHeight NOTIFY screenHeightChanged)


public:
    explicit LiveRuntime(QObject *parent = 0);
    qreal screenWidth() const;
    qreal screenHeight() const;

public slots:
    void setScreenWidth(qreal arg);
    void setScreenHeight(qreal arg);

signals:
    void screenWidthChanged(qreal arg);
    void screenHeightChanged(qreal arg);

private:
    qreal m_screenWidth;
    qreal m_screenHeight;
};

