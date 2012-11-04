#ifndef MAINWINDOWPOCKEMUL_H
#define MAINWINDOWPOCKEMUL_H

#include <QMutex>
#include <QPoint>
#include <QGesture>
#include <QGestureEvent>
//
#include "ui_pockemul.h"
#include "common.h"


class DialogLog;
class CpaperWidget;
class dialogAnalog;
class CPObject;
class CDirectLink;
class CKey;
class CpcXXXX;
class Cconnector;
class WindowIDE;
class LaunchButtonWidget;
class QCommandLine;
class DownloadManager;

class MainWindowPockemul : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
    QMutex audioMutex,analogMutex;

    MainWindowPockemul( QWidget * parent = 0, Qt::WFlags f = 0 );
    ~MainWindowPockemul();

    void setZoom(int );
    void setCpu(int );

    DialogLog		*dialoglog;
    dialogAnalog	*dialoganalogic;
    WindowIDE       *windowide;
    DownloadManager *downloadManager;

    //	CpaperWidget	*paperWidget;

    QTimer *FrameTimer;

    CDirectLink		*pdirectLink;
    void SendSignal_AddLogItem(QString str);
    void SendSignal_AddLogConsole(QString str);

    void slotUnlink(CPObject *);

    qint64	rawclk;
    CPObject * LoadPocket(int result);
    ASKYN saveAll;
    float	zoom;

    void doZoom(QPoint point, float delta, int step=10);

    bool event(QEvent *event);
    bool gestureEvent(QGestureEvent *event);
    void pinchTriggered(QPinchGesture *gesture);
    void MoveAll(QPoint p);

    void initCommandLine();

    QCommandLine *cmdline;

public slots:
    void opensession(QString sessionFN=QString());

private slots:
    void about();
    void Log();
    void IDE();
    void Analogic();
    void CheckUpdates();
    void Minimize_All();
    void Close_All();
    void resetZoom();
    void SelectPocket(QAction *);
    int  newsession();

        void saveassession();
    void updateTimer();
    void updateFrameTimer();
    void updateTimeTimer();
    void resizeSlot(QSize size , CPObject *pObject);
    void DestroySlot(CPObject *pObject);
    void slotNewLink(QAction *);
    void slotWebLink(QAction *);
    void slotDocument(QAction *);
    void slotUnLink(QAction *);
    void slotUnLink(Cconnector *);
    void slotMsgError(QString);

    void parseError(const QString &error);
    void paramFound(const QString &name, const QVariant &value);
    void optionFound(const QString &name, const QVariant &value);
    void switchFound(const QString &name);

protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent( QMouseEvent * event );
        void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *event);

signals:
    void AddLogItem(QString);
    void AddLogConsole(QString);
    void DestroySignal(CPObject *pObject);
    void NewPObjectsSignal(CPObject *pObject);

private:
    QList<CKey>::iterator keyIterator;
    bool startKeyDrag;
    QPoint KeyDrag;
    bool startPosDrag;
    QPoint PosDrag;
    qreal scaleFactor;



};
#endif
