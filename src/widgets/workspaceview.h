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

#include <QtGui>
#include <QtWidgets>

class FileSystemModel;

class WorkspaceView : public QWidget
{
    Q_OBJECT
public:
    explicit WorkspaceView(QWidget *parent = 0);
    QString activeDocument() const;
    QString rootPath() const;
    void setDirectoriesSelectable(bool enabled);
    bool directoriesSelectable() const;

public Q_SLOTS:
    void setRootPath(const QString& dirPath);
    void activateDocument(const QString& path);
    void activateRootPath();
    void goUp();

Q_SIGNALS:
    void pathActivated(const QString& path);

private Q_SLOTS:
    void indexActivated(const QModelIndex& index);

private:
    void selectIndex(const QModelIndex& index);
    QTreeView *m_view;
    FileSystemModel *m_model;
    QModelIndex m_rootIndex;
    QString m_currentDocument;
};
