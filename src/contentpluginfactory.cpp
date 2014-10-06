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

#include "contentpluginfactory.h"
#include "contentadapterinterface.h"

#include <QPluginLoader>
#include <QDirIterator>
#include <QDebug>

ContentPluginFactory::ContentPluginFactory(QObject *parent) :
    QObject(parent) ,
    m_loaded(false)
{
    m_pluginPath = QDir::currentPath() + QDir::separator() + "plugins";
}

void ContentPluginFactory::setPluginPath(const QString &path)
{
    m_pluginPath = path;
}

QString ContentPluginFactory::pluginPath()
{
    return m_pluginPath;
}


QList<ContentAdapterInterface *> ContentPluginFactory::plugins()
{
    return m_plugins;
}

bool ContentPluginFactory::isLoaded()
{
    return m_loaded;
}

void ContentPluginFactory::load()
{
    if (m_loaded)
        return;

    QDirIterator it(m_pluginPath);

    while (it.hasNext()) {
        const QString path = it.next();

        if (!it.fileInfo().isFile())
            continue;

        QPluginLoader loader(path);

        ContentAdapterInterface* plugin = qobject_cast<ContentAdapterInterface*>(loader.instance());
        if (plugin)
            m_plugins.append(plugin);

        if (!loader.isLoaded())
            qWarning() << "Error while trying to load" <<path << ":" << loader.errorString();
    }

    if (!m_plugins.isEmpty())
        m_loaded = true;
}
