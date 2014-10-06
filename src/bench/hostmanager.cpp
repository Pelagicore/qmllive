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

#include "hostmanager.h"
#include "hostmodel.h"
#include "hostwidget.h"
#include "dummydelegate.h"
#include "livehubengine.h"
#include "logreceiver.h"
#include "widgets/logview.h"
#include <QDockWidget>

#include <QDebug>

HostManager::HostManager(QWidget *parent) :
    QListView(parent)
{
    setItemDelegate(new DummyDelegate(this));
    setFlow(QListView::LeftToRight);
    setUniformItemSizes(true);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setDragEnabled(true);
    setDefaultDropAction(Qt::MoveAction);
    setDragDropMode(QAbstractItemView::InternalMove);
    viewport()->setAcceptDrops(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    viewport()->setAutoFillBackground(false);
}

void HostManager::setModel(HostModel *model)
{
    m_model = model;
    QListView::setModel(model);

    connect(model, SIGNAL(modelReset()), this, SLOT(modelReseted()));
}

void HostManager::setLiveHubEngine(LiveHubEngine *engine)
{
    m_engine = engine;

    for (int i=0; i < m_model->rowCount(); i++) {
        HostWidget* widget = qobject_cast<HostWidget*>(indexWidget(m_model->index(i, 0)));
        if(widget)
            widget->setLiveHubEngine(engine);
    }
}

void HostManager::followTreeSelection(const QString &currentFile)
{
    for (int i=0; i < m_model->rowCount(); i++) {
        HostWidget* widget = qobject_cast<HostWidget*>(indexWidget(m_model->index(i, 0)));
        if(widget && widget->followTreeSelection())
            widget->setCurrentFile(currentFile);
    }
}

void HostManager::setCurrentFile(const QString &currentFile)
{
    for (int i=0; i < m_model->rowCount(); i++) {
        HostWidget* widget = qobject_cast<HostWidget*>(indexWidget(m_model->index(i, 0)));
        if(widget)
            widget->setCurrentFile(currentFile);
    }
}

void HostManager::publishAll()
{
    for (int i=0; i < m_model->rowCount(); i++) {
        HostWidget* widget = qobject_cast<HostWidget*>(indexWidget(m_model->index(i, 0)));
        if(widget)
            widget->publishWorkspace();
    }
}

void HostManager::refreshAll()
{
    for (int i=0; i < m_model->rowCount(); i++) {
        HostWidget* widget = qobject_cast<HostWidget*>(indexWidget(m_model->index(i, 0)));
        if(widget)
            widget->refresh();
    }
}

void HostManager::rowsInserted(const QModelIndex &parent, int start, int end)
{
    if (parent.isValid())
        return;

    for (int i=start; i<= end; i++) {
        addHost(i);
    }
}

void HostManager::addHost(int index)
{
    HostWidget* widget = new HostWidget();
    Host* host = m_model->hostAt(index);
    widget->setLiveHubEngine(m_engine.data());
    widget->setHost(host);
    setIndexWidget(m_model->index(index,0), widget);
    connect(widget, SIGNAL(openHostConfig(Host*)), this, SIGNAL(openHostConfig(Host*)));

    QDockWidget* dock = new QDockWidget(host->name());
    dock->setObjectName(host->name() + "LogDock");
    connect(host, SIGNAL(nameChanged(QString)), dock, SLOT(setWindowTitle(QString)));
    LogView* view = new LogView(false, dock);
    connect(widget, SIGNAL(remoteLog(int,QString,QUrl,int,int)), view, SLOT(appendToLog(int,QString,QUrl,int,int)));
    dock->setWidget(view);
    m_logList.append(dock);
    emit logWidgetAdded(dock);
}

void HostManager::rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end)
{
    if (parent.isValid())
        return;

    for (int i=start; i<= end; i++) {
        QDockWidget* dock = m_logList.takeAt(i);
        delete dock;
    }
}

void HostManager::modelReseted()
{
    for (int i=0; i < m_model->rowCount(); i++) {
        addHost(i);
    }
}
