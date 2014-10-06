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

#include "importpathoptionpage.h"
#include "ui_importpathoptionpage.h"

ImportPathOptionPage::ImportPathOptionPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImportPathOptionPage)
{
    ui->setupUi(this);
    QSettings s;
    int count = s.beginReadArray("imports");
    QListWidgetItem* item;
    for(int i=0; i<count; i++) {
        s.setArrayIndex(i);
        item = new QListWidgetItem(s.value("path").toString());
        item->setFlags(item->flags () | Qt::ItemIsEditable);
        ui->importList->addItem(item);
    }
    s.endArray();

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addItem()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeItem()));
    connect(ui->editButton, SIGNAL(clicked()), this, SLOT(editItem()));
}

ImportPathOptionPage::~ImportPathOptionPage()
{
    delete ui;
}

void ImportPathOptionPage::apply()
{
    QStringList paths;
    QSettings s;
    s.beginWriteArray("imports");
    for(int i=0; i<ui->importList->count(); i++) {
        QString path(ui->importList->item(i)->text());
        paths << path;
        s.setArrayIndex(i);
        s.setValue("path", path);
    }
    s.endArray();
}

void ImportPathOptionPage::addItem()
{
    QString path = QFileDialog::getExistingDirectory(this, "Add Import Path");
    if(path.isEmpty()) {
        return;
    }
    QListWidgetItem* item = new QListWidgetItem(path);
    item->setFlags(item->flags () | Qt::ItemIsEditable);
    ui->importList->addItem(item);
}

void ImportPathOptionPage::removeItem()
{
    QListWidgetItem *item = ui->importList->currentItem();
    if(item) {
        delete item;
    }
}

void ImportPathOptionPage::editItem()
{
    QListWidgetItem *item = ui->importList->currentItem();
    if(item) {
        ui->importList->editItem(item);
    }
}
