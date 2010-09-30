#include <QPainter> 
#include <QMouseEvent>
#include <QFileDialog>

#include "dialoganalog.h"
#include "Log.h"
#include "common.h"
#include "pcxxxx.h"
#include "Connect.h"
#include "sio.h"
#include "init.h"

#define DEFAULT_POINTS_PER_SAMPLE 10
//
class CPObject;
extern QList<CPObject *> listpPObject; 

dialogAnalog::dialogAnalog( int nbbits,QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);

    connect(chkBCapture, SIGNAL(stateChanged(int)), this, SLOT(updatecapture(int))); 
    connect(pbZoomIn, SIGNAL(clicked()), this, SLOT(zoomin())); 
    connect(pbZoomOut, SIGNAL(clicked()), this, SLOT(zoomout())); 
    connect(pbFit, SIGNAL(clicked()), this, SLOT(fitmarkers())); 
    connect(hlScrollBar, SIGNAL(valueChanged(int)), this, SLOT(scroll(int))); 
    connect(pbSave, SIGNAL(clicked()), this, SLOT(slotSave())); 
    connect(pbLoad, SIGNAL(clicked()), this, SLOT(slotLoad())); 
    connect(pbMarker, SIGNAL(clicked()), this, SLOT(slotMarker())); 
    connect(twWatchPoint, SIGNAL(currentItemChanged ( QTreeWidgetItem * , QTreeWidgetItem * )), this, SLOT(slotChangeWatchPoint( QTreeWidgetItem * , QTreeWidgetItem * ))); 

	Capture = false; 
    NbBits = nbbits;
    m_zoom = 1.0;
    pPC = 0;
    currentWatchPoint = 0;
    currentWatchPointSize=8;
    fill_twWatchPoint();
    twWatchPoint->expandAll();
}


void dialogAnalog::slotChangeWatchPoint( QTreeWidgetItem * current , QTreeWidgetItem * previous)
{
	int pos = current->data(0,Qt::UserRole).toInt();
	currentWatchPoint = WatchPoint.Point.at( pos );
	currentWatchPointSize = WatchPoint.PointSize.at( pos );
	NbBits = WatchPoint.nbBits.at( pos );
	pPC = WatchPoint.PObject.at( pos );
}


void dialogAnalog::fill_twWatchPoint(void)
{
	twWatchPoint->setColumnCount(1);

	
	for (int i = 0; i < listpPObject.size(); i++)
	{
		QTreeWidgetItem *material = new QTreeWidgetItem(twWatchPoint,QStringList(listpPObject.at(i)->getName()));
		for (int j = 0; j < WatchPoint.PObject.size(); j++)
		{
			if (listpPObject.at(i) == WatchPoint.PObject.at(j)) 
			{
				QTreeWidgetItem *Point = new QTreeWidgetItem(material,QStringList(WatchPoint.WatchPointName.at(j)));
				Point->setData(0,Qt::UserRole,j);
			}
		}
	}
}

void dialogAnalog::slotMarker(void)
{
	dataplot.Marker = 1;
}

void dialogAnalog::slotSave(void)
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                            ".",
                            tr("Analogic Sample (*.ana)"));

	QFile file(fileName);
 	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);

	// Write a header with a "magic number" and a version
//	out << (quint32)0xA0B0C0D0;
//	out << (qint32)123;

	out.setVersion(QDataStream::Qt_4_0);

	// Write the data

	out << dataplot;
}

void dialogAnalog::slotLoad(void)
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 ".",
                                                 tr("Analogic Sample (*.ana)"));


	QFile file(fileName);
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);

	// Read the data
	in >> dataplot;

	m_zoom = 1.0;
	ComputeScrollBar();
	this->update();
}


void dialogAnalog::paintEvent(QPaintEvent *event)
{
	QPainter p;
	
	plot(true,frame_dataview->size());

    p.begin(this);
    p.drawPixmap(frame_dataview->mapTo ( this,QPoint(0,0) ), screenPixmap); 
    p.end();

}

// -------------------------------------------------------------------------------------------------
void dialogAnalog::mousePressEvent(QMouseEvent* evt)
{
    if (evt->button() == Qt::LeftButton || evt->button() == Qt::RightButton)
    {
        if (evt->button() == Qt::LeftButton)
        {
            setLeftMarker(frame_dataview->mapFrom( this,QPoint(evt->x(),0)).x());
        }
        else
        {
            setRightMarker(frame_dataview->mapFrom( this,QPoint(evt->x(),0)).x());
        }
        plot(false,frame_dataview->size());
        this->update();
    }
}

void dialogAnalog::wheelEvent( QWheelEvent * event )
{
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;

	hlScrollBar->setValue( hlScrollBar->value() -  numSteps * hlScrollBar->singleStep());
}

