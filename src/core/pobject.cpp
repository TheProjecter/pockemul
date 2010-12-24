#include <QtGui>
#include <QMutex>

#include "common.h"
#include "pobject.h"
#include "Log.h"
#include "pcxxxx.h"
#include "Keyb.h"
#include "extension.h"
#include "Connect.h"
#include "clink.h"
#include "dialogkeylist.h"
#include "dialogdump.h"
#include "weblinksparser.h"
 
extern QList<CPObject *> listpPObject; 
FILE	*fp_tmp=NULL;

#define NEW_SOUND 0
extern QWidget* mainwidget;

CPObject::CPObject(CPObject *parent):QWidget(mainwidget)
	{
		pPC = (CpcXXXX*) parent;
		Parent	= parent;
		toDestroy = false;
		PosX	= 0;
		PosY	= 0;
		Pc_DX	= 0;
		Pc_DY	= 0;
        zoom = 100;
		pKEYB	= 0;
		pTIMER	= 0;
		pLCDC	= 0;
		FinalImage = 0;
		BackgroundImage = 0;
		LcdImage = 0;
		SymbImage = 0;
		extensionArray[0] = 0;
		extensionArray[1] = 0;
		extensionArray[2] = 0;
		extensionArray[3] = 0;
		extensionArray[4] = 0;
		frequency	= 0;
		setFocusPolicy(Qt::StrongFocus);	
		Refresh_Display = false;
		stackBehind = false;
		
		SnapPts = QPoint(0,0);
		
		Front = true;
		fillSoundBuffer_old_state = -1;

		dialogkeylist	= 0;
		dialogdump		= 0;
		Power = false;
        audioBuff.clear();
		
	};


CPObject::~CPObject()
{
	//delete pKEYB;
	//delete pTIMER;
	//delete pLCDC;	
	
	//delete FinalImage;
	//delete BackgroundImage;
	//delete LcdImage;
	//delete SymbImage;
	//
	//delete extensionArray[0];
	//delete extensionArray[1];
	//delete extensionArray[2];
	//delete extensionArray[3];
	//delete extensionArray[4];
}


int	CPObject::posx()
{
	return PosX;
}
int CPObject::posy()
{
	return PosY;
}
void CPObject::setPosX(int val)
{
	PosX = val;
}
void CPObject::setPosY(int val)
{
	PosY = val;
}
void CPObject::Move(QPoint p)
{
	PosX += p.x();
	PosY += p.y();
	QWidget::move(QPoint(PosX,PosY));
}

QPoint CPObject::pos()
{
	return QPoint(PosX,PosY);
}
QRect CPObject::rect()
{
	return QRect(PosX,PosY,Pc_DX,Pc_DY);
}

bool CPObject::init()
{
	startKeyDrag = false;
	startPosDrag = false;
	setMouseTracking(true);
	resize(Pc_DX,Pc_DY);	
	move(QPoint(PosX,PosY));
	setAttribute(Qt::WA_AlwaysShowToolTips,true);

	return true;
}

bool CPObject::exit()
{
	if (pKEYB)		pKEYB->exit();

	return true;
}

#define SAMPLERATE 8000
#define BUFFLEN 500

void CPObject::audioStateChanged(QAudio::State state)
{
    if (state == QAudio::IdleState) {

    }
    qWarning() << "state = " << state;
}

int CPObject::initsound()
{
    int DataFrequencyHz = 8000;
    int BufferSize      = 800;
#if NEW_SOUND
    QAudioDeviceInfo m_device(QAudioDeviceInfo::defaultOutputDevice());
    m_format.setFrequency(DataFrequencyHz);
    m_format.setChannels(1);
    m_format.setSampleSize(8);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }
    //delete m_audioOutput;
    m_audioOutput = 0;
    m_audioOutput = new QAudioOutput(m_device, m_format, this);
    //connect(m_audioOutput, SIGNAL(notify()), SLOT(notified()));
    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(audioStateChanged(QAudio::State)));
   // m_audioOutput->setBufferSize(BufferSize);

    m_output = m_audioOutput->start();
    int p = m_audioOutput->periodSize();
    qWarning()<<p;
