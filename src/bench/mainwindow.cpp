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

#include "mainwindow.h"
#include "widgets/windowwidget.h"

#include <QToolBar>
#include <QtNetwork>
#include <private/qabstractanimation_p.h>

#include "widgets/workspaceview.h"
#include "widgets/logview.h"
#include "livehubengine.h"
#include "benchlivenodeengine.h"
#include "qmlhelper.h"
#include "optionsdialog.h"
#include "benchquickview.h"
#include "hostmodel.h"
#include "hostmanager.h"
#include "allhostswidget.h"
#include "hostdiscoverymanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_qmlview(0)
    , m_workspace(new WorkspaceView())
    , m_log(new LogView(true, this))
    , m_hostManager(new HostManager(this))
    , m_hostModel(new HostModel(this))
    , m_discoveryManager(new HostDiscoveryManager(this))
    , m_allHosts(new AllHostsWidget(this))
    , m_hub(new LiveHubEngine(this))
    , m_node(new BenchLiveNodeEngine(this))
{
    setupContent();
    setupMenuBar();
    setupToolBar();

    m_discoveryManager->setKnownHostsModel(m_hostModel);\

    m_hostManager->setModel(m_hostModel);
    m_hostManager->setLiveHubEngine(m_hub);

    setWindowIcon(QIcon("://images/favicon.png"));

    m_hub->setFilePublishingActive(true);
    m_node->setWorkspaceView(m_workspace);

    connect(m_workspace, SIGNAL(pathActivated(QString)), m_hub, SLOT(setActivePath(QString)));
    connect(m_workspace, SIGNAL(pathActivated(QString)), m_hostManager, SLOT(followTreeSelection(QString)));
    connect(m_hub, SIGNAL(activateDocument(QString)), this, SLOT(updateWindowTitle()));
    connect(m_hub, SIGNAL(activateDocument(QString)), m_node, SLOT(setActiveDocument(QString)));
    connect(m_node, SIGNAL(viewChanged(BenchQuickView*)), this, SLOT(initView(BenchQuickView*)));
    connect(m_allHosts, SIGNAL(publishAll()), m_hostManager, SLOT(publishAll()));
    connect(m_allHosts, SIGNAL(currentFileChanged(QString)), m_hostManager, SLOT(setCurrentFile(QString)));
    connect(m_allHosts, SIGNAL(refreshAll()), m_hostManager, SLOT(refreshAll()));
    connect(m_hostManager, SIGNAL(logWidgetAdded(QDockWidget*)), this, SLOT(onLogWidgetAdded(QDockWidget*)));
    connect(m_hostManager, SIGNAL(openHostConfig(Host*)), this, SLOT(openPreferences(Host*)));

    BenchQuickView view;
    m_qmlDefaultimportList = view.engine()->importPathList();
}

MainWindow::~MainWindow()
{
}


void MainWindow::setupContent()
{
    setupLogView();
    setupWorkspaceView();
    setupHostView();

    m_ww = new WindowWidget(this);
    m_ww->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_ww->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_ww->setBackgroundRole(QPalette::Dark);
    m_node->setWindowWidget(m_ww);

    setCentralWidget(m_ww);
}

void MainWindow::initView(BenchQuickView *view)
{
    m_qmlview = view;

    m_ww->setCenteringEnabled(true);
    m_ww->setHostedWindow(m_qmlview);

    connect(m_qmlview->engine(), SIGNAL(quit()), this, SLOT(logQuitEvent()));
}

void MainWindow::onLogWidgetAdded(QDockWidget *logDock)
{
    m_logDockMenu->addAction(logDock->toggleViewAction());

    logDock->setFeatures(m_logDock->features());
    logDock->setAllowedAreas(m_logDock->allowedAreas());
    addDockWidget(dockWidgetArea(m_logDock), logDock);
    tabifyDockWidget(m_logDock, logDock);
}

void MainWindow::setupWorkspaceView()
{
    m_workspaceDock = new QDockWidget("Workspace", this);
    m_workspaceDock->setObjectName("workspace");
    m_workspaceDock->setWidget(m_workspace);
    m_workspaceDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_workspaceDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    addDockWidget(Qt::LeftDockWidgetArea, m_workspaceDock);
}

void MainWindow::setupHostView()
{
    m_hostDock = new QDockWidget("Hosts", this);
    m_hostDock->setObjectName("hosts");
    m_hostDock->setFeatures(QDockWidget::AllDockWidgetFeatures);

    QFrame* hostContainer = new QFrame();
    hostContainer->setFrameStyle(QFrame::StyledPanel);
    QHBoxLayout* layout = new QHBoxLayout(hostContainer);
    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(m_allHosts);
    vbox->addStretch(1);
    layout->addLayout(vbox);
    layout->addWidget(m_hostManager);
    layout->setContentsMargins(0,0,0,0);

    m_hostDock->setWidget(hostContainer);
    addDockWidget(Qt::TopDockWidgetArea, m_hostDock);
}

