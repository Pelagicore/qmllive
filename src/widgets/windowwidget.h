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

#include <QAbstractScrollArea>
#include <QPointer>

class QQuickView;

class WindowWidget : public QAbstractScrollArea
{
    Q_OBJECT
public:
    explicit WindowWidget(QWidget *parent = 0);

    QQuickView *hostedWindow() const;
    void setHostedWindow(QQuickView *hostedWindow);
    void setVisible(bool visible);
    void setCenteringEnabled(bool enabled);
    QSize sizeHint() const;
    void forceInitialResize();

Q_SIGNALS:
    void widthChanged(int w);
    void heightChanged(int w);

protected:
    bool event(QEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    void scrollContentsBy(int dx, int dy);

private:
    void updateScrollBars();
    void updateWindowPosition();
    QSize qmlSize() const;

    QPointer<QQuickView> m_hostedWindow;
    bool m_resizing;
    bool m_centering;
};

