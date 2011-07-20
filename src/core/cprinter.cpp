#include <QMenu>
#include <QClipboard>
#include <QFileDialog>

#include "cprinter.h"
#include "paperwidget.h"

Cprinter::Cprinter(CPObject *parent):CPObject(parent)
{
	PaperColor = QColor(255,255,255);
	paperWidget = 0;
    pos=QRect(0,0,0,0);

}

Cprinter::~Cprinter()
{
	delete paperWidget;
}

void Cprinter::resizeEvent ( QResizeEvent * ) {
    float ratio = (float)this->width()/getDX() ;

    QRect rect = this->paperWidget->baseRect;
    this->paperWidget->setGeometry( rect.x()*ratio,
                                    rect.y()*ratio,
                                    rect.width()*ratio,
                                    rect.height()*ratio);
}


void Cprinter::setPaperPos(QRect pos)
{
    this->pos = pos;
}

QRect Cprinter::PaperPos()
{
    return(pos);
}
