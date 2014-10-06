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

#include "allhostswidget.h"

#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QGridLayout>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QMessageBox>

AllHostsWidget::AllHostsWidget(QWidget *parent) :
    QWidget(parent)
{
    setAcceptDrops(true);

    setFixedHeight(155);

    QGridLayout* layout = new QGridLayout(this);
    m_groupBox = new QGroupBox("All Hosts");
    layout->addWidget(m_groupBox);

    m_menuButton = new QPushButton("...", m_groupBox);
    m_menuButton->setMaximumWidth(30);
    m_menuButton->setCheckable(true);
    connect(m_menuButton, SIGNAL(clicked()), this, SLOT(showMenu()));

    QVBoxLayout* hbox = new QVBoxLayout(m_groupBox);
    hbox->addWidget(new QLabel("Drop File"));
    hbox->addWidget(m_menuButton);

    m_menu = new QMenu(this);
    m_publishAction = m_menu->addAction("Publish All", this, SLOT(onPublishTriggered()));
    connect(m_menu, SIGNAL(aboutToHide()), m_menuButton, SLOT(toggle()));

    m_refreshAction = m_menu->addAction("Refresh All", this, SIGNAL(refreshAll()));
}

void AllHostsWidget::setWorkspace(const QString &workspace)
{
    m_workspace.setPath(workspace);
}

void AllHostsWidget::showMenu()
{
    QPoint p = mapToGlobal(m_menuButton->pos());
    p.ry() += m_menuButton->height() + 5;
    p.rx() += 5;
    m_menu->exec(p);
}

void AllHostsWidget::onPublishTriggered()
{
    if (QMessageBox::question(this, QString("Publish %1").arg(m_workspace.path()),
                              QString("Do you really want to publish the content of %1").arg(m_workspace.path())) == QMessageBox::Yes) {
        emit publishAll();
    }
}

void AllHostsWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void AllHostsWidget::dropEvent(QDropEvent *event)
{
    QUrl url(event->mimeData()->text());

    if (url.isLocalFile())
        emit currentFileChanged(url.toLocalFile());
    event->acceptProposedAction();
}
