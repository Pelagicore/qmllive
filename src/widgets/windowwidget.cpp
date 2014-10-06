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

#include "windowwidget.h"
#include <QQuickView>
#include <QQuickItem>
#include <QGuiApplication>
#include <QDebug>
#include <QScrollBar>
#include <QResizeEvent>

WindowWidget::WindowWidget(QWidget *parent) :
    QAbstractScrollArea(parent), m_hostedWindow(0), m_resizing(false), m_centering(false)
{
    setFrameShape(QFrame::NoFrame);
}

void WindowWidget::setHostedWindow(QQuickView *hostedWindow)
{
    QQuickView* oldView = 0;
    if (m_hostedWindow) {
        m_hostedWindow->removeEventFilter(this);
        oldView = m_hostedWindow;
    }

    m_hostedWindow = hostedWindow;
    if (m_hostedWindow) {
        m_hostedWindow->installEventFilter(this);

        // force creation of the TL window in order to get its handle
        if (!testAttribute(Qt::WA_WState_Created))
            create(0, true, true);

        if (!windowHandle())
            qWarning("Could not get a valid windowhandle for our widget based window");

        m_hostedWindow->setFlags(Qt::Tool | Qt::FramelessWindowHint);
        m_hostedWindow->setParent(windowHandle());
        m_hostedWindow->setVisible(isVisible());

        // this needs to be done in order to prevent showing unitialized frames
        m_hostedWindow->setGeometry(QRect(viewport()->mapToGlobal(QPoint()), viewport()->size()));

        viewport()->setBackgroundRole(backgroundRole());
        viewport()->setPalette(palette());
        viewport()->setBackgroundRole(QPalette::Background);
        setFocusPolicy(Qt::StrongFocus);
        m_hostedWindow->show();
    }

    if (oldView) {
        oldView->hide();
        oldView->setParent(0);
    }
}

QQuickView *WindowWidget::hostedWindow() const
{
    return m_hostedWindow;
}

void WindowWidget::setVisible(bool visible)
{
    QAbstractScrollArea::setVisible(visible);
    if (m_hostedWindow)
        m_hostedWindow->setVisible(visible);
}

void WindowWidget::setCenteringEnabled(bool enabled)
{
    m_centering = enabled;
    updateScrollBars();
}

QSize WindowWidget::sizeHint() const
{
    QSize s = qmlSize();

    return s + QSize(frameWidth() * 2, frameWidth() * 2);
}

void WindowWidget::forceInitialResize()
{
    if (m_hostedWindow && m_hostedWindow->resizeMode() == QQuickView::SizeRootObjectToView) {
        m_hostedWindow->resize(size());
    }
    updateGeometry();
    updateScrollBars();
    emit widthChanged(width());
    emit heightChanged(height());

}

bool WindowWidget::event(QEvent *e)
{
    bool handled = false;

    if (m_hostedWindow) {
        switch (e->type()) {
        case QEvent::Wheel: {
            QWheelEvent *oe = static_cast<QWheelEvent *>(e);
            if (!viewport()->geometry().contains(oe->pos()))
                break;

            QWheelEvent ne(m_hostedWindow->mapFromGlobal(oe->globalPos()),
                           oe->globalPos(),
                           oe->pixelDelta(),
                           oe->angleDelta(),
                           oe->delta(),
                           oe->orientation(),
                           oe->buttons(),
                           oe->modifiers());

            qGuiApp->sendEvent(m_hostedWindow, &ne);
            handled = true;
            break;
        }
        case QEvent::KeyPress:
        case QEvent::KeyRelease: {
            qGuiApp->sendEvent(m_hostedWindow, e);
            handled = true;
            break;
        }
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove: {
            QMouseEvent *oe = static_cast<QMouseEvent *>(e);

            if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonDblClick) {
                if (!viewport()->geometry().contains(oe->pos()))
                    break;
            }

            QMouseEvent ne(oe->type(),
                           m_hostedWindow->mapFromGlobal(oe->globalPos()),
                           m_hostedWindow->mapFromGlobal(oe->globalPos()),
                           oe->globalPos(),
                           oe->button(),
                           oe->buttons(),
                           oe->modifiers());

            qGuiApp->sendEvent(m_hostedWindow, &ne);
            handled = true;
            break;
        }
        case QEvent::Resize: {
            QResizeEvent* re = static_cast<QResizeEvent*>(e);
            if (m_hostedWindow && m_hostedWindow->resizeMode() == QQuickView::SizeRootObjectToView) {
                m_hostedWindow->resize(re->size());
            }
            updateGeometry();
            updateScrollBars();
            if (re->oldSize().width() != re->size().width())
                emit widthChanged(re->size().width());
            if (re->oldSize().height() != re->size().height())
                emit heightChanged(re->size().height());
            break;
        }
        case QEvent::Hide:
            m_hostedWindow->close();
            break;
        default:
            break;
        }
    }
    return handled ? true : QAbstractScrollArea::event(e);
}

