#ifndef MAINWINDOWPOCKEMUL_H
#define MAINWINDOWPOCKEMUL_H

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

class MainWindowPockemul : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
	MainWindowPockemul( QWidget * parent = 0, Qt::WFlags f = 0 );
	void setZoom(int );
	void setCpu(int );

	DialogLog		*dialoglog;
	dialogAnalog	*dialoganalogic;
	
//	CpaperWidget	*paperWidget;
	
	QTimer *FrameTimer;

	CDirectLink		*pdirectLink;
	void SendSignal_AddLogItem(QString str);
	
	qint64	rawclk;

private slots:
	void about();
	void Log();
	void Analogic();
	void CheckUpdates();
	int  newsession();
	void opensession();
	void updateTimer();
	void updateFrameTimer();
	void updateTimeTimer();
	void resizeSlot(QSize size , CPObject *pObject);
	void DestroySlot(CPObject *pObject);
	void slotNewLink(QAction *);
	void slotUnLink(QAction *);
	
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
