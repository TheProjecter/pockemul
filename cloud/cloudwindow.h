#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QMap>
#include <QModelIndex>

class EnginioClient;
class ImageModel;
class PMLModel;
class EnginioReply;
class ImageDownloader;
class ImageObject;
class QFileDialog;
class QListView;
class QNetworkReply;
class QPushButton;

class CloudWindow : public QWidget
{
    Q_OBJECT
    
public:
    CloudWindow(QWidget *parent = 0);
    virtual QSize sizeHint() const;


protected:
    void wheelEvent(QWheelEvent *event);

public slots:

private slots:
    void refresh();
    void save();
    void queryImages();
    void queryPMLFiles(bool force=false);
    void error(EnginioReply *error);
    void beginUpload(EnginioReply *reply);
    void fileSelected(const QString &file);
    void uploadFinished(EnginioReply *reply);
    void downloadData(QModelIndex index);
    void replyFinished(EnginioReply *enginioReply);
    void downloadFinished();

private:
    // The Enginio client object used in all enginio operations
    EnginioClient *m_client;

    // Enginio object model containing image objects
    PMLModel *m_model;

    // The list view showing contents of m_model
    QListView *m_view;
    QPushButton *m_refreshButton;
    QPushButton *m_saveButton;
    QPushButton *m_uploadButton;
    QPushButton *m_quitButton;
    QFileDialog *m_fileDialog;
    QMap<EnginioReply*, QString> m_uploads;

    QNetworkReply *m_reply;


};

#endif // MAINWINDOW_H
