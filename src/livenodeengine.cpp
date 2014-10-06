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

#include "livenodeengine.h"
#include "liveruntime.h"
#include "qmlhelper.h"
#include "contentpluginfactory.h"
#include "imageadapter.h"
#include "fontadapter.h"

#include "QtQml/qqml.h"
#include "QtQuick/private/qquickpixmapcache_p.h"

// TODO: create proxy configuration settings, controlled by command line and ui

#ifdef QMLLIVE_DEBUG
#define DEBUG qDebug()
#else
#define DEBUG if (0) qDebug()
#endif

/*!
 * \class LiveNodeEngine
 * \brief The LiveNodeEngine class instruments a qml viewer in cooperation with LiveHubEngine.
 * \group qmllive
 *
 * LiveNodeEngine provides ways to reload qml documents based incoming requests
 * from a hub. A hub can be connected via a local mode ( LocalPublisher) or
 * remote mode (RemotePublisher with a RemoteReceiver).
 *
 * In Addition to showing qml-Files the LiveNodeEngine can be extended by plugins to show any other datatype.
 * One need to set the Plugin path to the right destination and the LiveNodeEngine will load all the plugins
 * it finds there.
 *
 * \sa {ContentPlugin Example}
 */

/*!
 * Standard constructor using \a parent as parent
 */
LiveNodeEngine::LiveNodeEngine(QObject *parent)
    : QObject(parent)
    , m_runtime(new LiveRuntime(this))
    , m_xOffset(0)
    , m_yOffset(0)
    , m_rotation(0)
    , m_view(0)
    , m_recreateView(0)
    , m_delayReload(new QTimer(this))
    , m_mode(ReloadDocument)
    , m_pluginFactory(new ContentPluginFactory(this))
    , m_activePlugin(0)
    , m_reloadPlugins(false)
{
    m_delayReload->setInterval(250);
    m_delayReload->setSingleShot(true);
    connect(m_delayReload, SIGNAL(timeout()), this, SLOT(reloadDocument()));
}

/*!
 * Sets the view which should be used to \a view
 */
    void LiveNodeEngine::setView(QQuickView *view)
{
    m_view = view;
    m_view->rootContext()->setContextProperty("livert", m_runtime);
    m_view->engine()->setOutputWarningsToStandardError(false); // log
    m_view->engine()->setImportPathList(m_importPaths);

    connect(m_view->engine(), SIGNAL(warnings(QList<QQmlError>)),
            this, SIGNAL(logErrors(QList<QQmlError>)));
    connect(m_view, SIGNAL(statusChanged(QQuickView::Status)),
            this, SLOT(onStatusChanged(QQuickView::Status)));
    connect(m_view, SIGNAL(widthChanged(int)),
            this, SLOT(onSizeChanged()));
    connect(m_view, SIGNAL(heightChanged(int)),
            this, SLOT(onSizeChanged()));
}

void LiveNodeEngine::setXOffset(int offset)
{
    QQuickView* view = 0;

    if (m_view)
        view = m_view;
    if (m_recreateView)
        view = m_recreateView;
    if (view)
        view->contentItem()->setX(offset);

    m_xOffset = offset;
}

int LiveNodeEngine::xOffset() const
{
    return m_xOffset;
}

void LiveNodeEngine::setYOffset(int offset)
{
    QQuickView* view = 0;

    if (m_view)
        view = m_view;
    if (m_recreateView)
        view = m_recreateView;
    if (view)
        view->contentItem()->setY(offset);

    m_yOffset = offset;
}

int LiveNodeEngine::yOffset() const
{
    return m_yOffset;
}

void LiveNodeEngine::setRotation(int rotation)
{
    QQuickView* view = 0;

    if (m_view)
        view = m_view;
    if (m_recreateView)
        view = m_recreateView;
    if (view) {
        view->contentItem()->setRotation(0);
        view->contentItem()->setTransformOriginPoint(QPointF(view->width()/2, view->height()/2));
        view->contentItem()->setRotation(rotation);
    }

    m_rotation = rotation;
}

int LiveNodeEngine::rotation() const
{
    return m_rotation;
}

void LiveNodeEngine::setUpdateMode(LiveNodeEngine::UpdateMode mode)
{
    m_mode = mode;
}

