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

class MainWindowPockemul : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
    QMutex audioMutex,analogMutex;

    MainWindowPockemul( QWidget * parent = 0, Qt::WFlags f = 0 );
    void setZoom(int );
    void setCpu(int );

    DialogLog		*dialoglog;
    dialogAnalog	*dialoganalogic;
    WindowIDE       *windowide;

    //	CpaperWidget	*paperWidget;

    QTimer *FrameTimer;

    CDirectLink		*pdirectLink;
    void SendSignal_AddLogItem(QString str);

    void slotUnlink(CPObject *);

    qint64	rawclk;
    CPObject * LoadPocket(int result);
    ASKYN saveAll;
    float	zoom;

    LaunchButtonWidget* launch;
    LaunchButtonWidget* launch2;

    void doZoom(QPoint point, float delta);

    bool event(QEvent *event);
    bool gestureEvent(QGestureEvent *event);
    void pinchTriggered(QPinchGesture *gesture);
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
    void opensession();
        void saveassession();
    void updateTimer();
    void updateFrameTimer();
    void updateTimeTimer();
    void resizeSlot(QSize size , CPObject *pObject);
    void DestroySlot(CPObject *pObject);
    void slotNewLink(QAction *);
    void slotWebLink(QAction *);
    void slotUnLink(QAction *);
    void slotUnLink(Cconnector *);

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

private:
    QList<CKey>::iterator keyIterator;
    bool startKeyDrag;
    QPoint KeyDrag;
    bool startPosDrag;
    QPoint PosDrag;



};
#endif
