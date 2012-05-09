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
#include <QStringList>


class CPocketThread;
class CWatchPoint;
class CPObject;

typedef qint64 pwatchfonc(void);

extern CPObject * InitApp(int idPC);
extern QList<CPocketThread*> listPcThread;	
extern CWatchPoint WatchPoint;

class CWatchPointItem {
public:
    CWatchPointItem(qint64* Point,qint8 PointSize,qint8 nbBits,CPObject* PObject,QString WatchPointName,QHash<int,QString> Labels) {
        this->Point = Point;
        this->PointSize = PointSize;
        this->nbBits = nbBits;
        this->PObject = PObject;
        this->WatchPointName = WatchPointName;
        this->Labels = Labels;
    }

    qint64 *Point;
    qint8   PointSize;
    qint8   nbBits;
    CPObject* PObject;
    QString WatchPointName;
    QHash<int,QString> Labels;
};

class CWatchPoint
{
public:
    QList<CWatchPointItem> items;
//	QList<qint64 *> Point;
//	QList<qint8> PointSize;
//	QList<qint8> nbBits;
//	QList<CPObject *> PObject;
//	QList<QString> WatchPointName;
//    QList< QHash<int,QString> > Labels;
	
	void remove(CPObject * object)
	{
        for (int i=items.size()-1;i>=0;i--)
		{
            if (items.at(i).PObject == object)
			{
                items.removeAt(i);
//				Point.removeAt(i);
//				PointSize.removeAt(i);
//				nbBits.removeAt(i);
//				PObject.removeAt(i);
//				WatchPointName.removeAt(i);
//                Labels.removeAt(i);
			}
		}
    }
	
    void add( qint64 * watchpoint, qint8 ptrSize,qint8 nbbits,CPObject * object, QString name,QHash<int,QString> lblList = QHash<int,QString>())
    {
        CWatchPointItem * item = new CWatchPointItem(watchpoint,ptrSize,nbbits,object,name,lblList);
        items.append(*item);
        delete item;
//		Point.append(watchpoint);
//		PointSize.append(ptrSize);
//		nbBits.append(nbbits);
//		PObject.append(object);
//		WatchPointName.append(name);
//        Labels.append(lblList);
    }

    void remove( qint64 * watchpoint)
    {
        for (int i =0;i< items.size();i++) {
            if (items.at(i).Point == watchpoint) {
                items.removeAt(i);
                return;
            }
        }
//        int i = Point.indexOf(watchpoint);
//        Point.removeAt(i);
//        PointSize.removeAt(i);
//        nbBits.removeAt(i);
//        PObject.removeAt(i);
//        WatchPointName.removeAt(i);
//        Labels.remove.at(i);
    }
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

enum Models {
    EMPTY,

    PC1245,
    MC2200,
    PC1250,
    PC1251,
    PC1255,
    TandyPC3,
    TandyPC3EXT,
    PC1260,
    PC1261,
    PC1262,
    PC1280,
    PC1350,
    PC1360,
    PC1401,
    PC1402,
    PC1403,
    PC1421,
    PC1403H,
    PC1450,
    PC1475,
    PC1500,
    PC1500A,
    TandyPC2,
    PC1251H,
    PC1600,
    PC2500,

    CE120P,
    CE125,
    MP220,
    CE126P,
    CE123P,
    CE129P,
    CE140P,
    CE140F,
    CE150,
    CE152,
    TANDY263591,

    SerialConsole,
    CABLE11Pins,
    POTAR,
    Simulator,

    CE1600P,
    X07,
    X710,
    E500,
    PB1000,
    PB2000,
    MD100
};

#endif
