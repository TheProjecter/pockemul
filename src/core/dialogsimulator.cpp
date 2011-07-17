#include <QTextCursor>
#include <QCloseEvent>

#include "dialogsimulator.h"
#include "common.h"
#include "pcxxxx.h"


//
DialogSimulator::DialogSimulator( QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
    setupUi(this);
    setFocusPolicy(Qt::StrongFocus);

//    connect(pbSend, SIGNAL(clicked()), this, SLOT(sendData()));
//    connect(baudCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeBaudrate(QString)));

    currentIndex=0;

}
//
void DialogSimulator::refresh( void)
{



}



void DialogSimulator::closeEvent(QCloseEvent *event)
{
    event->accept();
}