LiveNodeEngine::UpdateMode LiveNodeEngine::updateMode() const
{
    return m_mode;
}

/*!
 * Loads the given \a url onto the qml view. Clears any caches and reloads
 * the dummy data initially.
 */
void LiveNodeEngine::loadDocument(const QUrl& url)
{
    DEBUG << "LiveNodeEngine::loadDocument: " << url;
    m_activeFile = url;

    if (!m_activeFile.isEmpty())
        reloadDocument();
}

void LiveNodeEngine::delayReload()
{
    m_delayReload->start();
}

/*!
 * Sets the view which should be used to \a view
 */
void LiveNodeEngine::recreateView()
{
    if (m_windowComponent)
        delete m_windowComponent;
    if (m_windowObject)
        delete m_windowObject;

    if (m_recreateView) {
        //m_recreateView->setSource(QUrl());
        m_recreateView->engine()->clearComponentCache();
        delete m_recreateView;
        if (m_reloadPlugins)
            qmlClearTypeRegistrations();
        QQuickPixmap::purgeCache();
    }

    m_recreateView = initView();

    setXOffset(m_xOffset);
    setYOffset(m_yOffset);

    m_recreateView->rootContext()->setContextProperty("livert", m_runtime);
    m_recreateView->engine()->setOutputWarningsToStandardError(false); // log
    if (!m_importPaths.isEmpty())
        m_recreateView->engine()->setImportPathList(m_importPaths);

    m_recreateView->engine()->clearComponentCache();
    QmlHelper::loadDummyData(m_recreateView, m_workspace.absolutePath());

    connect(m_recreateView->engine(), SIGNAL(warnings(QList<QQmlError>)),
            this, SIGNAL(logErrors(QList<QQmlError>)));
    connect(m_recreateView, SIGNAL(statusChanged(QQuickView::Status)),
            this, SLOT(onStatusChanged(QQuickView::Status)));
    connect(m_recreateView, SIGNAL(widthChanged(int)),
            this, SLOT(onSizeChanged()));
    connect(m_recreateView, SIGNAL(heightChanged(int)),
            this, SLOT(onSizeChanged()));
}

void LiveNodeEngine::checkQmlFeatures(QQuickView *view)
{
    foreach(QString importPath, view->engine()->importPathList()) {
        QDir dir(importPath);
        if (dir.exists("QtQuick/Controls") &&
            dir.exists("QtQuick/Layouts") &&
            dir.exists("QtQuick/Dialogs")) {
            m_quickFeatures |= ContentAdapterInterface::QtQuickControls;
        }
    }
}

/*!
 * Reloads the qml view source.
 */
void LiveNodeEngine::reloadDocument()
{
    QQuickView *view = 0;

    if (m_mode == RecreateView) {
        recreateView();

        view = m_recreateView;
    } else {
        view = m_view;
        view->setSource(QUrl());
    }

    checkQmlFeatures(view);

    emit logClear();
    //emit logIgnoreMessages(true);

    QUrl url = queryDocumentViewer(m_activeFile);
    QQmlEngine* engine = view->engine();
    m_windowComponent = new QQmlComponent(engine);

    m_windowComponent->loadUrl(url);

    m_windowObject = m_windowComponent->create();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(m_windowObject);
    if (window) {
        engine->setIncubationController(window->incubationController());
        window->show();
    } else {
        view->setContent(url, m_windowComponent, m_windowObject);
    }

    //emit logIgnoreMessages(false);

    QList<QQmlError> errors = view->errors();
    if(!errors.isEmpty()) {
        emit logErrors(errors);
        if (m_quickFeatures.testFlag(ContentAdapterInterface::QtQuickControls))
            view->setSource(QUrl("qrc:/livert/error_qt5_controls.qml"));
        view->setSource(QUrl("qrc:/livert/error_qt5.qml"));
    }

    if (m_mode == RecreateView)
        view->setVisible(true);

    if (m_activePlugin || (view->rootObject() && QSize(view->rootObject()->width(), view->rootObject()->height()).isEmpty())) {
        view->setResizeMode(QQuickView::SizeRootObjectToView);
    } else {
        view->setResizeMode(QQuickView::SizeViewToRootObject);
    }
}


