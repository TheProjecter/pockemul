#include <QtGui>
#include <QString>
#include <QPainter>
#include <iostream>
#include <QtNetwork>


/** \mainpage
PockEmul is a Sharp Pocket Computer Emulator.

\version Version 0.9.3
\image html hexedit.png
*/




#include "qcommandline.h"

#include "mainwindowpockemul.h"

#ifdef P_AVOID
#include "libavoid.h"
#endif

#include "launchbuttonwidget.h"
#include "dialoganalog.h"
#include "dialogabout.h"
#include "dialogstartup.h"
#include "dialoglog.h"

#ifdef P_IDE
#include "ui/windowide.h"
#endif

#include "hexviewer.h"
#include "autoupdater.h"

#include "init.h"
#include "common.h"
#include "Log.h"
#include "pcxxxx.h"
#include "Connect.h"
#include "Inter.h"
#include "Lcdc.h"
#include "clink.h"
#include "downloadmanager.h"
#include "servertcp.h"


extern MainWindowPockemul* mainwindow;
extern DownloadManager *downloadManager;


#define NBFRAMEPERSEC		20
#define FRAMERATE			(1000/NBFRAMEPERSEC)
#define TIMER_RES			20

QTime t,tf;
QElapsedTimer et;
QTimer *timer;
QList<CPObject *> listpPObject;


MainWindowPockemul::MainWindowPockemul( QWidget * parent, Qt::WFlags f) : QMainWindow(parent, f)
{
    rawclk = 0;
    setupUi(this);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setStatusBar(0);
    dialoglog = 0;
    dialoganalogic = 0;
#ifdef P_IDE
    windowide = 0;
#endif
    scaleFactor = 1;
    zoom = 100;
    saveAll = ASK;
    startKeyDrag = false;
    startPosDrag = false;

    connect(actionAbout_PockEmul,	SIGNAL(triggered()),            this, SLOT(about()));
    connect(actionNew,				SIGNAL(triggered()),            this, SLOT(newsession()));
    connect(actionOpen,				SIGNAL(triggered()),            this, SLOT(opensession()));
    connect(actionSave_As,          SIGNAL(triggered()),            this, SLOT(saveassession()));
    connect(actionLog_Messages,		SIGNAL(triggered()),            this, SLOT(Log()));
    connect(actionAnalogic_Port,	SIGNAL(triggered()),            this, SLOT(Analogic()));
    connect(actionCheck_for_Updates,SIGNAL(triggered()),            this, SLOT(CheckUpdates()));
    connect(actionMinimize_All,     SIGNAL(triggered()),            this, SLOT(Minimize_All()));
    connect(actionReset_Zoom,       SIGNAL(triggered()),            this, SLOT(resetZoom()));
    connect(actionClose_All,        SIGNAL(triggered()),            this, SLOT(Close_All()));
    connect(menuPockets,            SIGNAL(triggered(QAction *)),   this, SLOT(SelectPocket( QAction *)));
    connect(actionEditor,           SIGNAL(triggered()),            this, SLOT(IDE()));

    pdirectLink = new CDirectLink;

    // Create a timer for Drawing screen FRAMERATE times per seconds
    FrameTimer = new QTimer(mainwindow);
    connect(FrameTimer, SIGNAL(timeout()), this, SLOT(updateFrameTimer()));
    FrameTimer->start(FRAMERATE);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    timer->start(TIMER_RES);

    // Create the Pocket Thread
    PcThread = new CPocketThread(this);
    PcThread->connect(PcThread,SIGNAL(Resize(QSize,CPObject * )),this,SLOT(resizeSlot(QSize,CPObject * )));
    PcThread->connect(PcThread,SIGNAL(Destroy(CPObject * )),this,SLOT(DestroySlot(CPObject * )));
#ifndef EMSCRIPTEN
    PcThread->start();
#endif

    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);

    initObjectTable();
#ifdef AVOID
    router = new Avoid::Router(Avoid::OrthogonalRouting);
    router->setRoutingPenalty((Avoid::PenaltyType)0, 50);
    router->setOrthogonalNudgeDistance(15);
#endif

#ifndef EMSCRIPTEN
server = new ServeurTcp(this);
#endif


qWarning("create");

}

