#ifndef MAINWINDOWPOCKEMUL_H
#define MAINWINDOWPOCKEMUL_H

#include <QMutex>
#include <QPoint>
//
#include "ui_pockemul.h"
class DialogLog;
class CpaperWidget;
class dialogAnalog;
#include "dialogkeylist.h"
class CPObject;
class CDirectLink;
class CKey;
class CpcXXXX;
class Cconnector;

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
	
//	CpaperWidget	*paperWidget;
	
	QTimer *FrameTimer;

    CDirectLink		*pdirectLink;
	void SendSignal_AddLogItem(QString str);

    void slotUnlink(CPObject *);
	
	qint64	rawclk;

    //Audio audio;

private slots:
	void about();
	void Log();
	void Analogic();
	void CheckUpdates();
    void Minimize_All();
    void SelectPocket(QAction *);
	int  newsession();
	void opensession();
	void updateTimer();
	void updateFrameTimer();
	void updateTimeTimer();
	void resizeSlot(QSize size , CPObject *pObject);
	void DestroySlot(CPObject *pObject);
	void slotNewLink(QAction *);
	void slotUnLink(QAction *);
    void slotUnLink(Cconnector *);

	
protected:
	void paintEvent(QPaintEvent *);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent( QMouseEvent * event );
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
	int	zoom;
};
#endif
