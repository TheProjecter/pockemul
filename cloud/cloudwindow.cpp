#include <QtWidgets>
#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QListView>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include <enginioclient.h>
#include <enginioreply.h>

#include "applicationconfig.h"
#include "imageobject.h"
#include "cloudwindow.h"
#include "imagemodel.h"

#include "mainwindowpockemul.h"
extern MainWindowPockemul *mainwindow;

CloudWindow::CloudWindow(QWidget *parent)
    : QWidget(parent)
{
//    setWindowTitle(QStringLiteral("Enginio Image Gallery"));

    m_client = new EnginioClient(this);
    m_client->setBackendId(Enginio::BACKEND_ID);
    m_client->setBackendSecret(Enginio::BACKEND_SECRET);
    qWarning()<<"E1";
    m_model = new PMLModel(this);
    m_model->setEnginio(m_client);
qWarning()<<"E2";
    m_view = new QListView;
    m_view->setModel(m_model);
    m_view->setViewMode(QListView::IconMode);
    m_view->setResizeMode(QListView::Adjust);
    m_view->setGridSize(QSize(304, 204));
    connect(m_view,SIGNAL(clicked(QModelIndex)),this,SLOT(downloadData(QModelIndex)));
qWarning()<<"E3";
    m_fileDialog = new QFileDialog(this);
    m_fileDialog->setFileMode(QFileDialog::ExistingFile);
    m_fileDialog->setNameFilter("PockEmul Session files (*.pml)");
    connect(m_fileDialog, SIGNAL(fileSelected(QString)),
            this, SLOT(fileSelected(QString)));

    m_refreshButton = new QPushButton("Refresh");
    connect(m_refreshButton, SIGNAL(clicked()),
            this, SLOT(refresh()));

    m_saveButton = new QPushButton("Save Session");
    connect(m_saveButton, SIGNAL(clicked()),
            this, SLOT(save()));

    m_uploadButton = new QPushButton("Upload pml file");
    connect(m_uploadButton, SIGNAL(clicked()),
            m_fileDialog, SLOT(show()));

    m_quitButton = new QPushButton("Quit");
    connect(m_quitButton, SIGNAL(clicked()),
            this, SLOT(hide()));

//    QFrame *frame = new QFrame(this);
    QVBoxLayout *windowLayout = new QVBoxLayout(this);
    windowLayout->addWidget(m_view);
    QHBoxLayout *windowHLayout = new QHBoxLayout(this);
    windowLayout->addLayout(windowHLayout);
    windowHLayout->addWidget(m_refreshButton);
    windowHLayout->addWidget(m_saveButton);
    windowHLayout->addWidget(m_uploadButton);
    windowHLayout->addWidget(m_quitButton);
//    setCentralWidget(frame);
qWarning()<<"E4";
    //queryImages();
    queryPMLFiles();
    qWarning()<<"E5";
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
    queryPMLFiles(true);
}

void CloudWindow::save()
{
    hide();
    QString filePath =mainwindow->saveassession();

    fileSelected(filePath);
    show();
    refresh();
}

void CloudWindow::queryImages()
{
    qWarning()<<"queryImages";
    QJsonObject query;
    query.insert("objectType", QStringLiteral("objects.image"));
    QJsonObject fileObject;
    fileObject.insert("file", QJsonObject());

    // Use include parameter to request full "file" object for every result
    // object including image and thumbnail URLs.
    query.insert("include", fileObject);

    // Filter out objects which have not yet been uploaded completely
    QJsonObject filter;
    QJsonObject exists;
    exists.insert("$exists", true);
    filter.insert("file.id", exists);
    query.insert("query", filter);

    m_model->setQuery(query);
}

void CloudWindow::queryPMLFiles(bool force)
{
    qWarning()<<"queryPMLFiles";
    QJsonObject query;
    query.insert("objectType", QStringLiteral("objects.pmlFiles"));

    // Filter out objects which have not yet been uploaded completely
    QJsonObject filter;
    QJsonObject exists;
    exists.insert("$exists", true);
    filter.insert("id", exists);
    query.insert("query", filter);

    m_model->setQuery(query,force);
}

