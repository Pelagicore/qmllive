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

#include "hostsoptionpage.h"
#include "ui_hostsoptionpage.h"
#include "hostmodel.h"
#include "autodiscoveryhostsdialog.h"

#include <QMenu>
#include <QDebug>

HostsOptionsPage::HostsOptionsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HostsOptionPage),
    m_currentIndex(-1),
    m_model(0),
    m_autoDiscoveryDialog(new AutoDiscoveryHostsDialog(this))
{
    ui->setupUi(this);

    ui->hostUI->setVisible(false);

    connect(ui->nameField, SIGNAL(textEdited(QString)), this, SLOT(updateName(QString)));
    connect(ui->ipField, SIGNAL(textEdited(QString)), this, SLOT(updateAddress(QString)));
    connect(ui->portField, SIGNAL(valueChanged(int)), this, SLOT(updatePort(int)));
    connect(ui->followTreeSelectionField, SIGNAL(clicked(bool)), this, SLOT(updateFollowTreeSelection(bool)));
    connect(ui->xField, SIGNAL(valueChanged(int)), this, SLOT(updateXOffset(int)));
    connect(ui->yField, SIGNAL(valueChanged(int)), this, SLOT(updateYOffset(int)));
    connect(ui->rotationField, SIGNAL(valueChanged(int)), this, SLOT(updateRotation(int)));

    QMenu* menu = new QMenu(ui->addHostButton);
    menu->addAction("Auto Discovery...", this, SLOT(showAutoDiscoveryDialog()));
    menu->addAction("Manual", this, SLOT(addHost()));
    ui->addHostButton->setMenu(menu);

    connect(ui->removeHostButton, SIGNAL(clicked()), this, SLOT(removeHost()));
}

HostsOptionsPage::~HostsOptionsPage()
{
    delete ui;
}

void HostsOptionsPage::setHostModel(HostModel *model)
{
    m_model = model;
    m_currentIndex = -1;

    connect(ui->hostsWidget->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(onCurrentRowChanged(QModelIndex,QModelIndex)));

    for(int i=0; i< m_model->rowCount(); i++) {
        Host* host = m_model->hostAt(i);
        addHost(host);
    }
}

void HostsOptionsPage::setDiscoveredHostsModel(HostModel *model)
{
    m_autoDiscoveryDialog->setDiscoveredHostsModel(model);
}

void HostsOptionsPage::apply()
{
    for(int i=0; i < ui->hostsWidget->rowCount(); i++) {
        QTableWidgetItem* item = ui->hostsWidget->item(i, 0);
        Host* host = item->data(Qt::UserRole).value<Host*>();
        if (ui->hostsWidget->isRowHidden(i)) {
            if (host)
                m_model->removeHost(host);
            continue;
        }

        host->setName(item->text());
        host->setAddress(item->data(Qt::UserRole + 1).toString());
        host->setPort(item->data(Qt::UserRole + 2).toInt());
        host->setFollowTreeSelection(item->data(Qt::UserRole + 4).toBool());
        host->setXOffset(item->data(Qt::UserRole + 5).toInt());
        host->setYOffset(item->data(Qt::UserRole + 6).toInt());
        host->setRotation(item->data(Qt::UserRole + 7).toInt());

        if (m_model->indexOf(host) == -1)
            m_model->addHost(host);
    }

    m_currentIndex = -1;
}

void HostsOptionsPage::setHostSelected(Host *host)
{
    ui->hostsWidget->selectRow(m_model->indexOf(host));
}

void HostsOptionsPage::onCurrentRowChanged ( const QModelIndex & current, const QModelIndex & previous )
{
    Q_UNUSED(previous);

    if (!current.isValid()) {
        ui->hostUI->setVisible(false);
        m_currentIndex = -1;
    }

    ui->removeHostButton->setEnabled(true);

    m_currentIndex = current.row();

    QTableWidgetItem* typeItem = ui->hostsWidget->item(m_currentIndex, 1);

    QString type = typeItem->text();
    bool autoDiscovery = (type == "Auto");

    QTableWidgetItem* item = ui->hostsWidget->item(m_currentIndex, 0);

    ui->nameField->setText(item->text());
    ui->typeField->setText(type);

    ui->ipField->setText(item->data(Qt::UserRole + 1).toString());
    ui->portField->setValue(item->data(Qt::UserRole + 2).toInt());
    ui->ipLabel->setDisabled(autoDiscovery);
    ui->ipField->setDisabled(autoDiscovery);
    ui->portLabel->setDisabled(autoDiscovery);
    ui->portField->setDisabled(autoDiscovery);

    ui->followTreeSelectionField->setChecked(item->data(Qt::UserRole + 4).toBool());

    ui->xField->setValue(item->data(Qt::UserRole + 5).toInt());
    ui->yField->setValue(item->data(Qt::UserRole + 6).toInt());
    ui->rotationField->setValue(item->data(Qt::UserRole + 7).toInt());

    ui->hostUI->setVisible(true);
}