MainWindowPockemul::~MainWindowPockemul() {
    qWarning("DELETE dialoglog");
    delete dialoglog;
    delete dialoganalogic;
#ifdef P_IDE
    delete windowide;
#endif
    delete FrameTimer;
    delete pdirectLink;

}

void MainWindowPockemul::initObjectTable() {
    objtable["PC-1211"]=PC1211;
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
    objtable["PC-1425"]=PC1425;

    objtable["PC-1500"]=PC1500;
    objtable["PC-1500A"]=PC1500A;
    objtable["Tandy PC-2"]=TandyPC2;

    objtable["PC-1600"]=PC1600;

    objtable["PC-2500"]=PC2500;

    objtable["CE-122"]=CE122;
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
    objtable["CE-127R"]= CE127R;
    objtable["26-3591"]=TANDY263591;

    objtable["Serial Console"]=SerialConsole;
    objtable["11Pins Cable"]=CABLE11Pins;
    objtable["Potar"]=POTAR;
    objtable["Simulator"]=Simulator;

    objtable["CE-1600P"]=CE1600P;
    objtable["Canon X-07"]=X07;
    objtable["Canon X-710"]=X710;
    objtable["PC-E500"]=E500;
    objtable["PC-E550"]=E550;
    objtable["PC-G850V"]=G850V;
    objtable["Casio PB-1000"]=PB1000;
    objtable["Casio PB-2000C"]=PB2000;
    objtable["Casio MD-100"]=MD100;
    objtable["Casio FP-100"]=FP100;
    objtable["Casio FP-40"]=FP40;
    objtable["Casio FP-200"]=FP200;
    objtable["Casio FX-890P"]=FX890P;
    objtable["Casio Z-1"]=Z1;
    objtable["Casio Z-1GR"]=Z1GR;
    objtable["Nec PC-2001"]=PC2001;
    objtable["Nec PC-2021"]=PC2021;
    objtable["Nec PC-2081"]=PC2081;
    objtable["General LBC-1100"]=LBC1100;
    objtable["General CL-1000"]=CL1000;
    objtable["SANCO TPC-8300"]=TPC8300;
    objtable["SANCO TP-83"]=TP83;
    objtable["Panasonic HHC RL-H1000"]=RLH1000;
    objtable["Panasonic RL-P6001"]=RLP6001;
    objtable["Panasonic RL-P9001"]=RLP9001;
    objtable["Panasonic RL-P1004A"]=RLP1004A;
}

//
void MainWindowPockemul::SendSignal_AddLogItem(QString str) {
    emit AddLogItem(str);
}

void MainWindowPockemul::SendSignal_AddLogConsole(QString str) {
    emit AddLogConsole(str);
}

