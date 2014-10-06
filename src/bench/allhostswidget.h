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

#include <QWidget>
#include <QDir>

class QGroupBox;
class QPushButton;
class QMenu;
class AllHostsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AllHostsWidget(QWidget *parent = 0);

    void setWorkspace(const QString& workspace);

signals:
    void refreshAll();
    void publishAll();
    void currentFileChanged(const QString file);

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
private slots:
    void showMenu();
    void onPublishTriggered();
private:
    QDir m_workspace;
    QGroupBox* m_groupBox;
    QPushButton* m_menuButton;
    QMenu* m_menu;
    QAction* m_publishAction;
    QAction* m_refreshAction;
};

