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

#include <QtCore>

class Watcher;
class ContentPluginFactory;

class LiveHubEngine : public QObject
{
    Q_OBJECT
public:
    explicit LiveHubEngine(QObject *parent = 0);
    void setWorkspace(const QString& path);
    QString workspace() const;

    QString activePath() const;
public Q_SLOTS:
    void setActivePath(const QString& path);
    void setFilePublishingActive(bool on);
    void publishWorkspace();
Q_SIGNALS:
    void publishFile(const QString& document);
    void fileChanged(const QString& document);
    void activateDocument(const QString& document);
    void workspaceChanged(const QString& workspace);
private Q_SLOTS:
    void directoriesChanged(const QStringList& changes);
private:
    void publishDirectory(const QString& dirPath, bool fileChange);
private:
    Watcher *m_watcher;
    bool m_filePublishingActive;
    QString m_activePath;
};

