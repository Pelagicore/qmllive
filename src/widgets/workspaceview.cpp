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

#include "workspaceview.h"
#include "filesystemmodel.h"
#include "workspacedelegate.h"

/*!
  \class WorkspaceView
  \internal
  \brief A TreeView showing the Local Filesystem.

  The activateDocument() signal can be used to connect to a LiveHubEngine.
 */


/*!
 Standard constructor using \a parent as parent
 */
WorkspaceView::WorkspaceView(QWidget *parent)
    : QWidget(parent)
    , m_view(new QTreeView(this))
    , m_model(new FileSystemModel(this))
{
    // setup view
//    m_view->setFocusPolicy(Qt::NoFocus);
    m_view->setModel(m_model);
    m_view->hideColumn(1); // size
    m_view->hideColumn(2); // type
    m_view->hideColumn(3); // modified time

    m_view->setItemDelegate(new WorkspaceDelegate(m_model, this));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(indexActivated(QModelIndex)));

    m_model->setAllowedTypesFilter(QStringList() << "*.qml" << "*.png" << "*.otf" << "*.ttf");

    // setup layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_view);
    layout->setMargin(1);
    setLayout(layout);


    m_view->setDragEnabled(true);
    m_view->setDragDropMode(QAbstractItemView::DragOnly);
}

/*!
 * Sets the root workspace path to be displayed in the view to \a dirPath
 */
void WorkspaceView::setRootPath(const QString &dirPath)
{
    m_rootIndex = m_model->setRootPath(dirPath);
    m_view->setRootIndex(m_model->index(dirPath));
}

/*!
 * Activates the document by the given \a path
 */
void WorkspaceView::activateDocument(const QString &path)
{
    //qDebug() << "WorkspaceView::activateDocument" << path;
    QModelIndex index = m_model->index(path);
    selectIndex(index);
}

void WorkspaceView::activateRootPath()
{
    selectIndex(m_rootIndex);
    emit pathActivated(m_model->rootPath());
}

void WorkspaceView::goUp()
{
    QModelIndex index = m_view->currentIndex().parent();
    if (!index.isValid() || index == m_rootIndex)
        return;

    selectIndex(index);
}

/*!
 * Returns the active, selected document.
 */
QString WorkspaceView::activeDocument() const
{
    return m_currentDocument;
}

QString WorkspaceView::rootPath() const
{
    return m_model->rootPath();
}

void WorkspaceView::setDirectoriesSelectable(bool enabled)
{
    m_model->setDirectoriesSelectable(enabled);
}

bool WorkspaceView::directoriesSelectable() const
{
    return m_model->directoriesSelectable();
}

/*!
 * Emits a document activated signal based on a given \a index
 */
void WorkspaceView::indexActivated(const QModelIndex &index)
{
    if (!(m_model->flags(index) & Qt::ItemIsSelectable))
        return;

    QString path = m_model->filePath(index);

    m_currentDocument = path;
    emit pathActivated(path);
}

void WorkspaceView::selectIndex(const QModelIndex &index)
{
    QModelIndex parentIndex = index.parent();
    while(parentIndex.isValid()) {
        m_view->expand(parentIndex);
        parentIndex = parentIndex.parent();
    }
    m_view->setCurrentIndex(index);
    indexActivated(index);
    //m_view->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}