bool WindowWidget::eventFilter(QObject *o, QEvent *e)
{
    if (o && o == m_hostedWindow) {
        switch (e->type()) {
        case QEvent::Resize:
            updateScrollBars();
            break;
        default:
            break;
        }
    }
    return QAbstractScrollArea::eventFilter(o, e);
}

void WindowWidget::scrollContentsBy(int dx, int dy)
{
    Q_UNUSED(dx)
    Q_UNUSED(dy)

    updateWindowPosition();
}

#include <QStyle>

void WindowWidget::updateScrollBars()
{
    if (!m_hostedWindow)
        return;

    if (window() && !m_resizing) {
        m_resizing = true;

        QSize vpSize = viewport()->size();
        QSize vpMaxSize = maximumViewportSize();
        QSize wSize = qmlSize();

        // does the window fit without scrollbars?
        if (vpMaxSize.expandedTo(wSize) == vpMaxSize)
            vpSize = vpMaxSize;

        // fix scrollbars
        horizontalScrollBar()->setRange(0, (wSize - vpSize).width());
        horizontalScrollBar()->setPageStep(vpSize.width());
        verticalScrollBar()->setRange(0, (wSize - vpSize).height());
        verticalScrollBar()->setPageStep(vpSize.height());

        //qWarning() << "wSize:" << wSize << "vpSize:" << vpSize << "vpRect" << viewport()->rect();
        //qWarning() << "Hrange:" << (wSize - vpSize).width() << "Hstep:" << vpSize.width();

        updateWindowPosition();
        m_resizing = false;
    }
}
void WindowWidget::updateWindowPosition()
{
    if (m_hostedWindow && m_hostedWindow->rootObject()) {
        QSize wSize = qmlSize();
        Qt::LayoutDirection dir = layoutDirection();
        QRect scrolled = QStyle::visualRect(dir, viewport()->rect(), QRect(QPoint(-horizontalScrollBar()->value(), -verticalScrollBar()->value()), wSize));
        QRect aligned = QStyle::alignedRect(dir, m_centering ? Qt::AlignCenter : Qt::AlignTop | Qt::AlignLeft, wSize, viewport()->rect());

        //qWarning() << "xAlign:" << aligned.x() << "xScrolled:" << scrolled.x();
        m_hostedWindow->setPosition(wSize.width() < viewport()->width() ? aligned.x() : scrolled.x(),
                                    wSize.height() < viewport()->height() ? aligned.y() : scrolled.y());
    }
}

QSize WindowWidget::qmlSize() const
{
    QSize s;
    if (m_hostedWindow && m_hostedWindow->rootObject()) {
        s = QSize(m_hostedWindow->rootObject()->width(),
                  m_hostedWindow->rootObject()->height());
    }

    if (s.width() <= 20)
        s.setWidth(800);
    if (s.height() <= 20)
        s.setHeight(600);

    return s;
}

