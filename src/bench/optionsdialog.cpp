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

#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "httpproxyoptionpage.h"
#include "importpathoptionpage.h"
#include "hostsoptionpage.h"

OptionsDialog::OptionsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptionsDialog)
    , m_httpProxyForm(new HttpProxyOptionPage(this))
    , m_importPathsForm(new ImportPathOptionPage(this))
    , m_hostsForm(new HostsOptionsPage(this))
{
    ui->setupUi(this);

    QListWidgetItem* item = new QListWidgetItem("HTTP Proxy");
    int index = ui->optionsStack->addWidget(m_httpProxyForm);
    item->setSelected(true);
    item->setData(Qt::UserRole, index);
    ui->optionsView->addItem(item);

    item = new QListWidgetItem("Import Paths");
    index = ui->optionsStack->addWidget(m_importPathsForm);
    item->setData(Qt::UserRole, index);
    ui->optionsView->addItem(item);

    item = new QListWidgetItem("Hosts");
    index = ui->optionsStack->addWidget(m_hostsForm);
    item->setData(Qt::UserRole, index);
    ui->optionsView->addItem(item);

    connect(ui->optionsView, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(optionSelected(QListWidgetItem*)));
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::setHostModel(HostModel *model)
{
    m_hostsForm->setHostModel(model);
}

void OptionsDialog::setDiscoveredHostsModel(HostModel *model)
{
    m_hostsForm->setDiscoveredHostsModel(model);
}

void OptionsDialog::openHostConfig(Host *host)
{
    ui->optionsView->setCurrentRow(2);
    m_hostsForm->setHostSelected(host);
}

void OptionsDialog::optionSelected(QListWidgetItem *current)
{
    int index = current->data(Qt::UserRole).toInt();
    ui->optionsStack->setCurrentIndex(index);
}

void OptionsDialog::accept()
{
    m_httpProxyForm->apply();
    m_importPathsForm->apply();
    m_hostsForm->apply();
    QDialog::accept();
}

void OptionsDialog::reject()
{
    QDialog::reject();
}


