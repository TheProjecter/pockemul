#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QDialog>
#include <QMap>
#include <QModelIndex>
#include <QDeclarativeView>


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


    Q_INVOKABLE void getPML(int id);
    Q_INVOKABLE QString getValueFor(const QString &objectName, const QString &defaultValue);
    Q_INVOKABLE void saveValueFor(const QString &objectName, const QString &inputValue);
    Q_INVOKABLE QString generateKey(QString username, QString password);
protected:
    void wheelEvent(QWheelEvent *event);

public slots:
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void save();
    Q_INVOKABLE void showFileDialog();

private slots:

    void sendPML(const QString &filePath);
    void downloadFinished();

private:
    QDeclarativeView *view;

    QNetworkReply *m_reply;
    QFileDialog *m_fileDialog;

    void resizeEvent(QResizeEvent *e);
};

#endif // MAINWINDOW_H
