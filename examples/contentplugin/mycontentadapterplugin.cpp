#include "mycontentadapterplugin.h"

#include <QtCore/QtPlugin>
#include <QtDeclarative/QDeclarativeContext>

MyContentAdapterPlugin::MyContentAdapterPlugin(QObject *parent) :
    QObject(parent)
{
}

//! [2]
bool MyContentAdapterPlugin::canPreview(const QString &path) const
{
    Q_UNUSED(path)

    return false;
}

QImage MyContentAdapterPlugin::preview(const QString &path, const QSize &requestedSize)
{
    Q_UNUSED(path);
    Q_UNUSED(requestedSize);

    return QImage();
}
//! [2]

//! [0]
bool MyContentAdapterPlugin::canAdapt(const QUrl &url) const
{
    return url.toLocalFile().endsWith(".png");
}
//! [0]

//! [1]
QUrl MyContentAdapterPlugin::adapt(const QUrl &url, QDeclarativeContext *context)
{
    context->setContextProperty("imageSource", url);

    return QString("qrc:/mycontentadatperplugin/plugin.qml");
}
//! [1]

Q_EXPORT_PLUGIN2(myContentAdapterPlugin, MyContentAdapterPlugin)