#endif
#ifndef NO_SOUND
	unsigned int mode;
	int lenbytes;
	int samplerate;
	
    lenbytes = BUFFLEN;
	mode = FSOUND_LOOP_OFF | FSOUND_MONO | FSOUND_8BITS | FSOUND_SIGNED;
	samplerate = SAMPLERATE;

	pStream = FSOUND_Stream_Create( CustomStreamCallBack , lenbytes ,  mode , samplerate ,  this);
	
	iChanIndex = FSOUND_Stream_Play( FSOUND_FREE, pStream ); 
	FSOUND_SetVolumeAbsolute(iChanIndex,30);
	return iChanIndex;
#endif
}

int CPObject::exitsound()
{
#ifndef NO_SOUND
	FSOUND_Stream_Close(pStream);
#endif
	return true;
}

#ifndef NO_SOUND
signed char F_CALLBACKAPI CPObject::CustomStreamCallBack( FSOUND_STREAM *stream, void *buff, int len, void * param )
{
    mainwindow->audioMutex.lock();

    CPObject *pPC = ((CPObject *) param);
    char *mono8bitbuffer = (char *)buff;

    for (int count=0; count<len; count++)        
    {
        if (! pPC->soundBuffer.isEmpty())
        {
            *mono8bitbuffer++ =  pPC->soundBuffer.at(0)-128;
            pPC->soundBuffer.removeAt(0);
		}
		else *mono8bitbuffer++ = 0-128;
    }
//	fprintf(fp_tmp,"\n%s\n",tr("%1 : buffersize = %2 len=%3 stream=%4 buff=%5").arg(pPC->getName()).arg(pPC->soundBuffer.size()).arg(len).arg((int)stream).arg((int)buff).toLocal8Bit().data());

	// Purge buffer for resync
    if ( (pPC->soundBuffer.size() > BUFFLEN) && (pPC->soundBuffer.indexOf(0xff) == -1) )
	{
		pPC->soundBuffer.clear();
	}

    mainwindow->audioMutex.unlock();
	return 1;
}
#endif

void CPObject::fillSoundBuffer(BYTE val)
{

	qint64 new_state,delta_state;
	 
	if (fillSoundBuffer_old_state == -1) fillSoundBuffer_old_state = pTIMER->state;
		
	new_state = pTIMER->state;
	delta_state = new_state - fillSoundBuffer_old_state;
    if (delta_state < 0) fillSoundBuffer_old_state=new_state;
	// Calculate nb of state to skip corresponding to the CPU frequency
    qint64 wait = ((pTIMER->CPUSpeed*getfrequency()) / SAMPLERATE );
//	fprintf(fp_tmp,"%s\n",tr("%1 : wait = %2  -  delta=%3  new:%4 - old:%5  ptimer:%6").arg(getName()).arg(wait).arg(delta_state).arg(new_state).arg(fillSoundBuffer_old_state).arg((int)pTIMER).toLocal8Bit().data());
    if (delta_state >= wait)
    {
        mainwindow->audioMutex.lock();
        while (delta_state >= wait)
        {
#if NEW_SOUND
            audioBuff.append(val);
            if (val) { AddLog(LOG_MASTER,tr("SOUND:%1").arg(val))}
            int ps = m_audioOutput->periodSize();
            if (audioBuff.size() >= (ps)) {
                m_output->write(audioBuff,ps);
                audioBuff.clear();
            }
            else {
                if((m_audioOutput->bufferSize()-m_audioOutput->bytesFree()) < ps) {
                    QByteArray fill(ps,0);
                    m_output->write(fill,ps);
                }
            }
#else
            //if (soundBuffer.size() < BUFFLEN)
                soundBuffer.append(val);
#endif
            fillSoundBuffer_old_state += wait;
            delta_state -= wait;
        }
        mainwindow->audioMutex.unlock();
    }
}

