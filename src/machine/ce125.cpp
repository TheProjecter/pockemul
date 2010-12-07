#include "ce125.h"
#include "ce152.h"

TransMap KeyMapce125tape[]={
1,	"EJECT/LOAD  ",	K_EJECT,34,234,	9,		//OK
2,	"PLAY        ",	K_PLAY,156,234,	9,		//OK
3,	"RECORD      ",	K_RECORD,194,234,9,		//OK
};
int KeyMapce125tapeLenght = 3;




void Cce125tape::ComputeKey(void)
{
	if (pKEYB->LastKey == K_PLAY) 
		{
            if (mode == LOAD)
				{
					BackGroundFname	= ":/EXT/ext/ce-125play.jpg";
					InitDisplay();
					update();				
					Play();	
				}
		}
	if (pKEYB->LastKey == K_EJECT) 
	{
		
		switch (mode)
		{
		case EJECT: 
		case STOP : BackGroundFname	= ":/EXT/ext/ce-125eject.jpg";
					InitDisplay();
					update();
					if (LoadTape())
						BackGroundFname	= ":/EXT/ext/ce-125load.jpg";
					else
						BackGroundFname	= ":/EXT/ext/ce-125tape.jpg";
					InitDisplay();
					update(); break;
		case LOAD : BackGroundFname	= ":/EXT/ext/ce-125eject.jpg";
					InitDisplay();
					update();
					if (LoadTape())
						BackGroundFname	= ":/EXT/ext/ce-125load.jpg";
					else
						BackGroundFname	= ":/EXT/ext/ce-125tape.jpg";
					InitDisplay();
					update(); break;
		case RECORD:
		case PLAY : BackGroundFname	= ":/EXT/ext/ce-125load.jpg";
					InitDisplay();
					update();
					StopPlay(); break;
		}
	}
	if (pKEYB->LastKey == K_RECORD) 
		{
			RecTape();	
		}
};

void Cce125::resizeEvent ( QResizeEvent * event ) {
    float ratio = (float)this->width()/this->Pc_DX ;

    QRect rect = QRect(0,0,pTAPE->Pc_DX,pTAPE->Pc_DY);
    pTAPE->setGeometry(rect.x()*ratio,
              rect.y()*ratio,
              rect.width()*ratio,
              rect.height()*ratio);

    Cce126::resizeEvent(event);
}

bool Cce125::init(void)
{
	Cce126::init();
	
	pTAPE = new Cce125tape( this );
	pTAPE->setParent ( this );
	pTAPE->init();
	pTAPE->InitDisplay();
	pTAPE->show();
	
	return true;
}

bool Cce125::exit(void)
{
	delete pTAPE;
	
	
	Cce126::exit();
	return true;
}

bool Cce125::run(void)
{
	Cce126::run();
	
	pTAPE->pTAPECONNECTOR->Set_pin(2,GET_PIN(PIN_MT_OUT1));
	pTAPE->pTIMER = pTIMER;
	pTAPE->run();
	SET_PIN(PIN_MT_IN,pTAPE->pTAPECONNECTOR->Get_pin(1));
	return true;	
}