void HostsOptionsPage::updateName(const QString &name)
{
    Q_ASSERT(m_currentIndex != -1);

    ui->hostsWidget->item(m_currentIndex, 0)->setText(name);
}

void HostsOptionsPage::updateAddress(const QString &address)
{
    Q_ASSERT(m_currentIndex != -1);

    QTableWidgetItem* item = ui->hostsWidget->item(m_currentIndex, 0);

    ui->hostsWidget->item(m_currentIndex, 2)->setText(QString("%1:%2").arg(address).arg(item->data(Qt::UserRole + 2).toInt()));

    item->setData(Qt::UserRole + 1, address);
}

void HostsOptionsPage::updatePort(int port)
{
    Q_ASSERT(m_currentIndex != -1);

    QTableWidgetItem* item = ui->hostsWidget->item(m_currentIndex, 0);

    ui->hostsWidget->item(m_currentIndex, 2)->setText(QString("%1:%2").arg(item->data(Qt::UserRole + 1).toString()).arg(port));

    item->setData(Qt::UserRole + 2, port);
}

void HostsOptionsPage::updateFollowTreeSelection(bool enabled)
{
    Q_ASSERT(m_currentIndex != -1);

    QTableWidgetItem* item = ui->hostsWidget->item(m_currentIndex, 0);

    item->setData(Qt::UserRole + 4, enabled);
}

void HostsOptionsPage::updateXOffset(int offset)
{
    Q_ASSERT(m_currentIndex != -1);

    QTableWidgetItem* item = ui->hostsWidget->item(m_currentIndex, 0);

    item->setData(Qt::UserRole + 5, offset);
}

void HostsOptionsPage::updateYOffset(int offset)
{
    Q_ASSERT(m_currentIndex != -1);

    QTableWidgetItem* item = ui->hostsWidget->item(m_currentIndex, 0);

    item->setData(Qt::UserRole + 6, offset);
}

void HostsOptionsPage::updateRotation(int rotation)
{
    Q_ASSERT(m_currentIndex != -1);

    QTableWidgetItem* item = ui->hostsWidget->item(m_currentIndex, 0);

    item->setData(Qt::UserRole + 7, rotation);
}

void HostsOptionsPage::addHost(Host *host)
{
    if (!host)
        host = new Host(Host::Manual, m_model);

    int count = ui->hostsWidget->rowCount();
    ui->hostsWidget->setRowCount(count + 1);

    QTableWidgetItem* item = new QTableWidgetItem(host->name());
    ui->hostsWidget->setItem(count, 0, item);

    item->setData(Qt::UserRole, QVariant::fromValue(host));
    item->setData(Qt::UserRole + 1, host->address());
    item->setData(Qt::UserRole + 2, host->port());
    item->setData(Qt::UserRole + 4, host->followTreeSelection());
    item->setData(Qt::UserRole + 5, host->xOffset());
    item->setData(Qt::UserRole + 6, host->yOffset());
    item->setData(Qt::UserRole + 7, host->rotation());

    ui->hostsWidget->setItem(count, 1, new QTableWidgetItem(host->type() == Host::AutoDiscovery ? "Auto" : "Manual"));
    ui->hostsWidget->setItem(count, 2, new QTableWidgetItem(QString("%1:%2").arg(host->address()).arg(host->port())));
}

void HostsOptionsPage::removeHost()
{
    if (m_currentIndex == -1)
        return;

    ui->hostsWidget->hideRow(m_currentIndex);

    ui->hostsWidget->clearSelection();
    ui->hostUI->setVisible(false);
    ui->removeHostButton->setEnabled(false);
    m_currentIndex = -1;
}

void HostsOptionsPage::showAutoDiscoveryDialog()
{
    m_autoDiscoveryDialog->clearSelection();
    if (m_autoDiscoveryDialog->exec()) {
        QList<QPointer<Host> > hostList = m_autoDiscoveryDialog->selectedHosts();
        foreach(QPointer<Host> host, hostList) {
            if (!host)
                continue;

            addHost(new Host(*host, m_model));
        }
    }
}
