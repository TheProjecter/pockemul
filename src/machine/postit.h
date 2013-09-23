#ifndef POSTIT_H
#define POSTIT_H

#include <QDebug>
#include <QTextEdit>
#include <QContextMenuEvent>

#include "pobject.h"


class QHBoxLayout;

class CpostitTextEdit:public QTextEdit {
public:
    CpostitTextEdit(QWidget * parent):QTextEdit(parent){}

    void contextMenuEvent ( QContextMenuEvent * event )
    {
        QMenu *menupocket = QTextEdit::createStandardContextMenu();
        menupocket->addSeparator();
        menupocket->addAction(tr("Double Size"),parentWidget(),SLOT(slotDblSize()));
        menupocket->addAction(tr("Reset"));//,this,SLOT(slotResetNow()));



    //    menu->setStyleSheet("QMenu { color: black }");
        //menu->exec(event->globalPos () );
    menupocket->popup(event->globalPos () );
        event->accept();
    }


};

class Cpostit:public CPObject {
    Q_OBJECT
public:
    const char*	GetClassName(){ return("Cpostit");}

    bool run(void);
    bool init(void);				//initialize
    bool exit(void);				//end
    virtual bool    SaveSession_File(QXmlStreamWriter *xmlOut);
    virtual bool	LoadSession_File(QXmlStreamReader *xmlIn);

    Cpostit(CPObject *parent = 0);
    virtual ~Cpostit();

protected:
    void paintEvent(QPaintEvent *);
protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
    void slotDblSize();

private:
    CpostitTextEdit *edit;
    QHBoxLayout *HBL;
    float size;

};

#endif // POSTIT_H
