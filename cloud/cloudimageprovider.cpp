#include "cloudimageprovider.h"
#include <QImage>
#include <QPixmap>

#include <QObject>
#include <QDebug>
#include <QEventLoop>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QApplication>

#include "cloudwindow.h"

CloudImageProvider::CloudImageProvider(QObject *parent) : QObject(parent),
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
    // This space intentionally left blank.

    mgr = new QNetworkAccessManager;
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(loadfinished(QNetworkReply*)));
}

CloudImageProvider::~CloudImageProvider()
{
    // This space intentionally left blank.
}


#if 0
QImage CloudImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    QEventLoop loop;
    QTimer timer;
    QNetworkAccessManager *_mgr = new QNetworkAccessManager;

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(_mgr, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
//    connect(_mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(loadfinished(QNetworkReply*)));


    qWarning()<<id<<"   auth_token="<<CloudWindow::getValueFor("auth_token")<<" size="<<requestedSize;

    QUrlQuery qu;
    qu.addQueryItem("apikey","7118206e08fed2c5ec8c0f2db61bbbdc09ab2dfa");
    qu.addQueryItem("auth_token",CloudWindow::getValueFor("auth_token"));

    QNetworkRequest req("http://"+id);
    qWarning()<<req.url();

    QNetworkReply *_reply = _mgr->post(req, qu.query(QUrl::FullyEncoded).toUtf8());
    timer.start(5000); //your predefined timeout

    loop.exec();

    if (timer.isActive()) {
        //replay received before timer, you can then get replay form network access manager and do whatever you want with it
        QByteArray imageData = _reply->readAll();

        QImage image;
        image.loadFromData(imageData);
        return image;
    }
    else //timer elapsed, no replay from client, ups
    {
    }

    return QImage();
}
#else
// CACHE MODULE
QImage CloudImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
//    qWarning()<<id<<"   auth_token="<<CloudWindow::getValueFor("auth_token")<<" size="<<requestedSize;

    QUrlQuery qu;
    qu.addQueryItem("apikey","7118206e08fed2c5ec8c0f2db61bbbdc09ab2dfa");
    qu.addQueryItem("auth_token",CloudWindow::getValueFor("auth_token"));

    QNetworkRequest req("http://"+id);
//    qWarning()<<req.url();
    if (cache.contains(req.url()))
        return cache[req.url()];

    cache[req.url()] = QImage();
    QNetworkReply *_reply = mgr->post(req, qu.query(QUrl::FullyEncoded).toUtf8());

//    qWarning()<<_reply;

    return QImage();
}
#endif

QPixmap CloudImageProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    QString rsrcid = ":/" + id;
    QPixmap image(rsrcid);
    QPixmap result;

    if (requestedSize.isValid()) {
        result = image.scaled(requestedSize, Qt::KeepAspectRatio);
    } else {
        result = image;
    }
    *size = result.size();
    return result;
}

void CloudImageProvider::loadfinished(QNetworkReply *reply)
{
//    qWarning()<<"Downloas finished*******";

    QByteArray imageData = reply->readAll();

    QImage image;
    image.loadFromData(imageData);

    cache[reply->url()] = image;

    reply->deleteLater();

    emit cacheUpdated();
}

