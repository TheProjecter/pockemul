#ifndef _CPRINTER_H
#define _CPRINTER_H

#include <QColor>
#include "pobject.h"
class CpaperWidget;


class Cprinter:public CPObject{
Q_OBJECT
public:
	virtual void clearPaper(void) {};
	virtual void SaveAsText(void) {};
	CpaperWidget *paperWidget;
	
	Cprinter(CPObject *parent);
	virtual ~Cprinter();
	
	int paper_X,paper_Y;
	void setPaperPos(QPoint);
	QPoint PaperPos();
	
protected:
	QColor	PaperColor;
};


#endif
