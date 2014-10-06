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
#include <QtQuick>

#include "contentadapterinterface.h"

class IpcPeer;
class LiveRuntime;
class ContentPluginFactory;

class LiveNodeEngine : public QObject
{
    Q_OBJECT
public:

    enum UpdateMode {
        ReloadDocument,
        RecreateView,
        RecreateProcess
    };
    Q_ENUMS(UpdateMode)

    explicit LiveNodeEngine(QObject *parent = 0);
    virtual void setView(QQuickView* view);

    int xOffset() const;
    int yOffset() const;
    int rotation() const;

    void setUpdateMode(LiveNodeEngine::UpdateMode mode);
    LiveNodeEngine::UpdateMode updateMode() const;

    void setWorkspace(const QString& path);

    void setImportPaths(const QStringList& paths);
    QStringList importPaths() const;

    void setPluginPath(const QString& path);
    QString pluginPath() const;

    QUrl activeDocument() const;
    ContentAdapterInterface *activePlugin() const;
    bool isReloadPluginsEnabled() const;

public Q_SLOTS:

    void setXOffset(int offset);
    void setYOffset(int offset);
    void setRotation(int rotation);
    void setReloadPluginsEnabled(bool enabled);
    void setActiveDocument(const QString& document);
    void loadDocument(const QUrl& url);
    void delayReload();
    virtual void reloadDocument();
Q_SIGNALS:
    void activateDocument(const QString& document);
    void logClear();
    void logIgnoreMessages(bool on);
    void documentLoaded();
    void viewChanged(QQuickView *view);
    void logErrors(const QList<QQmlError> &errors);


protected:
    virtual QQuickView* initView();
    virtual void initPlugins();
    QList<ContentAdapterInterface*> m_plugins;
    QUrl m_activeFile;
    LiveRuntime *m_runtime;
private Q_SLOTS:
    void onStatusChanged(QQuickView::Status status);
    void onSizeChanged();

    void recreateView();

    void checkQmlFeatures(QQuickView *view);

private:
    QUrl queryDocumentViewer(const QUrl& url);
private:
    int m_xOffset;
    int m_yOffset;
    int m_rotation;

    IpcPeer *m_ipc;
    QQuickView *m_view;
    QQuickView *m_recreateView;
    QPointer<QQmlComponent> m_windowComponent;
    QPointer<QObject> m_windowObject;
    QDir m_workspace;
    QTimer *m_delayReload;
    QStringList m_importPaths;

    UpdateMode m_mode;

    ContentPluginFactory* m_pluginFactory;
    ContentAdapterInterface* m_activePlugin;

    ContentAdapterInterface::Features m_quickFeatures;

    bool m_reloadPlugins;
};