void dialogAnalog::scroll(int value)
{
	this->update();
}

void dialogAnalog::updatecapture(int state)
{
	if (state == Qt::Checked )
	{
		setCapture(true);
		dataplot.Clear();
		pbZoomIn->setEnabled(false);
		pbZoomOut->setEnabled(false);
		pbFit->setEnabled(false);
		pbSave->setEnabled(false);
		pbLoad->setEnabled(false);
		groupBox->setEnabled(false);
		chkBShowMarker->setEnabled(false);
		
	}
	else
	{
		setCapture(false);
		// Store timeref
		dataplot.timeUnit = ( pPC->pTIMER->CPUSpeed * pPC->getfrequency() / 1000L );
		ComputeScrollBar();
		
		pbZoomIn->setEnabled(true);
		pbZoomOut->setEnabled(true);
		pbFit->setEnabled(true);
		pbSave->setEnabled(true);
		pbLoad->setEnabled(true);
		groupBox->setEnabled(true);
		chkBShowMarker->setEnabled(true);

		this->update();
	}
}

void dialogAnalog::captureData(void)
{
	if ( !pPC) return;
	if (Capture)
	{
		switch (currentWatchPointSize)
		{
			case 8 : dataplot.Write(*((qint8 *) currentWatchPoint),pPC->pTIMER->state); break;
			case 16: dataplot.Write(*((qint16*) currentWatchPoint),pPC->pTIMER->state); break;
			case 32: dataplot.Write(*((qint32*) currentWatchPoint),pPC->pTIMER->state); break;
			case 64: dataplot.Write(*currentWatchPoint,pPC->pTIMER->state); break;
		}
	}
}
	
void dialogAnalog::plot(bool forceRedraw,QSize size)
{
    QPainter p;
    screenPixmap = QPixmap( size);

	if (forceRedraw)
	{
		initPixmap(size);
        QPen dataPen1( QColor(Qt::green));
		fillPixmap(&dataplot,&dataPen1);
#if 0
	    QPen dataPen2(QColor(Qt::red));
	    dataPen2.setStyle(Qt::DashDotDotLine);
		fillPixmap(&dataplot2,&dataPen2);
#endif
	}
	
    p.begin(&screenPixmap);
    p.drawPixmap(0, 0, lastPixmap);
    drawMarkers(&p);
    p.end();	// draw markers
	


}

#define READ_BIT(b,p)	( ((b)>>(p)) & 0x01 ? 1 :0 )
void dialogAnalog::fillPixmap(CData *data, QPen *dataPen)
{
	TAnalog_Data plot,next_plot;
	QPainter painter;
    int heightPerField = lastPixmap.height() / NbBits;
	
	painter.begin(&lastPixmap);
	painter.setPen(*dataPen);
	
	for (int j=1;j<dataplot.size();j++)
	{
		plot = data->Read(j-1);
		next_plot = data->Read(j);

		int current = heightPerField;
		int X1,Y1,X2,Y2;
		
		X1=StateToX(plot.state);
		X2=StateToX(next_plot.state);
		for (int jj=0;jj<NbBits;jj++)
		{
			//#define READ_BIT(b,p)	( ((b)>>(p)) & 0x01 ? 1 :0 )
			Y1= current - READ_BIT(plot.values,jj)* 3 * heightPerField / 5;
			Y2=current - READ_BIT(next_plot.values,jj)* 3 * heightPerField / 5;
			painter.drawLine(X1,Y1,X2,Y1);
			painter.drawLine(X2,Y1,X2,Y2);
			current += heightPerField;
		}
		
		// plot the Marker
		if ( plot.marker && (chkBShowMarker->checkState() == Qt::Checked) )
			{
				QPen pen((Qt::white));
				pen.setStyle(Qt::DotLine);
				painter.setPen(pen);
				painter.drawLine(X1,12,X1,height());
			    // set font ------------------------------------------------------------------------------------
			    QFont textFont;
			    textFont.setPixelSize(10);
			    painter.setFont(textFont);
				painter.drawText(X1, 11, QString::number(plot.marker));
				painter.setPen(*dataPen);
			}
	}
	painter.end();
}

