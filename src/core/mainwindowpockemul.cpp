#include <QtGui>
#include <QString>
#include <QPainter>
 
#ifndef NO_SOUND
#include "fmod.h"
#endif

#include "mainwindowpockemul.h"

#include "dialoganalog.h"
#include "dialogabout.h"
#include "dialogstartup.h"
#include "dialoglog.h"
#include "dialogide.h"
#include "hexviewer.h"
#include "autoupdater.h"

#include "init.h"
#include "common.h"
#include "Log.h"
#include "pcxxxx.h"
#include "pc1500.h"
#include "clink.h"

//#include "lfhex/hexGui.h"

extern	MainWindowPockemul* mainwindow;

#define NBFRAMEPERSEC		20
#define FRAMERATE			(1000/NBFRAMEPERSEC)
#define TIMER_RES			5

QTime t,tf;
QTimer *timer;
QList<CPObject *> listpPObject; 
 

MainWindowPockemul::MainWindowPockemul( QWidget * parent, Qt::WFlags f) : QMainWindow(parent, f)
{
	rawclk = 0;
	setupUi(this);
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	dialoglog = 0;
	dialoganalogic = 0;
    dialogide = 0;
    zoom = 100;
    saveAll = false;
	startKeyDrag = false;
	startPosDrag = false;

    connect(actionAbout_PockEmul,	SIGNAL(triggered()), this, SLOT(about()));
    connect(actionNew,				SIGNAL(triggered()), this, SLOT(newsession()));
    connect(actionOpen,				SIGNAL(triggered()), this, SLOT(opensession()));
    connect(actionSave_As,              SIGNAL(triggered()),this,SLOT(saveassession()));
    connect(actionLog_Messages,		SIGNAL(triggered()), this, SLOT(Log()));
 	connect(actionAnalogic_Port,	SIGNAL(triggered()), this, SLOT(Analogic()));    
 	connect(actionCheck_for_Updates,SIGNAL(triggered()), this, SLOT(CheckUpdates()));    
    connect(actionMinimize_All,     SIGNAL(triggered()), this, SLOT(Minimize_All()));
    connect(actionReset_Zoom,           SIGNAL(triggered()),this, SLOT(resetZoom()));
    connect(menuPockets, SIGNAL(triggered( QAction *)), this, SLOT(SelectPocket( QAction *)));

    connect(actionEditor,SIGNAL(triggered()),this,SLOT(IDE()));

    pdirectLink = new CDirectLink;
 
#ifndef NO_SOUND
	FSOUND_Init(44100, 32, 0); 
#endif

//audio.show();


	// Create a timer for Drawing screen FRAMERATE times per seconds
	FrameTimer = new QTimer(mainwindow);
    connect(FrameTimer, SIGNAL(timeout()), this, SLOT(updateFrameTimer()));
	FrameTimer->start(FRAMERATE);
	
	timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
	timer->start(TIMER_RES);

	// Create the Pocket Thread
	PcThread = new CPocketThread(0);
	PcThread->connect(PcThread,SIGNAL(Resize(QSize,CPObject * )),this,SLOT(resizeSlot(QSize,CPObject * )));
	PcThread->connect(PcThread,SIGNAL(Destroy(CPObject * )),this,SLOT(DestroySlot(CPObject * )));
	PcThread->start();

}
//
void MainWindowPockemul::SendSignal_AddLogItem(QString str) { emit AddLogItem(str); }


void MainWindowPockemul::slotUnLink(Cconnector * conn) {
    int i;
    for ( i = 0 ; i< mainwindow->pdirectLink->AConnList.size() ; i++)
    {
        if (mainwindow->pdirectLink->AConnList.at(i) == conn)
        {
            mainwindow->pdirectLink->AConnList.removeAt(i);
            mainwindow->pdirectLink->BConnList.removeAt(i);
            i--;
        }
    }
    for ( i = 0 ; i< mainwindow->pdirectLink->BConnList.size() ; i++)
    {
        if (mainwindow->pdirectLink->BConnList.at(i) == conn)
        {
            mainwindow->pdirectLink->AConnList.removeAt(i);
            mainwindow->pdirectLink->BConnList.removeAt(i);
            i--;
        }
    }

}

void MainWindowPockemul::slotUnlink(CPObject * obj) {
    int i;
    for (i = 0;i < obj->ConnList.size(); i++)
    {
        slotUnLink(obj->ConnList.at(i));
    }
}

