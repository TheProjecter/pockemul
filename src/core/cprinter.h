#ifndef _CPRINTER_H
#define _CPRINTER_H

#include <QColor>
#include "pobject.h"
#include "paperwidget.h"


class Cprinter:public CPObject{

//Q_OBJECT

public:
	virtual void clearPaper(void) {};
	virtual void SaveAsText(void) {};

	CpaperWidget *paperWidget;
	
	Cprinter(CPObject *parent);
	virtual ~Cprinter();

    virtual void resizeEvent ( QResizeEvent * );
	
	int paper_X,paper_Y;
    int paper_DX,paper_DY;
    QRect PaperWidgetRect;

	void setPaperPos(QPoint);
	QPoint PaperPos();
	
protected:
	QColor	PaperColor;
};


#endif
