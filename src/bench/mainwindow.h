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
#include <QtWidgets>
#include <QtQuick>


class BenchQuickView;
class WorkspaceView;
class LogView;
class LiveRuntime;
class LiveHubEngine;
class BenchLiveNodeEngine;
class WindowWidget;
class QToolBar;
class HostModel;
class HostManager;
class AllHostsWidget;
class Host;
class HostDiscoveryManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void activateDocument(const QString path);
    void setWorkspace(const QString& path);
    void setPluginPath(const QString& path);
    void setImportPaths(const QStringList& pathList);
    void setStaysOnTop(bool enabled);
    void readSettings();
protected:
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);
private:
    void setupContent();
    void setupWorkspaceView();
    void setupHostView();
    void setupLogView();
    void setupToolBar();
    void setupMenuBar();
    void writeSettings();
    void resetImportPaths();
private slots:
    void resizeToFit();
    void takeSnapshot();
    void slowDownAnimations(bool enable);
    void openWorkspace();
    void logQuitEvent();
    void updateWindowTitle();
    void openPreferences(Host *host = 0);
    void openRecentFolder();
    void clearRecentFolder();
    void stayOnTop();

    void initView(BenchQuickView *view);

    void onLogWidgetAdded(QDockWidget* logDock);

private:
    void updateRecentFolder(const QString &path = QString());
    BenchQuickView *m_qmlview;
    WindowWidget *m_ww;
    WorkspaceView *m_workspace;
    LogView *m_log;
    QUrl m_currentSource;
    QDockWidget *m_logDock;
    QDockWidget *m_workspaceDock;
    QDockWidget *m_hostDock;
    HostManager *m_hostManager;
    HostModel *m_hostModel;
    HostDiscoveryManager *m_discoveryManager;
    AllHostsWidget* m_allHosts;
    LiveHubEngine *m_hub;
    BenchLiveNodeEngine *m_node;
    QStringList m_recentFolder;
    QMenu* m_recentMenu;
    QMenu* m_logDockMenu;
    QAction *m_stayOnTop;
    QAction *m_openWorkspace;
    QAction *m_refresh;
    QAction *m_resizeFit;
    QAction *m_enablePluginReload;
    QAction *m_clipRootObject;
    QToolBar* m_toolBar;
    QStringList m_qmlDefaultimportList;
};
