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

#include "httpproxyoptionpage.h"
#include "ui_httpproxyoptionpage.h"
#include <QtNetwork>

HttpProxyOptionPage::HttpProxyOptionPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HttpProxyOptionPage)
{
    ui->setupUi(this);
    QSettings s;
    ui->proxyGroup->setChecked(s.value("http_proxy/enabled").toBool());
    ui->serverField->setText(s.value("http_proxy/hostname").toString());
    ui->portField->setText(s.value("http_proxy/port").toString());
    ui->userField->setText(s.value("http_proxy/username").toString());
    ui->passwordField->setText(s.value("http_proxy/password").toString());
}

HttpProxyOptionPage::~HttpProxyOptionPage()
{
    delete ui;
}

void HttpProxyOptionPage::apply()
{
    bool enabled = ui->proxyGroup->isChecked();
    QString hostname = ui->serverField->text();
    int port = ui->portField->text().toInt();
    QString username = ui->userField->text();
    QString password = ui->passwordField->text();
    if(enabled) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(hostname);
        proxy.setPort(port);
        proxy.setUser(username);
        proxy.setPassword(password);
        QNetworkProxy::setApplicationProxy(proxy);
    } else {
        QNetworkProxy::setApplicationProxy(QNetworkProxy());
    }
    QSettings s;
    s.setValue("http_proxy/enabled", enabled);
    s.setValue("http_proxy/hostname", hostname);
    s.setValue("http_proxy/port", port);
    s.setValue("http_proxy/username", username);
    s.setValue("http_proxy/password", password);

}
