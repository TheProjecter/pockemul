#include <QPainter>
#include <QDebug>

#include "common.h"
#include "Log.h"
#include "postit.h"
#include "pcxxxx.h"

Cpostit::Cpostit(CPObject *parent )   : CPObject(this)
{                                                       //[constructor]


    setfrequency( 0);
    BackGroundFname     = P_RES(":/core/PostIt.png");

    setDXmm(85);
    setDYmm(318);
    setDZmm(51);
 // Ratio = 3,57
    setDX(150);//Pc_DX  = 75;
    setDY(150);//Pc_DY = 20;

}

Cpostit::~Cpostit(){

}


bool Cpostit::run(void)
{


    return true;
}




/*****************************************************************************/
/* Initialize                                                                                                                    */
/*****************************************************************************/
bool Cpostit::init(void)
{


    AddLog(LOG_MASTER,"PostIt initializing...");
    qWarning()<<"PostIt initializing...";
    CPObject::init();

    HBL = new QHBoxLayout();
    HBL->setMargin(20);
    this->setLayout(HBL);
    edit = new CpostitTextEdit(this);
//    edit->setTextBackgroundColor(Qt::transparent);
//    edit->setStyleSheet("background-color: rgba(255,0,0,0);");
    edit->viewport()->setAutoFillBackground(false);
    edit->setFrameStyle(QFrame::NoFrame);
//    edit->setContextMenuPolicy(Qt::CustomContextMenu);
    HBL->addWidget(edit);

    AddLog(LOG_MASTER,"done.\n");
    return true;
}

/*****************************************************************************/
/* Exit                                                                                                                                          */
/*****************************************************************************/
bool Cpostit::exit(void)
{
    return true;
}


void Cpostit::paintEvent(QPaintEvent *event)
{
    HBL->setMargin(20*mainwindow->zoom/100);
    CPObject::paintEvent(event);
}

void Cpostit::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    BuildContextMenu(&menu);

//    menu.addSeparator();

//    menu.addAction(tr("Show console"),this,SLOT(ShowConsole()));
//    menu.addAction(tr("Hide console"),this,SLOT(HideConsole()));

    menu.exec(event->globalPos () );
}

bool Cpostit::SaveSession_File(QXmlStreamWriter *xmlOut)
{
    xmlOut->writeStartElement("session");
        xmlOut->writeAttribute("version", "2.0");

        QByteArray ba(edit->toPlainText().toLatin1());
        xmlOut->writeTextElement("text",ba.toBase64());
    xmlOut->writeEndElement();  // session
    return true;
}

bool Cpostit::LoadSession_File(QXmlStreamReader *xmlIn)
{
    if (xmlIn->name()=="session") {
        if (xmlIn->readNextStartElement() && xmlIn->name() == "text" ) {
            QByteArray ba = QByteArray::fromBase64(xmlIn->readElementText().toLatin1());
            edit->setPlainText(QString(ba.data()));
        }
    }
    return true;
}
