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
#include <QModelIndex>

namespace Ui {
class HostsOptionPage;
}

class Host;
class HostModel;
class QMenu;
class AutoDiscoveryHostsDialog;

class HostsOptionsPage : public QWidget
{
    Q_OBJECT

public:
    explicit HostsOptionsPage(QWidget *parent = 0);
    ~HostsOptionsPage();

    void setHostModel(HostModel* model);
    void setDiscoveredHostsModel(HostModel* model);

    void apply();

    void setHostSelected(Host* host);

private slots:
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);

    void updateName(const QString& name);
    void updateAddress(const QString& address);
    void updatePort(int port);
    void updateFollowTreeSelection(bool enabled);
    void updateXOffset(int offset);
    void updateYOffset(int offset);
    void updateRotation(int rotation);

    void addHost(Host* host = 0);
    void removeHost();

    void showAutoDiscoveryDialog();

private:
    Ui::HostsOptionPage *ui;

    int m_currentIndex;
    HostModel* m_model;
    AutoDiscoveryHostsDialog* m_autoDiscoveryDialog;
};