void MainWindowPockemul::slotUnLink(Cconnector * conn) {
    int i;
    for ( i = 0 ; i< mainwindow->pdirectLink->AConnList.size() ; i++)
    {
        if (mainwindow->pdirectLink->AConnList.at(i) == conn)
        {
            mainwindow->pdirectLink->removeLink(i);
            i--;
        }
    }
    for ( i = 0 ; i< mainwindow->pdirectLink->BConnList.size() ; i++)
    {
        if (mainwindow->pdirectLink->BConnList.at(i) == conn)
        {
            mainwindow->pdirectLink->removeLink(i);
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
#define POCKEMUL_DOCUMENTS_URL "http://pockemul.free.fr/Documents/userguide/"

void MainWindowPockemul::slotDocument(QAction *action) {

#ifdef EMSCRIPEN
    QString s = action->data().toString();
    QUrl url(s);
    QDesktopServices::openUrl(url);
#else
    // Check if document is already downloaded
    // if yes open it
    // if no propose to download it : display the size ?

    QString fn=QDir::homePath()+"/pockemul/documents/"+QFileInfo(action->data().toString()).fileName();
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly)) {

        switch(QMessageBox::question(
                   mainwindow,
                   "PockEmul",
                   tr("The document %1 is not available locally. Do you want do download it ?").
                   arg(action->data().toString()),
                   QMessageBox::Yes | QMessageBox::No )) {
        case QMessageBox::Yes: break;
        case QMessageBox::No: return;
        default: return;
        }
        // Download it
        QUrl url(action->data().toString());
        downloadManager->doDownload(url);
    }
    QUrl url(fn);
    QDesktopServices::openUrl(url);
#endif
}

void MainWindowPockemul::slotNewLink(QAction * action)
{
    QString s = action->data().toString();
    QStringList list = s.split(":");

    qint64 l1 = list.at(0).toULongLong();
    qint64 l2 = list.at(1).toULongLong();

    Cconnector * p1 = (Cconnector*)l1;
    Cconnector * p2 = (Cconnector*)l2;

    mainwindow->pdirectLink->addLink(p1,p2,false);

    AddLog(LOG_MASTER,tr("DirectLink Iinsert (%1,%2)").arg((qint64)p1).arg((qint64)p2));
}

int MainWindowPockemul::newsession()
{
    DialogStartup dialogstartup(this);
    int result = dialogstartup.exec();
    LoadPocket(result);
    return 1;
}

CPObject * MainWindowPockemul::LoadPocket(QString Id) {
    return LoadPocket(objtable.value(Id));
}

CPObject * MainWindowPockemul::LoadPocket(int result) {
    qWarning()<<"Load Pocket:"<<result;
    CPObject *newpPC;
    if (result)	{
                newpPC = InitApp(result);
                if (! newpPC) {
 //                   MSG_ERROR("pPC is NULL in slotStart")
                }
                else
                {
                    AddLog(LOG_MASTER,tr("%1").arg((long)newpPC));
                                listpPObject.append(newpPC);

                    QAction * actionDistConn = menuPockets->addAction(newpPC->getName());
                    actionDistConn->setData(tr("%1").arg((long)newpPC));
                    QMenu *ctxMenu = new QMenu(newpPC);
                    newpPC->BuildContextMenu(ctxMenu);
                    actionDistConn->setMenu(ctxMenu);

                    emit NewPObjectsSignal(newpPC);

                    if (dialoganalogic) {
                        dialoganalogic->fill_twWatchPoint();
                        dialoganalogic->update();
                    }
#ifdef P_IDE
                    if (windowide) {
                        windowide->addtargetCB(newpPC);
                        windowide->update();
                    }
#endif
                    return newpPC;
                }

        }
        else
                QMessageBox::about(this, tr("Attention"),"Please choose a pocket model or Cancel");

        return 0;

}

//TODO Minimize should be transform to fit all pocket into screen
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

void MainWindowPockemul::Close_All() {
#ifdef EMSCRIPTEN
    saveAll = NO;
#else
    switch(QMessageBox::question(mainwindow,
                                 "PockEmul",
                                 "Do you want to save all sessions ?",
                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)) {
    case QMessageBox::Yes: saveAll = YES;break;
    case QMessageBox::No: saveAll = NO;break;
    case QMessageBox::Cancel: return;
    default: return;
    }
#endif

    for (int k = 0; k < listpPObject.size(); k++)
    {
        CPObject *pc = listpPObject.at(k);
        pc->slotExit();
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
    DialogAbout *dialogabout = new DialogAbout(this);
    dialogabout->setModal(true);
    dialogabout->show();
}

void MainWindowPockemul::Log()
{
        dialoglog = new DialogLog(this);
        dialoglog->show();
}

void MainWindowPockemul::IDE()
{
#ifdef P_IDE
        if (windowide==0) windowide = new WindowIDE(this);
        windowide->show();
#endif
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
    dialogcheckupdate.show();
}

void MainWindowPockemul::opensession(QString sessionFN)
{



    QMap<int,CPObject*> map;

    if (sessionFN=="") {
        sessionFN = QFileDialog::getOpenFileName(
                mainwindow,
                tr("Choose a file"),
                ".",
                tr("PockEmul sessions (*.pml)"));
    }

    QFile file(sessionFN);
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
                        if (locPC->Power) {

                            locPC->TurnON();
                        }

                        while (xml.readNextStartElement()) {
                            QString eltname = xml.name().toString();
//                            AddLog(LOG_TEMP,eltname);
                            if (eltname == "position") {
                                QString posX = xml.attributes().value("x").toString();
                                QString posY = xml.attributes().value("y").toString();
                                locPC->setPosX(posX.toFloat());
                                locPC->setPosY(posY.toFloat());
                                if (locPC->Front) {
//                                    locPC->setGeometry(posX.toFloat(),posY.toFloat(),locPC->getDX()*zoom/100,locPC->getDY()*zoom/100);
//                                    locPC->setMask(locPC->mask.scaled(locPC->getDX()*zoom/100,locPC->getDY()*zoom/100).mask());
                                    locPC->changeGeometry(posX.toFloat(),posY.toFloat(),locPC->getDX()*zoom/100,locPC->getDY()*zoom/100);
                                }
                                else {
                                    locPC->setGeometry(posX.toFloat(),posY.toFloat(),locPC->getDX()/4,locPC->getDY()/4);

                                }
                                xml.skipCurrentElement();

                            }
                            else
                            if (eltname == "session") {
//                                AddLog(LOG_MASTER,"OK");
//                                ((CpcXXXX*)locPC)->LoadSession_File(&xml);
                                locPC->LoadSession_File(&xml);
                                xml.skipCurrentElement();
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
                        bool close= (xml.attributes().value("close")=="false") ?false:true;
                        CPObject * locpc1 = map.value(idpc1);
                        CPObject * locpc2 = map.value(idpc2);
                        Cconnector * locco1 = locpc1->ConnList.value(idco1);
                        Cconnector * locco2 = locpc2->ConnList.value(idco2);
                        mainwindow->pdirectLink->addLink(locco1,locco2,close);

                    }
                    else
                        xml.skipCurrentElement();


                }
            }
        }
        else
            xml.raiseError(QObject::tr("The file is not a PML version 1.0 file."));
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

    // Take a snapshot

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    QPixmap::grabWidget(this).toImage().scaled(QSize(200,200),Qt::KeepAspectRatio,Qt::SmoothTransformation).save(&buffer, "PNG");

    saveAll = YES;
    QString s;
    QXmlStreamWriter *xml = new QXmlStreamWriter(&s);
    xml->setAutoFormatting(true);
    xml->writeStartElement("pml");
    xml->writeAttribute("version", "1.0");
    xml->writeAttribute("zoom",QString("%1").arg(zoom));
    xml->writeTextElement("snapshot",ba.toBase64());
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
        bool close = mainwindow->pdirectLink->closeList.at(j);

        xml->writeStartElement("link");
        xml->writeAttribute("idpcFrom",QString("%1").arg(idpc1));
        xml->writeAttribute("idcoFrom",QString("%1").arg(idco1));
        xml->writeAttribute("idpcTo",QString("%1").arg(idpc2));
        xml->writeAttribute("idcoTo",QString("%1").arg(idco2));
        xml->writeAttribute("close",QString("%1").arg(close?"true":"false"));
        xml->writeEndElement();
    }



    //xml->writeEndElement();  // links

    xml->writeEndElement();  // pml
    //MSG_ERROR(s)

    QString fn = QFileDialog::getSaveFileName(
                mainwindow,
                tr("Choose a filename to save session"),
                ".",
                tr("Session File (*.pml)"));


    QFileInfo fi( fn );
            if (fi.suffix().isEmpty())
            {
                    // no suffix, adding .pml  - BUG For Android
                fn.append(".pml");
            }
    QFile f(fn);
    if (f.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&f);
        out << s;
    }

    saveAll = ASK;

    delete xml;
}