void MainWindow::setupLogView()
{
    m_logDock = new QDockWidget("Log Output", this);
    m_logDock->setObjectName("log");
    m_logDock->setWidget(m_log);
    m_logDock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    m_logDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    addDockWidget(Qt::BottomDockWidgetArea, m_logDock);

    connect(m_node, SIGNAL(logClear()), m_log, SLOT(clear()));
    connect(m_node, SIGNAL(logIgnoreMessages(bool)), m_log, SLOT(setIgnoreMessages(bool)));
    connect(m_node, SIGNAL(logErrors(QList<QQmlError>)), m_log, SLOT(appendToLog(QList<QQmlError>)));
}

void MainWindow::setupMenuBar()
{
    QMenu *file = menuBar()->addMenu(tr("&File"));
    m_openWorkspace = file->addAction(QIcon::fromTheme("folder-open"), tr("&Open Workspace..."), this, SLOT(openWorkspace()), QKeySequence::Open);
    m_recentMenu = file->addMenu(QIcon::fromTheme("document-open-recent"), "&Recent");
    m_recentMenu->setEnabled(false);
    file->addAction(tr("Preferences..."), this, SLOT(openPreferences()), QKeySequence::Preferences);
    file->addSeparator();
    file->addAction(QIcon::fromTheme("application-exit"), tr("&Quit"), this, SLOT(close()), QKeySequence::Quit);

    QMenu *view = menuBar()->addMenu(tr("&View"));
    view->addAction(tr("Take Snapshot"), this, SLOT(takeSnapshot()), QKeySequence("Ctrl+F3"));
    view->addSeparator();

    QAction *slow = view->addAction(tr("Slow Down Animations"), this, SLOT(slowDownAnimations(bool)), QKeySequence(tr("Ctrl+.")));
    slow->setCheckable(true);
    view->addSeparator();

    m_refresh = view->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"), m_node, SLOT(refresh()), QKeySequence::Refresh);
    m_enablePluginReload = view->addAction("Reload QML Plugins");
    m_enablePluginReload->setCheckable(true);
    connect(m_enablePluginReload, SIGNAL(toggled(bool)), m_node, SLOT(setReloadPluginsEnabled(bool)));

    m_resizeFit = view->addAction(QIcon::fromTheme("zoom-fit-best"), tr("Resize to Fit"), this, SLOT(resizeToFit()));
    view->addAction(tr("Show Containing Folder"), m_workspace, SLOT(goUp()), QKeySequence("Ctrl+Esc"));
    m_stayOnTop = view->addAction(tr("Stay on Top"), this, SLOT(stayOnTop()));
    m_stayOnTop->setCheckable(true);

    view->addSeparator();
    view->addAction(m_workspaceDock->toggleViewAction());
    m_hostDock->close();
    view->addAction(m_hostDock->toggleViewAction());
    m_logDockMenu = view->addMenu("Logs");
    m_logDockMenu->addAction(m_logDock->toggleViewAction());
}

void MainWindow::readSettings()
{
    QSettings s;
    restoreGeometry(s.value("geometry").toByteArray());
    //Only set the workspace if we didn't already set it by command line
    if (m_hub->workspace() == QDir::currentPath())
        setWorkspace(s.value("workspace").toString());

    if(s.value("http_proxy/enabled").toBool()) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(s.value("http_proxy/hostname").toString());
        proxy.setPort(s.value("http_proxy/port").toInt());
        proxy.setUser(s.value("http_proxy/username").toString());
        proxy.setPassword(s.value("http_proxy/password").toString());
        QNetworkProxy::setApplicationProxy(proxy);
    } else {
        QNetworkProxy::setApplicationProxy(QNetworkProxy());
    }

    int size = s.beginReadArray("recentFolder");
    for(int i = 0; i < size; i++) {
        s.setArrayIndex(i);
        m_recentFolder.append(s.value("folder").toString());
    }
    s.endArray();

    updateRecentFolder();

    //Only set the workspace if we didn't already set it by command line
    if (m_workspace->activeDocument().isEmpty()) {
        if (s.contains("activeDocument"))
            activateDocument(s.value("activeDocument").toString());
        else
            m_workspace->activateRootPath();
    }

    resetImportPaths();

    m_hostModel->restoreFromSettings(&s);
    restoreState(s.value("windowState").toByteArray());
}

void MainWindow::writeSettings()
{
    QSettings s;
    s.setValue("geometry", saveGeometry());
    s.setValue("windowState", saveState());
    s.setValue("workspace", m_hub->workspace());
    s.setValue("activeDocument", m_node->activeDocument().toLocalFile());

    s.beginWriteArray("recentFolder");
    for(int i = 0; i < m_recentFolder.count(); i++) {
        s.setArrayIndex(i);
        s.setValue("folder", m_recentFolder.at(i));
    }
    s.endArray();

    m_hostModel->saveToSettings(&s);
}

