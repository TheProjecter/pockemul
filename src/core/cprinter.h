#ifndef _CPRINTER_H
#define _CPRINTER_H

#include <QColor>
#include "pobject.h"

class CpaperWidget;

class Cprinter:public CPObject{

Q_OBJECT

public:
    virtual void clearPaper(void) {}
    virtual void SaveAsText(void) {}

	CpaperWidget *paperWidget;
	
	Cprinter(CPObject *parent);
	virtual ~Cprinter();

    virtual void resizeEvent ( QResizeEvent * );
	
    QRect pos;
    QRect PaperWidgetRect;

    void setPaperPos(QRect);
    QRect PaperPos();
	
protected:
	QColor	PaperColor;
};


#endif
