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

#include <QListView>
#include <QPointer>

class LiveHubEngine;
class HostModel;

class QDockWidget;
class Host;
class HostManager : public QListView
{
    Q_OBJECT
public:
    explicit HostManager(QWidget *parent = 0);

    void setModel(HostModel* model);
    void setLiveHubEngine(LiveHubEngine* engine);

signals:
    void logWidgetAdded(QDockWidget* log);
    void openHostConfig(Host* host);

public slots:
    void followTreeSelection(const QString& currentFile);
    void setCurrentFile(const QString& currentFile);
    void publishAll();
    void refreshAll();

private slots:
    void rowsInserted(const QModelIndex& parent, int start, int end);\
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void modelReseted();
    void addHost(int index);

private:
    QPointer<LiveHubEngine> m_engine;

    HostModel* m_model;
    QList<QDockWidget*> m_logList;
};