void MainWindow::resetImportPaths()
{
    QStringList importPaths;
    QSettings s;
    int count = s.beginReadArray("imports");
    for(int i=0; i<count; i++) {
        s.setArrayIndex(i);
        importPaths.append(s.value("path").toString());
    }
    s.endArray();

    setImportPaths(importPaths);
}

void MainWindow::setupToolBar()
{
    m_toolBar = addToolBar("ToolBar");
    m_toolBar->setObjectName("toolbar");
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toolBar->addAction(m_openWorkspace);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_refresh);
    m_toolBar->addAction(m_resizeFit);
    m_toolBar->addAction(m_enablePluginReload);
}

void MainWindow::activateDocument(const QString path)
{
    m_workspace->activateDocument(path);
}

void MainWindow::resizeToFit()
{
    QSize diff = m_ww->sizeHint() - m_ww->rect().size();
    resize(rect().size() + diff);
    updateWindowTitle();
}

void MainWindow::takeSnapshot()
{
    QImage img;
    img = m_qmlview->grabWindow();

    if (img.isNull()) {
        m_log->appendToLog(LogView::InternalError, tr("QmlLive: could not retrieve snapshot pixmap"));
    } else {
        static int counter = 1;
        QString fileName = QString::fromLatin1("snapshot%1.png").arg(counter++);
        bool ok = img.save(fileName);

        if (!ok) // log
            m_log->appendToLog(LogView::InternalError, tr("QmlLive: could not save snapshot as \"%1\"").arg(fileName));
        else
            m_log->appendToLog(LogView::InternalInfo, tr("QmlLive: created snapshot \"%1\"").arg(fileName));
    }
}

void MainWindow::slowDownAnimations(bool enable)
{
    QUnifiedTimer::instance()->setSlowModeEnabled(enable);
}


void MainWindow::setWorkspace(const QString& path)
{
    m_workspace->setRootPath(path);
    m_node->setWorkspace(path);
    m_hub->setWorkspace(path);
    m_allHosts->setWorkspace(path);
    updateRecentFolder(path);
}

void MainWindow::setPluginPath(const QString &path)
{
    m_node->setPluginPath(path);
}

void MainWindow::setImportPaths(const QStringList &pathList)
{
    m_node->setImportPaths(pathList + m_qmlDefaultimportList);
}

void MainWindow::setStaysOnTop(bool enabled)
{
    m_stayOnTop->setChecked(enabled);
    stayOnTop();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    raise();
}

void MainWindow::openWorkspace()
{
    QString path = QFileDialog::getExistingDirectory(this, "Open Workspace");
    if(path.isEmpty()) {
        return;
    }
    setWorkspace(path);
}

void MainWindow::logQuitEvent()
{
    m_log->appendToLog(LogView::InternalInfo, tr("Qml Viewer tries to quit."));
}

void MainWindow::updateWindowTitle()
{
    setWindowFilePath(QString());
    if (m_hub->activePath().isEmpty()) {
        setWindowTitle(QApplication::applicationName());
    } else {
        setWindowTitle(QString());
        setWindowFilePath(m_hub->activePath());
    }
}

void MainWindow::openPreferences(Host* host)
{
    OptionsDialog dialog;
    dialog.setHostModel(m_hostModel);
    dialog.setDiscoveredHostsModel(m_discoveryManager->discoveredHostsModel());

    if (host)
        dialog.openHostConfig(host);
    if (dialog.exec()) {
        resetImportPaths();
        m_discoveryManager->rescan();
    }
}

void MainWindow::clearRecentFolder()
{
    m_recentFolder.clear();
    m_recentMenu->setEnabled(false);
    updateRecentFolder();
}

void MainWindow::openRecentFolder()
{
    if(QAction* action = qobject_cast<QAction*>(sender())) {
        setWorkspace(action->text());
    }
}

void MainWindow::updateRecentFolder(const QString& path)
{
    if (!path.isEmpty())
        m_recentFolder.prepend(path);
    m_recentFolder.removeDuplicates();

    if (m_recentFolder.count())
        m_recentMenu->setEnabled(true);

    while (m_recentFolder.count() > 7)
        m_recentFolder.removeAt(m_recentFolder.count() - 1);

    m_recentMenu->clear();
    foreach(const QString file, m_recentFolder) {
        m_recentMenu->addAction(file, this, SLOT(openRecentFolder()));
    }

    m_recentMenu->addSeparator();
    m_recentMenu->addAction("Clear Menu", this, SLOT(clearRecentFolder()));
}

void MainWindow::stayOnTop()
{
    Qt::WindowFlags flags = windowFlags();
    if(m_stayOnTop->isChecked()) {
        setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(flags ^ (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
    }
    show();
}