void MainWindowPockemul::paintEvent(QPaintEvent *event) {}

void MainWindowPockemul::updateTimer()
{
    static int deltaTime = -1;

    if (deltaTime == -1) {	t.start();}
    deltaTime = t.restart();

    rawclk += deltaTime;//*1000000L;
    //AddLog(LOG_TEMP,tr("temps:%1").arg(deltaTime));
#ifdef EMSCRIPTEN
    PcThread->run();
#endif

}

void MainWindowPockemul::doZoom(QPoint point,float delta,int step) {
//    qWarning()<<"mainwindow::doZoom";
    if (((zoom >= 20) && (delta<0)) ||
        ((zoom <300) && (delta >0))){
        int d = (delta>0) ? step : -step;
        delta = ((zoom+d)/zoom - 1)*100;
        zoom += d;

    }
    else delta = 0;


    this->setWindowTitle(QString("  Zoom=%2%").arg(zoom));

    for (int i=0;i<listpPObject.size();i++) {
        CPObject * locpc = listpPObject.at(i);

        // calculate the new origine

        float newposx = locpc->posx() + (locpc->posx()-point.x())*(delta)/100.0;
        float newposy = locpc->posy() + (locpc->posy()-point.y())*(delta)/100.0;

        locpc->changeGeometry(newposx,
                              newposy,
                              locpc->getDX()*zoom/100/(locpc->Front?1:4),
                              locpc->getDY()*zoom/100/(locpc->Front?1:4));

    }
}