/*!
 * Allows to adapt a \a url to display not native qml documents (e.g. images).
 */
QUrl LiveNodeEngine::queryDocumentViewer(const QUrl& url)
{
    initPlugins();

    foreach(ContentAdapterInterface* adapter, m_plugins) {
        if (adapter->canAdapt(url)) {
            adapter->cleanUp();
            adapter->setAvailableFeatures(m_quickFeatures);

            m_activePlugin = adapter;

            if (m_mode == RecreateView)
                return adapter->adapt(url, m_recreateView->rootContext());
            else
                return adapter->adapt(url, m_view->rootContext());
        }
    }

    m_activePlugin = 0;

    return url;
}

/*!
 * Sets the document \a document to be shown
 *
 */
void LiveNodeEngine::setActiveDocument(const QString &document)
{
    QUrl url;
    if(!document.isEmpty()) {
        url = QUrl::fromLocalFile(m_workspace.absoluteFilePath(document));
    }

    loadDocument(url);
    emit activateDocument(document);
}

/*!
 * Sets the current workspace to \a path. Documents location will be adjusted based on
 * this workspace path.
 */
void LiveNodeEngine::setWorkspace(const QString &path)
{
    m_workspace = QDir(path);
}

void LiveNodeEngine::setImportPaths(const QStringList &paths)
{
    m_importPaths = paths;

    if (m_view)
        m_view->engine()->setImportPathList(paths);
}

QStringList LiveNodeEngine::importPaths() const
{
    return m_importPaths;
}

/*!
 * Sets the pluginPath to \a path.
 *
 * The pluginPath will be used to load QmlLive plugins
 * \sa {ContentPlugin Example}
 */
void LiveNodeEngine::setPluginPath(const QString &path)
{
    m_pluginFactory->setPluginPath(path);
}

/*!
 * Returns the current pluginPath
 */
QString LiveNodeEngine::pluginPath() const
{
    return m_pluginFactory->pluginPath();
}

/*!
 * Returns the current active document url.
 */
QUrl LiveNodeEngine::activeDocument() const
{
    return m_activeFile;
}

ContentAdapterInterface *LiveNodeEngine::activePlugin() const
{
    return m_activePlugin;
}

void LiveNodeEngine::setReloadPluginsEnabled(bool enabled)
{
    m_reloadPlugins = enabled;
}

bool LiveNodeEngine::isReloadPluginsEnabled() const
{
    return m_reloadPlugins;
}

/*!
 * Loads all plugins found in the Pluginpath
 */
void LiveNodeEngine::initPlugins()
{
    if (m_plugins.isEmpty()) {
        m_pluginFactory->load();
        m_plugins.append(m_pluginFactory->plugins());
        m_plugins.append(new ImageAdapter(this));
        m_plugins.append(new FontAdapter(this));
    }
}

void LiveNodeEngine::onStatusChanged(QQuickView::Status status)
{
    if (status == QQuickView::Ready ||
        status == QQuickView::Error) {
        emit documentLoaded();
    }
}

QQuickView *LiveNodeEngine::initView()
{
    QQuickView* view = new QQuickView();
    emit viewChanged(view);
    return view;
}

void LiveNodeEngine::onSizeChanged()
{
    int width = -1, height = -1;

    if (m_mode == RecreateView && m_recreateView) {
        width = m_recreateView->width();
        height = m_recreateView->height();
    } else if (m_mode == ReloadDocument && m_view) {
        width = m_view->width();
        height = m_view->height();
    }

    if (width != -1 && height != -1) {
        m_runtime->setScreenWidth(width);
        m_runtime->setScreenHeight(height);
    }

    setRotation(m_rotation);
}

/*!
 * \fn void LiveNodeEngine::activateDocument(const QString& document);
 * The document \a document was activated
 */

/*!
 * \fn void LiveNodeEngine::logClear();
 * Requested to clear the log
 */

/*!
 * \fn void LiveNodeEngine::logIgnoreMessages(bool);
 * Requsted to ignore the Messages when \a on is true
 */

/*!
 * \fn void LiveNodeEngine::logErrors(const QList<QQmlError> &errors);
 * Log the Errors \a errors
 */