void CPObject::mouseDoubleClickEvent(QMouseEvent *event)
{
    // Check if we clic a key
    QPoint pts(event->x() , event->y());
    if ((pKEYB) &&(pKEYB->KeyClick(pts))) {
        // Send thee mouseclick event twice
        QMouseEvent *e=new QMouseEvent(QEvent::MouseButtonPress, pts, Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(this, e);
        delete e;
        return;
    }
#if 1


    if (parentWidget() == mainwidget)
    {
        // Search all conected objects then move them
        QList<CPObject *> ConList;
        ConList.append(this);
        mainwindow->pdirectLink->findAllObj(this,&ConList);
        for (int i=0;i<ConList.size();i++)
        {
            ConList.at(i)->SwitchFrontBack(this->pos());
            // Move object at the correct origine
        }

        //SwitchFrontBack();
        //update();
    }
#endif
}

void CPObject::SwitchFrontBack(QPoint point) {
    if (Front)
    {
        // calculate the new origine
        int newposx = point.x() + (pos().x()-point.x())/4;
        int newposy = point.y() + (pos().y()-point.y())/4;
        zoom = 25;
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(1000);
        animation->setStartValue(this->rect());
        animation->setEndValue(QRect(newposx,newposy,Pc_DX/4,Pc_DY/4));
        animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->start();
        this->setPosX(newposx);
        this->setPosY(newposy);

    }
    else
    {
        // calculate the new origine
        int newposx = point.x() + (pos().x()-point.x())*4;
        int newposy = point.y() + (pos().y()-point.y())*4;
        zoom = 100;
//        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
//        animation->setDuration(1000);
//        animation->setStartValue(this->rect());
//        animation->setEndValue(QRect(newposx,newposy,Pc_DX,Pc_DY));
//        animation->setEasingCurve(QEasingCurve::OutBounce);
//        animation->start();
        setGeometry(newposx,newposy,Pc_DX,Pc_DY);
        this->setPosX(newposx);
        this->setPosY(newposy);
//            QPoint newpos = pos() - mainwindow->pos();
//            setParent(mainwindow);
//            move(newpos);

    }
    Front = ! Front;
}

void CPObject::mousePressEvent(QMouseEvent *event)
{

    if (event->button() != Qt::LeftButton) return;
	if (event->modifiers() == Qt::MetaModifier) return;
		
	QPoint pts(event->x() , event->y());
	
	if (dialogkeylist)
	{
 		// look keyFound
		if ( dialogkeylist->i->Rect.contains(pts) )
		{
			// keydef found start drag mode	
			// Change mouse pointer
			setCursor(Qt::SizeAllCursor);
			startKeyDrag = true;
			KeyDrag = event->globalPos();
			return;
		}
	}

	if (pKEYB)
	{
		pKEYB->LastKey = pKEYB->KeyClick(pts);
//		AddLog(LOG_KEYBOARD,tr("KeyClick(%1,%2)=%3").arg(pts.x()).arg(pts.y()).arg(pKEYB->LastKey));

		if (pKEYB->LastKey == K_OF)
		{
            slotPower();
		}
		
		if (pKEYB->LastKey != 0)
		{
			ComputeKey();
		}
		
		if (pKEYB->LastKey != 0) return;
	}

	// NO KEY CLICK Global pobject drag mode

	CPObject * ConnectedObj = 0;						// bring the Widget in front
    if (ConnectedObj=mainwindow->pdirectLink->findObj(this))		// Is this object connected to another ?
	{
		if (stackBehind)				// Is this object always behind the connected one ?
		{
			ConnectedObj->raise();
			stackUnder(ConnectedObj);
		}
		else
		{
			 raise();
			 ConnectedObj->stackUnder(this);
		}
	}
	else raise();
	
    if ( (parentWidget() != mainwidget) //mainwindow
        && (parentWidget() != 0))
	{
		QApplication::sendEvent(parentWidget(), event);
	}
	else
	{
		setCursor(Qt::ClosedHandCursor);	// Change mouse pointer
		startPosDrag = true;
		PosDrag = event->globalPos();
	}
}

void CPObject::mouseMoveEvent( QMouseEvent * event )
{
	if (dialogkeylist)
	{
		if (startKeyDrag)
		{
			QPoint delta(event->globalPos() - KeyDrag);
			dialogkeylist->i->Rect.adjust(delta.x(),delta.y(),delta.x(),delta.y());
			pKEYB->modified = true;
			KeyDrag = event->globalPos();
			repaint();
			return;
		}
	}
	
	if (startPosDrag)
	{
		QPoint delta(event->globalPos() - PosDrag);
		
		// Search all conected objects then move them
		QList<CPObject *> ConList;
		ConList.append(this);
        mainwindow->pdirectLink->findAllObj(this,&ConList);
		for (int i=0;i<ConList.size();i++)
		{
			ConList.at(i)->Move(delta);
		}
		
		PosDrag = event->globalPos();
		repaint();
		return;
	}
	
	if (pKEYB)
	{
		QPoint pts(event->x() , event->y());
		if (pKEYB->KeyClick(pts))
		{
			setCursor(Qt::PointingHandCursor);
			setToolTip(pKEYB->KeyString(pts));
		}
		else
		{
			setCursor(Qt::ArrowCursor);
		}
	}
	
    if ( (parentWidget() != mainwidget)
        && (parentWidget() != 0))
	{
		QApplication::sendEvent(parentWidget(), event);
	}
	
}

#define SNAPRANGE 20

QList<Cconnector *> CPObject::nearConnectors(Cconnector *refConnector,qint8 snaprange) {
    // Compare snap between the refConnector and all free object connectors
    QList<Cconnector *> retList;
    for (int i=0;i<ConnList.size();i++) {
        if (!mainwindow->pdirectLink->isLinked(ConnList.at(i)) &&
            Cconnector::arePluggable(ConnList.at(i),refConnector)) {
            Cconnector *c = ConnList.at(i);
            // ATTENTION : POSITIONN DE L'OBJECT + SNAP !!!!!!
            CPObject *p = refConnector->Parent;
            qreal range = QLineF(p->pos()+refConnector->getSnap(),this->pos()+c->getSnap()).length();
            if (range < snaprange) {
                retList.append(ConnList.at(i));
            }
        }
    }
    return retList;
}

void CPObject::mouseReleaseEvent(QMouseEvent *event)
{
	// if a connector is free
	// if an object with free connector is "near"
	// propose to autolink

#if 1
        // Fetch all object
		for (int k = 0; k < listpPObject.size(); k++)
		{
            // fetch all others objects FREE connectors
            if (listpPObject.at(k) != this) {
                // Fect object connectors
                for (int c=0; c < listpPObject.at(k)->ConnList.size(); c++) {
                    if (!mainwindow->pdirectLink->isLinked(listpPObject.at(k)->ConnList.at(c))) {
                        // If not already linked
                        QList<Cconnector *> nearList = nearConnectors(listpPObject.at(k)->ConnList.at(c),SNAPRANGE);
                        for (int r=0; r<nearList.size();r++) {
                            switch(QMessageBox::question(mainwindow, "PockEmul",
                                                    "Do you want to link those two materials ?\n"+
                                                    nearList.at(r)->Desc + "--> ["+ listpPObject.at(k)->getName()+"]"+listpPObject.at(k)->ConnList.at(c)->Desc,
                                                    "Yes",
                                                    "No", 0, 0, 1))
                            {
                            case 0: // The user clicked the Yes button or pressed Enter
                            // Connect
                                Move(listpPObject.at(k)->pos() + listpPObject.at(k)->ConnList.at(c)->getSnap() - pos() - nearList.at(r)->getSnap());
                                mainwindow->pdirectLink->AConnList.append(listpPObject.at(k)->ConnList.at(c));
                                mainwindow->pdirectLink->BConnList.append(nearList.at(r));

                                break;
                            case 1: // The user clicked the No or pressed Escape
                                // exit
                                break;
                            }
                        }
                    }
                }
            }
        }



#else
    if (! mainwindow->pdirectLink->isLinked(this))
    {
        for (int k = 0; k < listpPObject.size(); k++)
        {
			if ( (listpPObject.at(k) != this) && 
                !mainwindow->pdirectLink->isLinked(listpPObject.at(k)) &&
				(RangeFrom(listpPObject.at(k)) < SNAPRANGE) )
			{
				// Propose to Autolink
				switch(QMessageBox::question(mainwindow, "PockEmul",
	                                    "Do you want to link those two materials ?",
	                                    "Yes",
	                                    "No", 0, 0, 1))
	            { 
	        	case 0: // The user clicked the Yes again button or pressed Enter
	            // try again
					Move(listpPObject.at(k)->pos() + listpPObject.at(k)->SnapPts - pos() - SnapPts);
                    mainwindow->pdirectLink->AConnList.append(ConnList.at(0));
                    mainwindow->pdirectLink->BConnList.append(listpPObject.at(k)->ConnList.at(0));

	        	    break;
	        	case 1: // The user clicked the No or pressed Escape
	        	    // exit
	        	    break;
	        	}
			}
		}
    }
#endif
	
	
    startKeyDrag = false;
    startPosDrag = false;
	setCursor(Qt::ArrowCursor);
	if (pKEYB) pKEYB->LastKey = 0;

    if ( (parentWidget() != mainwidget)
        && (parentWidget() != 0))
	{
		QApplication::sendEvent(parentWidget(), event);
	}

}
			
qreal CPObject::RangeFrom(CPObject * target)
{
	qreal range = QLineF(pos() + SnapPts,target->pos() + target->SnapPts).length();
	return range;
}

void CPObject::paintEvent(QPaintEvent *event)
{
	QPainter painter;

	UpdateFinalImage();
		
	painter.begin(this);	
	if (FinalImage)
	{
		painter.drawImage(QPoint(0,0), FinalImage->scaled(this->size(),Qt::IgnoreAspectRatio));
	}
		
	if (dialogkeylist)
	{
		painter.setPen(QPen(Qt::red));
		QRect rect = dialogkeylist->getkeyFoundRect();
		painter.drawRect(rect);
	}

	painter.end();

}

void CPObject::keyReleaseEvent(QKeyEvent * event )
{
	if (!pKEYB) return;	// if no Keyboard then return;
		
	pKEYB->LastKey = 0;
}

void CPObject::keyPressEvent (QKeyEvent * event )
{
		
	if (!pKEYB) return;	// if no Keyboard then return;

    pKEYB->isShift = (QApplication::keyboardModifiers() == Qt::ShiftModifier);

   	switch (event->key()) {
        //case Qt::Key_Shift:		pKEYB->LastKey = K_SHT;		event->accept();	break;
   		case Qt::Key_Return:	pKEYB->LastKey = K_RET;		event->accept();	break;
        case Qt::Key_Delete:	pKEYB->LastKey = K_DEL;		event->accept();	break;
        case Qt::Key_Insert:	pKEYB->LastKey = K_INS;		event->accept();	break;
        case Qt::Key_Space:		pKEYB->LastKey = ' ';		event->accept();	break;
		case Qt::Key_Period:	pKEYB->LastKey = '.';		event->accept();	break;
		case Qt::Key_Plus:		pKEYB->LastKey = '+';		event->accept();	break;
		case Qt::Key_Minus:		pKEYB->LastKey = '-';		event->accept();	break;
		case Qt::Key_Asterisk:	pKEYB->LastKey = '*';		event->accept();	break;
		case Qt::Key_Slash:		pKEYB->LastKey = '/';		event->accept();	break;
		case Qt::Key_Tab:		pKEYB->LastKey = '=';		event->accept();	break;
		case Qt::Key_Backspace:	pKEYB->LastKey = K_MOD;		event->accept();	break;
		case Qt::Key_CapsLock:	pKEYB->LastKey = K_SML;		event->accept();	break;
		case Qt::Key_Left:		pKEYB->LastKey = K_LA;		event->accept();	break;
		case Qt::Key_Right:		pKEYB->LastKey = K_RA;		event->accept();	break;
		case Qt::Key_Up:		pKEYB->LastKey = K_UA;		event->accept();	break;
		case Qt::Key_Down:		pKEYB->LastKey = K_DA;		event->accept();	break;
		case Qt::Key_F1:		pKEYB->LastKey = K_F1;		event->accept();	break;
		case Qt::Key_F2:		pKEYB->LastKey = K_F2;		event->accept();	break;
		case Qt::Key_F3:		pKEYB->LastKey = K_F3;		event->accept();	break;
		case Qt::Key_F4:		pKEYB->LastKey = K_F4;		event->accept();	break;
		case Qt::Key_F5:		pKEYB->LastKey = K_F5;		event->accept();	break;
		case Qt::Key_F6:		pKEYB->LastKey = K_F6;		event->accept();	break;
        case Qt::Key_F7:		pKEYB->LastKey = K_F7;		event->accept();	break;
		case Qt::Key_F8:		pKEYB->LastKey = K_CLR;		event->accept();	break;
		case Qt::Key_F9:		pKEYB->LastKey = K_DEF;		event->accept();	break;
		case Qt::Key_F11:		pKEYB->LastKey = K_BRK;		event->accept();	break;
		}
	if ( (event->key() >= 0x41) && (event->key() <= 0x5A) ) { pKEYB->LastKey = event->key(); event->accept();	}
	if ( (event->key() >= 0x30) && (event->key() <= 0x39) ) { pKEYB->LastKey = event->key(); event->accept();	}
		
	event->ignore();
}

void CPObject::focusInEvent ( QFocusEvent * event )
{
}

void CPObject::focusOutEvent ( QFocusEvent * event )
{
}

void CPObject::contextMenuEvent ( QContextMenuEvent * event )
{

	QMenu menu(this);
	BuildContextMenu(&menu);

	menu.exec(event->globalPos () );
}

void CPObject::BuildContextMenu(QMenu * menu)
{
	if ( dynamic_cast<CpcXXXX *>(this) )
	{
		QMenu * menupocket = menu->addMenu(tr("Pocket"));
			menupocket->addAction(tr("Turn On"),this,SLOT(slotPower()));
			menupocket->addAction(tr("Reset"),this,SLOT(slotReset()));
            //menupocket->addAction(tr("Detach"),this,SLOT(slotDetach()));
            menupocket->addSeparator();
            menupocket->addAction(tr("Load ..."),this,SLOT(slotLoadSession()));
            menupocket->addAction(tr("Save ..."),this,SLOT(slotSaveSession()));
	}
	
	QMenu * menuext = NULL;
	for (int i=0;i<5;i++)
    {
        if (extensionArray[i])
            {
                if (!menuext) menuext = menu->addMenu(tr("Extensions"));
                menuext->addMenu(((CpcXXXX *)this)->extensionArray[i]->Menu);
            }
    }
	
	QMenu * menuconfig = menu->addMenu(tr("Configuration"));
	
    if ( dynamic_cast<CpcXXXX *>(this) )
    {
        QMenu * menucpuspeed = menuconfig->addMenu(tr("CPU Speed"));
            menucpuspeed->addAction(tr("100%"));
            menucpuspeed->addAction(tr("200%"));
            menucpuspeed->addAction(tr("300%"));
            menucpuspeed->addAction(tr("500%"));
            menucpuspeed->addAction(tr("Maximum"));

            connect(menucpuspeed, SIGNAL(triggered(QAction*)), this, SLOT(slotCpu(QAction*)));
}
	
    if (pLCDC)
    {
        QMenu * menulcd = menuconfig->addMenu(tr("LCD contrast"));
            menulcd->addAction(tr("0"));
            menulcd->addAction(tr("1"));
            menulcd->addAction(tr("2"));
            menulcd->addAction(tr("3"));
            menulcd->addAction(tr("4"));

            connect(menulcd, SIGNAL(triggered(QAction*)), this, SLOT(slotContrast(QAction*)));

        }

    if (pKEYB) {
        menuconfig->addAction(tr("Keyboard"),this,SLOT(KeyList()));
    }

    if ( dynamic_cast<CpcXXXX *>(this) ) {
        menu->addAction(tr("Dump Memory"),this,SLOT(Dump()));
    }
	menu->addSeparator();
	computeLinkMenu(menu);
	computeUnLinkMenu(menu);
	menu->addSeparator();
	menu->addAction(tr("Close"),this,SLOT(slotExit()));
    computeWebLinksMenu(menu);
}

void CPObject::computeWebLinksMenu(QMenu * menu) {
    menuweblink = menu->addMenu(tr("Web Links"));
    connect(menuweblink, SIGNAL(triggered( QAction *)), mainwindow, SLOT(slotWebLink( QAction *)));
// FETCH XML FILE TO ADD MENU ACTIONS
    QFile fileRes(":/POCKEMUL/weblinks.xml");
    QXmlInputSource sourceRes(&fileRes);
    QXmlSimpleReader reader;
    reader.setContentHandler( new WebLinksParser(this) );

    bool result = reader.parse( sourceRes );


}

void CPObject::insertLinkAction(QString desc,QString link) {
    QLabel* label = new QLabel("<a href='"+link+"'>"+desc+"</a>", this);
    label->setOpenExternalLinks(true);

    QWidgetAction* a = new QWidgetAction(this);
    a->setDefaultWidget( label );
    menuweblink->addAction(a);
}

void CPObject::computeLinkMenu(QMenu * menu)
{
	QMenu * menulink = menu->addMenu(tr("Link"));
	connect(menulink, SIGNAL(triggered( QAction *)), mainwindow, SLOT(slotNewLink( QAction *)));    
	for (int i = 0;i < ConnList.size(); i++)
 	{
		QMenu * menuLocConn = menulink->addMenu(ConnList.at(i)->Desc);
		
		for (int j = 0;j < listpPObject.size(); j++)
		{
			if (listpPObject.at(j) != this)
			{
				QMenu * menuAllPc = menuLocConn->addMenu(listpPObject.at(j)->getName());
				for (int k = 0; k < listpPObject.at(j)->ConnList.size(); k++)
				{
					QAction * actionDistConn = menuAllPc->addAction(listpPObject.at(j)->ConnList.at(k)->Desc);
					actionDistConn->setData(tr("%1:%2").arg((long)ConnList.at(i)).arg((long)listpPObject.at(j)->ConnList.at(k)));

				}
			}
		}
	}	
}

void CPObject::computeUnLinkMenu(QMenu * menu)
{
	QMenu * menulink = menu->addMenu(tr("Remove Link"));
    if(ConnList.size()>1) {
        QAction * actionLocAllConn = menulink->addAction("All");
        actionLocAllConn->setData(tr("A%1").arg((long)this));
        menulink->addSeparator();
    //connect(menulink, SIGNAL(triggered( QAction *)), mainwindow, SLOT(slotUnLinkAll( QAction *)));
    }
	for (int i = 0;i < ConnList.size(); i++)
 	{
		QAction * actionLocConn = menulink->addAction(ConnList.at(i)->Desc);
        actionLocConn->setData(tr("C%1").arg((long)ConnList.at(i)));
		connect(menulink, SIGNAL(triggered( QAction *)), mainwindow, SLOT(slotUnLink( QAction *)));    
		
	}	
}

QImage * CPObject::LoadImage(QSize size,QString fname)
{
	QImage *tempImage;
	QImage loc = QImage(fname).scaled(size);
	tempImage = new QImage(loc);
	return tempImage;
}


void CPObject::publish(Cconnector* newConn)
{
	ConnList.append(newConn);
}
void CPObject::remove(Cconnector* newConn)
{
    ConnList.removeAt(ConnList.indexOf(newConn));
}


void CPObject::slotExit(void)
{
	toDestroy = true;
}

bool CPObject::InitDisplay(void)
{
	BackgroundImageBackup = LoadImage(QSize(Pc_DX, Pc_DY),BackGroundFname);
	BackgroundImage  =  LoadImage(QSize(Pc_DX, Pc_DY),BackGroundFname);
	FinalImage  =  LoadImage(QSize(Pc_DX, Pc_DY),BackGroundFname);

	return(1);
}

void CPObject::UpdateFinalImage(void)
{
	QPainter painter;
	if ( (BackgroundImage) )
	{
		painter.begin(FinalImage);
		painter.drawImage(QPoint(0,0),*BackgroundImage);
		painter.end();
	}
	Refresh_Display = false;
}

void CPObject::KeyList()
{
        dialogkeylist = new DialogKeyList(this);
        dialogkeylist->show();
}

void CPObject::setCpu(int newspeed)
{
	pTIMER->SetCPUspeed(newspeed);
}

void CPObject::slotCpu(QAction* action) {
    if (action->text() == tr("100%")) setCpu(1);
    if (action->text() == tr("200%")) setCpu(2);
    if (action->text() == tr("300%")) setCpu(3);
    if (action->text() == tr("500%")) setCpu(5);
    if (action->text() == tr("Maximum")) setCpu(1000);
}

void CPObject::slotContrast(QAction * action) {
    if (action->text() == tr("0")) pLCDC->Contrast(0);
    if (action->text() == tr("1")) pLCDC->Contrast(1);
    if (action->text() == tr("2")) pLCDC->Contrast(2);
    if (action->text() == tr("3")) pLCDC->Contrast(3);
    if (action->text() == tr("4")) pLCDC->Contrast(4);
}

void CPObject::slotPower()
{ 
	Power = !Power;
	if (Power)
		TurnON();
	else
		TurnOFF();
}

void CPObject::slotReset()		{ Reset(); }

void CPObject::slotLoadSession()
{
    ((CpcXXXX *)this)->LoadSession();
}

void CPObject::slotSaveSession()
{
    ((CpcXXXX *)this)->SaveSession();
}

void CPObject::Dump()
{
	dialogdump = new DialogDump(this);
	dialogdump->show();
}

