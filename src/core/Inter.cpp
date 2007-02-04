#include <QTimer>
#include <QPainter>

#include "common.h"
#include "Inter.h"
#include "pcxxxx.h"
#include "init.h"
#include "Log.h"
#define TIMER_RES 30


UINT nTimerId;
WORD wTimerRes;

void Ctimer::SetCPUspeed(float t)
	{		
		//set CPU speed by %
		CPUSpeed = t ;
		if (t<100) state=( mainwindow->rawclk * (CPUSpeed *(pPC->getfrequency() / 1000L)) );
	};


bool Ctimer::init(void)
{
	AddLog(LOG_MASTER,tr("Timer init..."));
	wTimerRes = 0;
	SetCPUspeed(1);
	AddLog(LOG_MASTER,"Success");
	return(1);
}

bool Ctimer::exit(void)
{
	return(1);
}

BYTE Ctimer::CheckSpeed(void)
{
	qint64 r = (qint64) ( mainwindow->rawclk * (CPUSpeed *(pPC->getfrequency() / 1000L)) );
	if ( state <  r)
		return(1);
	else
		return(0);
}

void Ctimer::check(void)
{

}