void MainWindowPockemul::slotUnLink(QAction * action)
{
    if (action->data().toString().startsWith("A")) {
        CPObject * obj = (CPObject*) action->data().toString().mid(1).toULongLong();
        slotUnlink(obj);
    }
    else if (action->data().toString().startsWith("C"))
        {
            Cconnector * conn = (Cconnector*) action->data().toString().mid(1).toULongLong();
            slotUnLink(conn);
        }
}

void MainWindowPockemul::slotWebLink(QAction *action) {
    QString s = action->data().toString();
    QUrl url(s);
    QDesktopServices::openUrl(url);
}

void MainWindowPockemul::slotNewLink(QAction * action)
{
	QString s = action->data().toString();
	QStringList list = s.split(":");
	
	qint64 l1 = list.at(0).toULongLong();
	qint64 l2 = list.at(1).toULongLong();
	
	Cconnector * p1 = (Cconnector*)l1;
	Cconnector * p2 = (Cconnector*)l2;
	
    mainwindow->pdirectLink->AConnList.append(p1);
    mainwindow->pdirectLink->BConnList.append(p2);
	
	AddLog(LOG_MASTER,tr("DirectLink Iinsert (%1,%2)").arg((qint64)p1).arg((qint64)p2));
}


int MainWindowPockemul::newsession()
{
    DialogStartup dialogstartup(this);
    int result = dialogstartup.exec();
    LoadPocket(result);
    return 1;
}

CPObject * MainWindowPockemul::LoadPocket(int result) {
    CPObject *newpPC;
    if (result)	{
                newpPC = InitApp(result);
                if (! newpPC) MSG_ERROR("pPC is NULL in slotStart")
                else
                {
                    AddLog(LOG_MASTER,tr("%1").arg((long)newpPC));
                                listpPObject.append(newpPC);

                    QAction * actionDistConn = menuPockets->addAction(newpPC->getName());
                    actionDistConn->setData(tr("%1").arg((long)newpPC));

                    if (dialoganalogic) {
                        dialoganalogic->fill_twWatchPoint();
                        dialoganalogic->update();
                    }
                    if (dialogide) {
                        dialogide->fill_inject();
                        dialogide->update();
                    }
                    return newpPC;
                }

        }
        else
                QMessageBox::about(this, tr("Attention"),"Please choose a pocket model or Cancel");

        return 0;

}

