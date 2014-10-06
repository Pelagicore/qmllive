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

#include "autodiscoveryhostsdialog.h"
#include "ui_autodiscoveryhostsdialog.h"
#include "hostmodel.h"

AutoDiscoveryHostsDialog::AutoDiscoveryHostsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoDiscoveryHostsDialog),
    m_model(0)
{
    ui->setupUi(this);
}

AutoDiscoveryHostsDialog::~AutoDiscoveryHostsDialog()
{
    delete ui;
}

QList<QPointer<Host> > AutoDiscoveryHostsDialog::selectedHosts() const
{
    QList<QPointer<Host> > hostList;

    QModelIndexList selectedRows = ui->hostView->selectionModel()->selectedRows();
    foreach(QModelIndex index, selectedRows) {
        Host* host = m_model->hostAt(index.row());
        hostList.append(QPointer<Host>(host));
    }

    return hostList;
}

void AutoDiscoveryHostsDialog::clearSelection()
{
    ui->hostView->clearSelection();
}

void AutoDiscoveryHostsDialog::setDiscoveredHostsModel(HostModel *model)
{
    m_model = model;

    ui->hostView->setModel(model);
    ui->hostView->hideColumn(0);
}
