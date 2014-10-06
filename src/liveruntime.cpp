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

#include "liveruntime.h"

// TODO: create a variant model for dynamic passing of key value pairs with notify
// TODO: create support for background and overlay layer image files livert.background, livert.overlay

/*!
 * \class LiveRuntime
 * \brief Collects properties to be used for an enhanced live runtime.
 * \group qmllive
 *
 * This runtime is used in an live enhanced qml project to be able to access more
 * advanced features. Currently it does nothing
 */

/*!
 * Standard constructor using \a parent as parent
 */
LiveRuntime::LiveRuntime(QObject *parent) :
    QObject(parent)
{
}

void LiveRuntime::setScreenHeight(qreal arg)
{
    if (m_screenHeight != arg) {
        m_screenHeight = arg;
        emit screenHeightChanged(arg);
    }
}

qreal LiveRuntime::screenWidth() const
{
    return m_screenWidth;
}

qreal LiveRuntime::screenHeight() const
{
    return m_screenHeight;
}

void LiveRuntime::setScreenWidth(qreal arg)
{
    if (m_screenWidth != arg) {
        m_screenWidth = arg;
        emit screenWidthChanged(arg);
    }
}
