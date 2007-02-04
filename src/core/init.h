/********************************************************************************************************
 * PROGRAM      : test
 * DATE - TIME  : samedi 28 octobre 2006 - 12h42
 * AUTHOR       :  (  )
 * FILENAME     : init.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef _INIT_H
#define _INIT_H

#include <QEvent>
#include <QResizeEvent>
#include <QCustomEvent>
#include <QThread>
#include <QTimer>

class CPObject;
class CPocketThread;
class CWatchPoint;

typedef qint64 pwatchfonc(void);

extern CPObject * InitApp(int idPC);
extern QList<CPocketThread*> listPcThread;	
extern CWatchPoint WatchPoint;


class CWatchPoint
{
public:
	QList<qint64 *> Point;
	QList<qint8> PointSize;
	QList<qint8> nbBits;
	QList<CPObject *> PObject;
	QList<QString> WatchPointName; 
	
	void remove(CPObject * object)
	{
		for (int i=PObject.size()-1;i>=0;i--)
		{
			if (PObject.at(i) == object)
			{
				Point.removeAt(i);
				PointSize.removeAt(i);
				nbBits.removeAt(i);
				PObject.removeAt(i);
				WatchPointName.removeAt(i);
			}
		}
	};
	
	void add( qint64 * watchpoint, qint8 ptrSize,qint8 nbbits,CPObject * object, QString name)
	{
		Point.append(watchpoint);
		PointSize.append(ptrSize);
		nbBits.append(nbbits);
		PObject.append(object);
		WatchPointName.append(name);
	};
};

class CPocketThread:public QThread
{
Q_OBJECT

public:
    void run();
	QTimer *threadTimer;
	void SendSignal_AddLogItem(QString);
	void SendSignal_Resize(QSize,CPObject * );
	

	CPocketThread(CPObject * material)
	{
//		pPC = material;
	}
	
signals:
	void AddLogItem(QString);
	void Resize(QSize,CPObject * );
	void Destroy(CPObject * );
    
};

extern CPocketThread* PcThread;
#endif
