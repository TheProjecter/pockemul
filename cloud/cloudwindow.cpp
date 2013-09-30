#include <QApplication>
#if QT_VERSION >= 0x050000
// Qt5 code
#   include <QtWidgets>
#else
// Qt4 code
#   include <QtCore>
#   include <QFileDialog>
#   include <QGraphicsObject>
#   include <QtGui>
#endif
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
#include "cloudwindow.h"


#include "mainwindowpockemul.h"
extern MainWindowPockemul *mainwindow;
extern int ask(QWidget *parent, QString msg, int nbButton);
extern void m_addShortcut(QString param);

CloudWindow::CloudWindow(QWidget *parent)
    : QWidget(parent)
{

    view = new QDeclarativeView(this);
    view->rootContext()->setContextProperty("cloud", this);
    view->setSource(QUrl("qrc:/main.qml"));
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    connect(view->engine(), SIGNAL(quit()), this,SLOT(hide()));
    object = view->rootObject();
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


QString CloudWindow::save()
{
    hide();
    mainwindow->repaint();

    QString s = mainwindow->saveassessionString().remove(0,1);
    qWarning()<<"session saved";
    qWarning()<<s.left(500);
    qWarning()<<"session saved";
    show();
    return s;

//    sendPML(filePath);
//    show();
//    refresh();
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

#if QT_VERSION >= 0x050000
// Qt5 code
    QUrlQuery qu;
    qu.addQueryItem("apikey",apikey);
    qu.addQueryItem("content",QString(data).replace("+","%2B"));
    qWarning()<<qu.query(QUrl::FullyEncoded).toUtf8();
    // some parameters for the HTTP request

    QNetworkRequest req(server);
    qWarning()<<req.url();
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(finishedSave(QNetworkReply*)));
    QNetworkReply *reply = mgr->post(req, qu.query(QUrl::FullyEncoded).toUtf8());
#else
// Qt4 code
#endif
}

void CloudWindow::finishedSave(QNetworkReply *reply)
{
    QByteArray xmlData = reply->readAll();
    qWarning()<<"received id:"<<xmlData;

    QString pmlid,username,ispublic,isdeleted,title,description,objects,listobjects;

    QXmlStreamReader *xml = new QXmlStreamReader(xmlData);
    if (xml->readNextStartElement()) {
        if (xml->name() == "item") {
            while (!xml->atEnd()) {
                while (xml->readNextStartElement()) {
                    QString eltname = xml->name().toString();
                    if (eltname == "pmlid") pmlid = xml->readElementText();
                    if (eltname == "username") username = xml->readElementText();
                    if (eltname == "ispublic") ispublic = xml->readElementText();
                    if (eltname == "isdeleted") isdeleted = xml->readElementText();
                    if (eltname == "title") title = xml->readElementText();
                    if (eltname == "description") description = xml->readElementText();
                    if (eltname == "objects") objects = xml->readElementText();
                    if (eltname == "listobjects") listobjects = xml->readElementText();
                }
            }
        }
    }
    qWarning()<<pmlid<<username<<ispublic<<isdeleted<<title<<description<<objects<<listobjects;

    QMetaObject::invokeMethod(object, "addRefPmlModel",
                              Q_ARG(QVariant, pmlid),
                              Q_ARG(QVariant, username),
                              Q_ARG(QVariant, objects),
                              Q_ARG(QVariant, listobjects),
                              Q_ARG(QVariant, ispublic),
                              Q_ARG(QVariant, isdeleted),
                              Q_ARG(QVariant, title),
                              Q_ARG(QVariant, description)
                              );


    reply->deleteLater();
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


void CloudWindow::warning(QString msg) {
    ask(this, msg, 1);
}

void CloudWindow::addShortcut(QString param) {
    m_addShortcut(param);
}

bool CloudWindow::isPortraitOrientation() {
#if QT_VERSION >= 0x050000

    return  (QGuiApplication::primaryScreen()->orientation() == Qt::PortraitOrientation) ||
            (QGuiApplication::primaryScreen()->orientation() == Qt::InvertedPortraitOrientation) ||
             ( (QGuiApplication::primaryScreen()->orientation() == Qt::LandscapeOrientation ||
              QGuiApplication::primaryScreen()->orientation() == Qt::InvertedLandscapeOrientation) &&
             (this->height()>this->width())) ;

#endif
}
