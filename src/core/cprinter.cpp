#include <QMenu>
#include <QClipboard>
#include <QFileDialog>

#include "cprinter.h"
#include "paperwidget.h"
#include "Log.h"

//TODO  Lorsque pointeur positionné sur papier, afficher scroolbar verticale et gerer mousewheel
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
    this->paperWidget->updated=true;
}

void Cprinter::moveEvent ( QMoveEvent * event ) {
    this->paperWidget->updated=true;
}

void Cprinter::setPaperPos(QRect pos)
{
    this->pos = pos;
}

QRect Cprinter::PaperPos()
{
    return(pos);
}

void Cprinter::raise()
{
    paperWidget->updated = true;
    AddLog(LOG_TEMP,"RAISE");
    CPObject::raise();

}
