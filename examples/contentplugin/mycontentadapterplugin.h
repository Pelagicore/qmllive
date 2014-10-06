#ifndef MYCONTENTADAPTERPLUGIN_H
#define MYCONTENTADAPTERPLUGIN_H

#include <contentadapterinterface.h>

//! [0]
class MyContentAdapterPlugin : public QObject, public ContentAdapterInterface
{
    Q_OBJECT
//! [1]
    Q_INTERFACES(ContentAdapterInterface)
//! [1]
public:
    explicit MyContentAdapterPlugin(QObject *parent = 0);

    bool canPreview(const QString& path) const;
    QImage preview(const QString& path, const QSize &requestedSize);
    
    bool canAdapt(const QUrl& url) const;
    QUrl adapt(const QUrl& url, QDeclarativeContext* context);
    
};
//! [0]

#endif // MYCONTENTADAPTERPLUGIN_H
