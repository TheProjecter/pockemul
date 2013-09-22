#include <QApplication>
#include <QtWidgets>
#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QListView>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QUrl>
#include <QSettings>
#include <QCryptographicHash>

#include <QtDeclarative/QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>

#include "applicationconfig.h"
#include "imageobject.h"
#include "cloudwindow.h"
#include "imagemodel.h"

#include "mainwindowpockemul.h"
extern MainWindowPockemul *mainwindow;

CloudWindow::CloudWindow(QWidget *parent)
    : QWidget(parent)
{

    view = new QDeclarativeView(this);
    view->setSource(QUrl("qrc:/main.qml"));
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view->rootContext()->setContextProperty("cloud", this);
    connect(view->engine(), SIGNAL(quit()), this,SLOT(hide()));
    QObject *object = view->rootObject();
    QObject::connect(object, SIGNAL(sendWarning(QString)), this, SLOT(warning(QString)));

    m_fileDialog = new QFileDialog(this);
    m_fileDialog->setFileMode(QFileDialog::ExistingFile);
    m_fileDialog->setNameFilter("PockEmul Session files (*.pml)");
    connect(m_fileDialog, SIGNAL(fileSelected(QString)),
            this, SLOT(sendPML(QString)));

    QVBoxLayout *windowLayout = new QVBoxLayout(this);
    windowLayout->addWidget(view);
    windowLayout->setMargin(0);

}

void CloudWindow::resizeEvent(QResizeEvent *e) {
//    view->setGeometry(this->geometry());
}

QSize CloudWindow::sizeHint() const
{
    return QSize(500, 700);
}

void CloudWindow::wheelEvent(QWheelEvent *event)
{
    event->accept();
}

void CloudWindow::refresh()
{

}


void CloudWindow::save()
{
    hide();
    QString filePath =mainwindow->saveassession();

    //fileSelected(filePath);
    sendPML(filePath);
    show();
    refresh();
}






void CloudWindow::sendPML(const QString &filePath)
{
    qWarning()<<"sendPML";
    if (filePath.isEmpty())
        return;

    QFile file(filePath);

    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    file.close();
    QNetworkAccessManager *mgr = new QNetworkAccessManager();
    // the HTTP request

    QString apikey = getValueFor("apikey","");
    QString server = getValueFor("serverURL","http://rrouvin.dyndns.org/cloud/")+"savePML";
    // Check if apikey exists

    QUrlQuery qu;
    qu.addQueryItem("apikey",apikey);
    qu.addQueryItem("content",QString(data).replace("+","%2B"));
    qWarning()<<qu.query(QUrl::FullyEncoded).toUtf8();
    // some parameters for the HTTP request

    QNetworkRequest req(server);
    qWarning()<<req.url();
    QNetworkReply *reply = mgr->post(req, qu.query(QUrl::FullyEncoded).toUtf8());
}




void CloudWindow::downloadFinished()
{
    qWarning()<<"CloudWindow::downloadFinished - ";
    QByteArray xmlData = m_reply->readAll();
//    qWarning() << "data="<<xmlData.left(200);
    QXmlStreamReader *xml = new QXmlStreamReader(xmlData);

    mainwindow->opensession(xml);
//    emit imageChanged(m_object.value("id").toString());

    m_reply->deleteLater();
    this->hide();

}

void CloudWindow::showFileDialog()
{
    m_fileDialog->show();
}

void CloudWindow::getPML(int id) {
    QNetworkAccessManager *mgr = new QNetworkAccessManager();

    QString apikey = getValueFor("apikey","");
    QString server = getValueFor("serverURL","http://rrouvin.dyndns.org/cloud/")+"getPML";
    QNetworkRequest req(server+QString("/%1/%2").arg(getValueFor("apikey","0")).arg(id));
    qWarning()<<req.url();
    m_reply = mgr->get(req);
    connect(m_reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
}


QString CloudWindow::getValueFor(const QString &objectName, const QString &defaultValue)
{
    QSettings settings;
    if (settings.value(objectName).isNull()) {
        qWarning()<<"getValue("<<objectName<<","<<defaultValue<<")";
        return defaultValue;
    }
//    qWarning()<<"getValue("<<objectName<<","<<settings.value(objectName).toString()<<")";
    return settings.value(objectName).toString();
}

void CloudWindow::saveValueFor(const QString &objectName, const QString &inputValue)
{
    QSettings settings;
    settings.setValue(objectName, QVariant(inputValue));
//    qWarning()<<"saveValue("<<objectName<<","<<inputValue<<")";
}

QString CloudWindow::generateKey(QString username,QString password) {
    QString key = QString("!PockEmul"+username+"_"+password+"&éklm!;");
//    qWarning()<<"KEY:"<<key;
    return QCryptographicHash::hash ( key.toLatin1(), QCryptographicHash::Md5);
}

extern int ask(QWidget *parent, QString msg, int nbButton);
void CloudWindow::warning(QString msg) {
    ask(this, msg, 1);
}

bool CloudWindow::isPortraitOrientation() {
    return  (QGuiApplication::primaryScreen()->orientation() == Qt::PortraitOrientation) ||
            (QGuiApplication::primaryScreen()->orientation() == Qt::InvertedPortraitOrientation) ||
             ( (QGuiApplication::primaryScreen()->orientation() == Qt::LandscapeOrientation ||
              QGuiApplication::primaryScreen()->orientation() == Qt::InvertedLandscapeOrientation) &&
             (this->height()>this->width())) ;
}
