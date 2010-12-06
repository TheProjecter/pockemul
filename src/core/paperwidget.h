#ifndef _PAPERWIDGET_H
#define _PAPERWIDGET_H

//#include "cprinter.h"
#include "pobject.h"

class CpaperWidget:public QWidget{
Q_OBJECT
public:

	QImage *bufferImage;
	CPObject * pPC;
	
	void setOffset(QPoint);
	QPoint getOffset();
    QRect baseRect;
	
	
	CpaperWidget(QRect rect,QImage * buffer,QWidget * parent):QWidget(parent)
	{
		pPC = (CPObject *)parent;
		bufferImage = buffer;

		setMouseTracking(true);
		setCursor(Qt::PointingHandCursor);
		resize(rect.width(),rect.height());
		move(rect.x(),rect.y());
		Offset = QPoint(0,0);
        this->baseRect = rect;
	}
	~CpaperWidget()
	{

	}
private slots:
	void contextMenuEvent ( QContextMenuEvent * event );
	void paperCopy();
	void paperCut();
	void paperSaveImage();
	void paperSaveText();

protected:
	void paintEvent(QPaintEvent *);
	
private:
	QPoint Offset;
};


#endif

