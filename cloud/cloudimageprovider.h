#ifndef CLOUDIMAGEPROVIDER_H
#define CLOUDIMAGEPROVIDER_H


#include <QDeclarativeImageProvider>
#include <QMap>
#include <QNetworkReply>
#include <QMutex>

class CloudImageProvider : public QObject, public QDeclarativeImageProvider
{
Q_OBJECT

public:
    CloudImageProvider(QObject*);
    ~CloudImageProvider();
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);
    QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize);

    QMap<QUrl,QImage> cache;
    QNetworkAccessManager * mgr;

    QMutex mutex;
signals:
    Q_INVOKABLE void cacheUpdated();

public slots:
    void loadfinished(QNetworkReply *reply);
};
#endif // CLOUDIMAGEPROVIDER_H