void CloudWindow::error(EnginioReply *error)
{
    qWarning() << Q_FUNC_INFO << error;
}

void CloudWindow::fileSelected(const QString &filePath)
{
    qWarning()<<"fileSelected";
    if (filePath.isEmpty())
        return;

    QJsonObject object;
    object.insert("objectType", QString("objects.pmlFiles"));
    QString fileName = filePath.split(QDir::separator()).last();
    object.insert("name", fileName);
    object.insert("description", QString("DESCRIPTION"));

    QString screenshot;
    QString xmldata;

    QFile file(filePath);

    file.open(QIODevice::ReadOnly);
    QXmlStreamReader xml;
    xml.setDevice(&file);
    while (!xml.atEnd()) {
        if (xml.readNextStartElement()) {
            QString elt = xml.name().toString();
            if (elt=="snapshot")  {
                screenshot=xml.readElementText();
                break;
            }
        }
    }
    file.reset();

    file.close();

    object.insert("screenshot", screenshot);
    EnginioReply *reply = m_model->append(object);
    connect(reply, SIGNAL(finished(EnginioReply*)), this, SLOT(beginUpload(EnginioReply*)));
    m_uploads.insert(reply, filePath);
}


void CloudWindow::beginUpload(EnginioReply *reply)
{
    reply->deleteLater();
    QString path = m_uploads.take(reply);
    QString objectId = reply->data().value("id").toString();


    QJsonObject object;
    object["id"] = objectId;
    object.insert("objectType", QString("objects.pmlFiles"));
    object["propertyName"] = QStringLiteral("file");

    QJsonObject fileObject;
    fileObject[QStringLiteral("fileName")] = path;

    QJsonObject uploadJson;
    uploadJson[QStringLiteral("targetFileProperty")] = object;
    uploadJson[QStringLiteral("file")] = fileObject;

    EnginioReply *upload = m_client->uploadFile(uploadJson, QUrl::fromUserInput(path));
    connect(upload, SIGNAL(finished(EnginioReply*)), this, SLOT(uploadFinished(EnginioReply*)));
    m_uploads.insert(upload,objectId);
}

void CloudWindow::uploadFinished(EnginioReply *reply)
{
    QJsonDocument jdoc(reply->data());
    qWarning()<<":"<<jdoc.toJson();
    QString id = m_uploads.take(reply);
    QString fileid = reply->data().value("id").toString();
qWarning()<<"upload finished : "<<fileid;
    PMLObject * pmlobject = m_model->getPMLObject(id);


    pmlobject->updateFileId(fileid);

}

void CloudWindow::downloadData(QModelIndex index)
{
    qWarning()<<"downloadData";
    QJsonObject rowData = m_model->data(index,PMLModel::Object).value<QJsonObject>();
    QString fileid = rowData.value("file").toObject().value("id").toString();
    if (fileid=="")
        fileid = rowData.value("fileid").toString();
     qWarning()<<"fileid="<<fileid;
    if (!fileid.isEmpty()) {
        QJsonObject fileObject;
        fileObject.insert("id", fileid);
        EnginioReply *reply = m_client->downloadFile(fileObject);
        qWarning()<<"yes";
        connect(reply, SIGNAL(finished(EnginioReply*)), this, SLOT(replyFinished(EnginioReply*)));
    }
}
void CloudWindow::replyFinished(EnginioReply *enginioReply)
{
    qWarning()<<"replyFinished";
    QString url = enginioReply->data().value("expiringUrl").toString();
    qWarning()<<url;
    QNetworkRequest request(url);
    m_reply = m_client->networkManager()->get(request);
//    m_reply->setParent(this);
    connect(m_reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
}

void CloudWindow::downloadFinished()
{
    qWarning()<<"CloudWindow::downloadFinished - ";
    QByteArray xmlData = m_reply->readAll();
    qWarning() << "data="<<xmlData.left(200);
    QXmlStreamReader *xml = new QXmlStreamReader(xmlData);

    mainwindow->opensession(xml);
//    emit imageChanged(m_object.value("id").toString());

    m_reply->deleteLater();
    this->hide();

}