void dialogAnalog::initPixmap(QSize size)
{
	// resize the pixmap
	//	complete it
	// Draw plot
	QPainter painter;
    int heightPerField = size.height() / NbBits;
	
	lastPixmap=QPixmap( size.width(), size.height() );
	lastPixmap.fill(Qt::black);
	painter.begin(&lastPixmap);
	
    // set font ------------------------------------------------------------------------------------
    QFont textFont;
    textFont.setPixelSize(heightPerField / 2);
    painter.setFont(textFont);

    // set the needed pens -------------------------------------------------------------------------
	QPen linePen(QColor(100,100,100));
    QPen textPen(QColor(255, 255, 255));
    QPen gridPen(QColor(100, 100, 100));
    QPen dataPen(QColor(Qt::green));

	// draw the fields and the text ----------------------------------------------------------------
    {
        int current = heightPerField;
        for (int i = 0; i < (NbBits - 1); i++)
        {
            painter.setPen(linePen);
            painter.drawLine(0, current, size.width(), current);
            
            painter.setPen(textPen);
            painter.drawText(10, current - heightPerField / 3, QString::number(i+1));
            current += heightPerField;
        }
        painter.setPen(textPen);
        painter.drawText(10, current - 15, QString::number(NbBits));
    }

	painter.end();
}

void dialogAnalog::drawLeftMarker(QPainter* painter)
{
    
    if (m_leftMarker != -1)
    {
		QPen leftMarkerPen(QColor(0,255,0));
		painter->setPen(leftMarkerPen);
        painter->drawLine( m_leftMarker, 0, m_leftMarker, height());
    }
}

void dialogAnalog::drawRightMarker(QPainter* painter)
{
    if (m_rightMarker != -1)
    {
		QPen rightMarkerPen(QColor(255,0,0));
		painter->setPen(rightMarkerPen);
        painter->drawLine( m_rightMarker, 0, m_rightMarker, height());
    }
}

void dialogAnalog::drawMarkers(QPainter* painter)
{
	drawLeftMarker(painter);
	drawRightMarker(painter);
}

int dialogAnalog::getLeftMarker() { return m_leftMarker; }
int dialogAnalog::getRightMarker(){ return m_rightMarker;}

void dialogAnalog::setLeftMarker(int markerpos)
{
	m_leftMarker = markerpos;
	ComputeMarkersLength();
}

// -------------------------------------------------------------------------------------------------
void dialogAnalog::setRightMarker(int markerpos)
{
	m_rightMarker = markerpos;
	ComputeMarkersLength();
}

void dialogAnalog::ComputeMarkersLength(void)
{
	long LMarkerState,RMarkerState;
	
	if (m_leftMarker == -1)
		LMarkerState = dataplot.Read_state(0);
	else
		LMarkerState = XToState(m_leftMarker);

	if (m_rightMarker == -1)
		RMarkerState = dataplot.Read_state(dataplot.size()-1);
	else
		RMarkerState = XToState(m_rightMarker);

	long deltaState = qAbs(RMarkerState - LMarkerState);
	
	MarkersLength = deltaState / dataplot.timeUnit;
	
	labelLength->setText(tr("%1 ms").arg(MarkersLength,0,'f',5));
}

void dialogAnalog::ComputeScrollBar(void)
{
	// define scrollbar min,max,step
	long min,max,pagestep;
	
	min = dataplot.Read_state(0);
	pagestep =  (long) ( ( dataplot.Read_state(dataplot.size()-1) - min ) / m_zoom );
	max = dataplot.Read_state(dataplot.size()-1) - pagestep;
	
	hlScrollBar->setMinimum(min);
	hlScrollBar->setMaximum(max);
	hlScrollBar->setPageStep(pagestep);
	hlScrollBar->setSingleStep (pagestep / 50 );
	
}

void dialogAnalog::fitmarkers(void)
{
	int new_pos = XToState(m_leftMarker);
	
	m_zoom = (float)( dataplot.Read_state(dataplot.size()-1) - dataplot.Read_state(0) ) / ( XToState(m_rightMarker) - XToState(m_leftMarker) );
	ComputeScrollBar();
	hlScrollBar->setValue(new_pos);
	setLeftMarker(0);
	setRightMarker(frame_dataview->width());
	this->update();
}

void dialogAnalog::zoomin(void)
{
	m_zoom *= 2;
	ComputeScrollBar();	
	this->update();	
}
void dialogAnalog::zoomout(void)
{
	m_zoom /= 2;
	if (m_zoom <1.0) m_zoom=1.0;
	ComputeScrollBar();		
	this->update();	
	
}

void dialogAnalog::setCapture(bool val)	{ Capture = val; }
bool dialogAnalog::capture(void)		{ return Capture; }

long dialogAnalog::StateToX(long plotState)
{ 
	float ratio = (float) frame_dataview->width() / hlScrollBar->pageStep();
    int loc_X;
	loc_X = (int) (( plotState-hlScrollBar->value() ) * ratio);
	
	return loc_X;
}

long dialogAnalog::XToState(long x)
{ 
    long loc_State;
	float ratio = (float) frame_dataview->width() / hlScrollBar->pageStep();
	loc_State= (long) (((float)x / ratio) + hlScrollBar->value());
	return loc_State;
}