void MainWindowPockemul::Minimize_All() {
    // Fetch listpPObject and minimize if not minimized
    for (int k = 0; k < listpPObject.size(); k++)
    {
        CPObject *pc = listpPObject.at(k);
        if (pc->isFront()) {
            QMouseEvent *m = new QMouseEvent(QEvent::MouseButtonDblClick, QPoint(0,0), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
            pc->mouseDoubleClickEvent(m);
        }
    }

}

void MainWindowPockemul::resetZoom() {
    zoom = 100;
    update();
}

void MainWindowPockemul::SelectPocket(QAction * action) {
    if (action->data().isNull()) return;

    CPObject *pc = (CPObject*) action->data().toString().toULongLong();
    pc->raise();
    pc->setFocus();
    if (!pc->isFront()) {
        QMouseEvent *e=new QMouseEvent(QEvent::MouseButtonDblClick, QPoint(0,0), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(pc, e);
        delete e;
    }
}

void MainWindowPockemul::about()
{
	DialogAbout dialogabout(this);
	dialogabout.exec();
}

void MainWindowPockemul::Log()
{
        dialoglog = new DialogLog(this);
        dialoglog->show();
}

void MainWindowPockemul::IDE()
{
        dialogide = new DialogIDE(this);
        dialogide->show();
}

void MainWindowPockemul::Analogic()
{
        dialoganalogic = new dialogAnalog(11,this);
        dialoganalogic->setWindowTitle("Logic Analyser");
        dialoganalogic->show();
}

void MainWindowPockemul::CheckUpdates()
{
	CAutoUpdater dialogcheckupdate(this);
	dialogcheckupdate.exec();
}
    
void MainWindowPockemul::opensession()
{
    QHash<QString, Models> objtable;

    objtable["PC-1245"]=PC1245;
    objtable["MC-2200"]=MC2200;
    objtable["PC-1250"]=PC1250;
    objtable["PC-1251"]=PC1251;
    objtable["PC-1251H"]=PC1251H;
    objtable["PC-1255"]=PC1255;
    objtable["Tandy PC-3"]=TandyPC3;
    objtable["Tandy PC-3 (4Ko)"]=TandyPC3EXT;

    objtable["PC-1260"]=PC1260;
    objtable["PC-1261"]=PC1261;
    objtable["PC-1262"]=PC1262;
    objtable["PC-1280"]=PC1280;

    objtable["PC-1350"]=PC1350;
    objtable["PC-1360"]=PC1360;
    objtable["PC-1401"]=PC1401;
    objtable["PC-1402"]=PC1402;
    objtable["PC-1403"]=PC1403;
    objtable["PC-1403H"]=PC1403H;
    objtable["PC-1421"]=PC1421;
    objtable["PC-1450"]=PC1450;
    objtable["PC-1475"]=PC1475;

    objtable["PC-1500"]=PC1500;
    objtable["PC-1500A"]=PC1500A;
    objtable["Tandy PC-2"]=TandyPC2;

    objtable["PC-1600"]=PC1600;

    objtable["PC-2500"]=PC2500;

    objtable["CE-125"]=CE125;
    objtable["MP-220"]=MP220;
    objtable["CE-120P"]=CE120P;
    objtable["CE-126P"]=CE126P;
    objtable["CE-123P"]=CE123P;
    objtable["CE-129P"]=CE129P;
    objtable["CE-140P"]=CE140P;
    objtable["CE-140F"]=CE140F;
    objtable["CE-150"]= CE150;
    objtable["CE-152"]= CE152;
    objtable["26-3591"]=TANDY263591;

    objtable["Serial Console"]=SerialConsole;
    objtable["11Pins Cable"]=CABLE11Pins;
    objtable["Potar"]=POTAR;
    objtable["Simulator"]=Simulator;

    objtable["CE-1600P"]=CE1600P;


    QMap<int,CPObject*> map;

    QString fileName = QFileDialog::getOpenFileName(
                                                                            mainwindow,
                                                                            tr("Choose a file"),
                                                                            ".",
                                                                            tr("PockEmul sessions (*.pml)"));

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(mainwindow,tr("PockEmul"),
                                                            tr("Cannot read file %1:\n%2.")
                                                            .arg(file.fileName())
                                                            .arg(file.errorString()));
            return ;
    }

    QXmlStreamReader xml;

    xml.setDevice(&file);

    CPObject * firstPC = 0;

    if (xml.readNextStartElement()) {
        if (xml.name() == "pml" && xml.attributes().value("version") == "1.0") {
            zoom = xml.attributes().value("zoom").toString().toFloat();
            if (zoom==0)zoom=100;
            while (!xml.atEnd()) {
                while (xml.readNextStartElement()) {
                    QString eltname = xml.name().toString();
                    CPObject * locPC;
                    if (eltname == "object") {
                        QString name = xml.attributes().value("name").toString();
                        locPC = LoadPocket(objtable.value(name));
                        if (firstPC == 0) firstPC = locPC;      // Store the first pocket to manage stack
                        int id = xml.attributes().value("id").toString().toInt();
                        map.insert(id,locPC);
                        locPC->Front = (xml.attributes().value("front")=="true") ?true:false;

                        locPC->Power = (xml.attributes().value("power")=="true") ?true:false;
                        if (locPC->Power) locPC->TurnON();

                        while (xml.readNextStartElement()) {
                            QString eltname = xml.name().toString();
                            if (eltname == "position") {
                                QString posX = xml.attributes().value("x").toString();
                                QString posY = xml.attributes().value("y").toString();
                                locPC->setPosX(posX.toFloat());
                                locPC->setPosY(posY.toFloat());
                                if (locPC->Front) {
                                    locPC->setGeometry(posX.toFloat(),posY.toFloat(),locPC->Pc_DX*zoom/100,locPC->Pc_DY*zoom/100);
                                }
                                else {
                                    locPC->setGeometry(posX.toFloat(),posY.toFloat(),locPC->Pc_DX/4,locPC->Pc_DY/4);
                                }


                            }
                            else
                                xml.skipCurrentElement();
                        }
             //           xml.readNextStartElement();
                     }
                    else if (eltname == "link") {
                        int idpc1 = xml.attributes().value("idpcFrom").toString().toInt();
                        int idco1 = xml.attributes().value("idcoFrom").toString().toInt();
                        int idpc2 = xml.attributes().value("idpcTo").toString().toInt();
                        int idco2 = xml.attributes().value("idcoTo").toString().toInt();
                        CPObject * locpc1 = map.value(idpc1);
                        CPObject * locpc2 = map.value(idpc2);
                        Cconnector * locco1 = locpc1->ConnList.value(idco1);
                        Cconnector * locco2 = locpc2->ConnList.value(idco2);
                        mainwindow->pdirectLink->AConnList.append(locco1);
                        mainwindow->pdirectLink->BConnList.append(locco2);

                    }
                    else
                        xml.skipCurrentElement();


                }
            }
        }
        else
            xml.raiseError(QObject::tr("The file is not an XBEL version 1.0 file."));
    }
    for (int i=0;i <listpPObject.size();i++) {
        QMouseEvent *e=new QMouseEvent(QEvent::MouseButtonPress, QPoint(0,0), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(listpPObject.at(i), e);
        QMouseEvent *e2=new QMouseEvent(QEvent::MouseButtonRelease, QPoint(0,0), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(listpPObject.at(i), e2);
        delete e;
        delete e2;
    }
}

void MainWindowPockemul::saveassession()
{
    QMap<CPObject*,int> map;

    saveAll = true;
    QString s;
    QXmlStreamWriter *xml = new QXmlStreamWriter(&s);
    xml->autoFormatting();
    xml->writeStartElement("pml");
    xml->writeAttribute("version", "1.0");
    xml->writeAttribute("zoom",QString("%1").arg(zoom));

    // Fetch all objects
    for (int i=0;i<listpPObject.size();i++)
    {
        CPObject *po = listpPObject.at(i);
        map.insert(po,i);
        po->serialize(xml,i);
    }

    // Connectors
    //xml->writeStartElement("links");

    // fetch AConnList
    for (int j = 0;j < mainwindow->pdirectLink->AConnList.size(); j++)
    {
        int idpc1 = map.value(mainwindow->pdirectLink->AConnList.at(j)->Parent);
        int idco1 = mainwindow->pdirectLink->AConnList.at(j)->Parent->ConnList.indexOf(mainwindow->pdirectLink->AConnList.at(j));
                //mainwindow->pdirectLink->AConnList.at(j)->Id;
        int idpc2 = map.value(mainwindow->pdirectLink->BConnList.at(j)->Parent);
        //int idco2 = mainwindow->pdirectLink->BConnList.at(j)->Id;
        int idco2 = mainwindow->pdirectLink->BConnList.at(j)->Parent->ConnList.indexOf(mainwindow->pdirectLink->BConnList.at(j));

        xml->writeStartElement("link");
        xml->writeAttribute("idpcFrom",QString("%1").arg(idpc1));
        xml->writeAttribute("idcoFrom",QString("%1").arg(idco1));
        xml->writeAttribute("idpcTo",QString("%1").arg(idpc2));
        xml->writeAttribute("idcoTo",QString("%1").arg(idco2));
        xml->writeEndElement();
    }



    //xml->writeEndElement();  // links

    xml->writeEndElement();  // pml
    MSG_ERROR(s)

            QString fn = QFileDialog::getSaveFileName(
                    mainwindow,
                    tr("Choose a filename to save session"),
                    ".",
                   tr("Session File (*.pml)"));

    QFile f(fn);
    if (f.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&f);
        out << s;
    }

    saveAll = false;
}

void MainWindowPockemul::paintEvent(QPaintEvent *event) {}

void MainWindowPockemul::updateTimer()
{
	static int deltaTime = -1;
	
	if (deltaTime == -1) {	t.start(); }
	deltaTime = t.restart();	
	
	rawclk += deltaTime;

}

void MainWindowPockemul::wheelEvent(QWheelEvent *event) {
    QPoint point = event->pos();

    float delta = event->delta()/12;

    if (((zoom >= 20) && (delta<0)) ||
        ((zoom <300) && (delta >0))){
        int d = (delta>0) ? 10 : -10;
        delta = ((zoom+d)/zoom - 1)*100;
        zoom += d;
    }
    else delta = 0;


    this->setWindowTitle(QString("Delta=%1  zoom=%2").arg(delta).arg(zoom));

    for (int i=0;i<listpPObject.size();i++) {
        CPObject * locpc = listpPObject.at(i);

        // calculate the new origine
        float newposx = locpc->posx() + (locpc->posx()-point.x())*(delta)/100.0;
        float newposy = locpc->posy() + (locpc->posy()-point.y())*(delta)/100.0;

        locpc->setPosX(newposx);
        locpc->setPosY(newposy);
        locpc->setGeometry(newposx,newposy,locpc->Pc_DX*zoom/100,locpc->Pc_DY*zoom/100);
    }
}

void  MainWindowPockemul::updateTimeTimer()
{
}

void  MainWindowPockemul::updateFrameTimer()
{

	int statepersec;
	int rate=0;
	static int OneSecTimer=0;
	long Current_State;

// Calculate emulation speed
// Normally each frame equal pPC->frequency state / NBFRAMEPERSEC


	if (! listpPObject.size()) return;
		
	static int deltaTime = -1;
	
	if (deltaTime == -1) {	tf.start(); }
	
	deltaTime = tf.elapsed();
	if (deltaTime >= 1000) tf.restart();
		
 	for (int i = 0;i < listpPObject.size(); i++)
 	{
            CPObject* CurrentpPC = listpPObject.at(i);
            if (CurrentpPC && CurrentpPC->pTIMER) {
                Current_State = CurrentpPC->pTIMER->state;

                CurrentpPC->pTIMER->nb_state += (Current_State - CurrentpPC->pTIMER->last_state);
                CurrentpPC->pTIMER->last_state = Current_State;

                // Update ToolTip only one time per second
                if ( deltaTime >= 1000)
                {
                    QString str;
                    if (CurrentpPC->getfrequency()) {
                        //	AddLog(LOG_TIME,tr("Time Frame elapsed : %1 ms  nb=%2 cur=%3 last=%4").arg(deltaTime).arg(CurrentpPC->pTIMER->nb_state).arg(Current_State).arg(CurrentpPC->pTIMER->last_state));
                        statepersec = (int) ( CurrentpPC->getfrequency());
                        rate = (int) ((100L*CurrentpPC->pTIMER->nb_state)/((statepersec/1000)*deltaTime));
                        CurrentpPC->pTIMER->nb_state=0;

                        str.setNum((int)rate);
                        str = ": "+str+tr("% original speed");
                    }
                    CurrentpPC->setToolTip(CurrentpPC->getName()+str);
                }

                if (CurrentpPC->pLCDC)
                {
                            CurrentpPC->pLCDC->disp();
                            if (CurrentpPC->pLCDC->Refresh) CurrentpPC->Refresh_Display = true;
                }
                if ( CurrentpPC->Refresh_Display) {
                    CurrentpPC->update();
                    CurrentpPC->Refresh_Display= false;
                }
            }
        }
	
	if (OneSecTimer >= 1000) OneSecTimer=0;
}

void MainWindowPockemul::mousePressEvent	( QMouseEvent *event){
    setCursor(Qt::ClosedHandCursor);	// Change mouse pointer
    startPosDrag = true;
    PosDrag = event->globalPos();
}

void MainWindowPockemul::mouseMoveEvent		( QMouseEvent * event ){
    if (startPosDrag)
    {
        QPoint delta(event->globalPos() - PosDrag);

        // Fetch all_object and move them
        for (int i=0;i<listpPObject.size();i++)
        {
            listpPObject.at(i)->Move(delta);
        }

        PosDrag = event->globalPos();
        repaint();
        return;
    }
}

void MainWindowPockemul::mouseReleaseEvent	( QMouseEvent *event){
    startPosDrag = false;
    setCursor(Qt::ArrowCursor);

}


void MainWindowPockemul::keyReleaseEvent	( QKeyEvent * event ){}
void MainWindowPockemul::keyPressEvent		( QKeyEvent * event ){}
void MainWindowPockemul::resizeEvent		( QResizeEvent * event ){}

void MainWindowPockemul::resizeSlot( QSize size , CPObject *pObject)
{
	delete(pObject->FinalImage);
	
	int l = menuBar()->height();
	pObject->FinalImage = new QImage(size-QSize(0,l),QImage::Format_RGB32);
}

void MainWindowPockemul::DestroySlot( CPObject *pObject)
{

    QList< QAction *> actionList = menuPockets->actions();
    for (int i=0; i< actionList.size();i++) {
        QAction* action = actionList.at(i);
        if (! action->data().isNull()) {
            CPObject *pc = (CPObject*) action->data().toString().toULongLong();
            if (pc == pObject) {
                 menuPockets->removeAction(action);
            }
        }
    }

    WatchPoint.remove(pObject);
    if (dialoganalogic) {
        dialoganalogic->fill_twWatchPoint();
        dialoganalogic->update();
    }
    pObject->exit();
	pObject->deleteLater();
}


void MainWindowPockemul::closeEvent(QCloseEvent *event)
{

}