void MainWindowPockemul::wheelEvent(QWheelEvent *event) {
    QPoint point = event->pos();

    float delta = event->delta()/12;

    doZoom(point,delta);

}

bool MainWindowPockemul::event(QEvent *event)
 {
     if (event->type() == QEvent::Gesture)
         return gestureEvent(static_cast<QGestureEvent*>(event));
     return QWidget::event(event);
 }

bool MainWindowPockemul::gestureEvent(QGestureEvent *event)
{
//    if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
//        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
//    else
//    if (QGesture *pan = event->gesture(Qt::PanGesture))
//        panTriggered(static_cast<QPanGesture *>(pan));
    if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
        startPosDrag = false;
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
        event->accept();
    }
    return true;
}

//void MainWindowPockemul::panTriggered(QPanGesture *gesture)
// {
// #ifndef QT_NO_CURSOR
//     switch (gesture->state()) {
//         case Qt::GestureStarted:
//         case Qt::GestureUpdated:
//             setCursor(Qt::SizeAllCursor);
//             break;
//         default:
//             setCursor(Qt::ArrowCursor);
//     }
// #endif
//     QPointF delta = gesture->delta();
//     horizontalOffset += delta.x();
//     verticalOffset += delta.y();
//     update();
// }

 void MainWindowPockemul::pinchTriggered(QPinchGesture *gesture)
 {
     QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
//     if (changeFlags & QPinchGesture::RotationAngleChanged) {
//         qreal value = gesture->property("rotationAngle").toReal();
//         qreal lastValue = gesture->property("lastRotationAngle").toReal();
//         rotationAngle += value - lastValue;
//     }
     if (changeFlags & QPinchGesture::ScaleFactorChanged) {
         qreal value = gesture->scaleFactor();//property("scaleFactor").toReal();
         scaleFactor *= value;
         if ((scaleFactor >=1.1) || (scaleFactor <=.9)) {
             doZoom(gesture->centerPoint().toPoint(),(scaleFactor*100.0)-100);
             scaleFactor = 1;
             update();
         }
     }
     if (gesture->state() == Qt::GestureFinished) {
         scaleFactor = 1;

         update();
     }

 }

void MainWindowPockemul::updateTimeTimer()
{
}

void MainWindowPockemul::updateFrameTimer()
{

    int statepersec;
    int rate=0;
    static int OneSecTimer=0;
    quint64 Current_State;

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
                        CurrentpPC->rate = rate;
                        str.setNum((int)rate);
                        str = ": "+str+tr("% original speed");
                    }
#ifndef Q_OS_ANDROID
                    CurrentpPC->setToolTip(CurrentpPC->getName()+str);
#endif

                }

//                bool disp_on = true;
#if 1
                if (CurrentpPC->pLCDC)
                {
                    if (dynamic_cast<CpcXXXX *>(CurrentpPC) )
                    {
//                        CpcXXXX *tmpPC = (CpcXXXX*)CurrentpPC;
                        //if (tmpPC->getDisp_on())
                        {
                            CurrentpPC->pLCDC->disp();
                            if (CurrentpPC->pLCDC->Refresh) CurrentpPC->Refresh_Display = true;
                        }
                    }
                }
#endif
                if ( CurrentpPC->Refresh_Display) {
//                   AddLog(LOG_DISPLAY,tr("Refresh Display"));
                    CurrentpPC->update();
                    CurrentpPC->Refresh_Display= false;
                }
            }
        }

    if (OneSecTimer >= 1000) OneSecTimer=0;
}

void MainWindowPockemul::mousePressEvent	( QMouseEvent *event){
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }
    setCursor(Qt::ClosedHandCursor);	// Change mouse pointer
    startPosDrag = true;
    PosDrag = event->globalPos();
    event->accept();
}

void MainWindowPockemul::MoveAll(QPoint p) {
    // Fetch all_object and move them
    for (int i=0;i<listpPObject.size();i++)
    {
        listpPObject.at(i)->Move(p);
    }
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
        update();
        return;
    }
}

void MainWindowPockemul::mouseReleaseEvent	( QMouseEvent *event){
    startPosDrag = false;
    setCursor(Qt::ArrowCursor);

}

void MainWindowPockemul::keyReleaseEvent	( QKeyEvent * event ){}

void MainWindowPockemul::keyPressEvent		( QKeyEvent * event ){
    event->ignore();
}

void MainWindowPockemul::resizeEvent		( QResizeEvent * event ){
#ifndef EMSCRIPTEN
    downloadManager->resize();
#endif
    zoomSlider->setGeometry(mainwindow->width()-30,20,20,mainwindow->height()-40);

}

void MainWindowPockemul::resizeSlot( QSize size , CPObject *pObject)
{
    delete(pObject->FinalImage);

    int l = menuBar()->height();
    pObject->FinalImage = new QImage(size-QSize(0,l),QImage::Format_RGB32);
}

void MainWindowPockemul::DestroySlot( CPObject *pObject)
{
//qWarning()<< QApplication::topLevelWidgets();
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

    emit DestroySignal(pObject);
    WatchPoint.remove(pObject);

    pObject->exit();
    pObject->close();
#ifdef EMSCRIPTEN
    //delete pObject;
#else
    pObject->deleteLater();
#endif
}

void MainWindowPockemul::closeEvent(QCloseEvent *event)
{

}

void MainWindowPockemul::slotMsgError(QString msg) {
    MSG_ERROR(msg);
}



void MainWindowPockemul::initCommandLine(void) {
//    static const struct QCommandLineConfigEntry conf[] =
//        {
//          { QCommandLine::Option, "v", "verbose", "Verbose level (0-3)", QCommandLine::Mandatory },
//          { QCommandLine::Switch, "l", "list", "Show a list", QCommandLine::Optional },
//          { QCommandLine::Param, NULL, "target", "The target", QCommandLine::Mandatory },
//          { QCommandLine::Param, NULL, "source", "The sources", QCommandLine::MandatoryMultiple },
//          { QCommandLine::None, NULL, NULL, NULL, QCommandLine::Default }
//        };
       cmdline = new QCommandLine(this);
//       cmdline->addOption(QChar('v'), "verbose", "verbose level (0-3)");
//       cmdline->addSwitch(QChar('l'), "list", "show a list");
//       cmdline->addParam("source", "the sources", QCommandLine::MandatoryMultiple);
//       cmdline->addParam("target", "the target", QCommandLine::Mandatory);

       cmdline->addOption('l',"load","Load a .pml session file");
       cmdline->addSwitch('v', "version", "show current version");



//       cmdline->setConfig(conf);
       cmdline->enableVersion(true); // enable -v // --version
       cmdline->enableHelp(true); // enable -h / --help

      connect(cmdline, SIGNAL(switchFound(const QString &)),
              this, SLOT(switchFound(const QString &)));
      connect(cmdline, SIGNAL(optionFound(const QString &, const QVariant &)),
              this, SLOT(optionFound(const QString &, const QVariant &)));
      connect(cmdline, SIGNAL(paramFound(const QString &, const QVariant &)),
              this, SLOT(paramFound(const QString &, const QVariant &)));
      connect(cmdline, SIGNAL(parseError(const QString &)),
              this, SLOT(parseError(const QString &)));

      cmdline->parse();
}

void MainWindowPockemul::switchFound(const QString & name)
{
  qDebug() << "Switch:" << name;
}

void MainWindowPockemul::optionFound(const QString & name, const QVariant & value)
{
  qDebug() << "Option:" << name << value;
  if (name == "load") { opensession(value.toString()); }
}

void MainWindowPockemul::paramFound(const QString & name, const QVariant & value)
{
  qDebug() << "Param:" << name << value;
}

void MainWindowPockemul::parseError(const QString & error)
{
  qDebug() << qPrintable(error);
  cmdline->showHelp(true, -1);
  QCoreApplication::quit();
}
