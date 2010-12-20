#include "ce125.h"
#include "ce152.h"

TransMap KeyMapce125tape[]={
    {1,	"EJECT/LOAD  ",	K_EJECT,34,234,	9},		//OK
    {2,	"PLAY        ",	K_PLAY,156,234,	9},		//OK
    {3,	"RECORD      ",	K_RECORD,194,234,9},		//OK
};
int KeyMapce125tapeLenght = 3;

TransMap KeyMapce125[]={
    {1,	"FEED  ",	K_PFEED,34,234,	9}		//OK
};
int KeyMapce125Lenght = 1;


void Cce125tape::ComputeKey(void)
{

	if (pKEYB->LastKey == K_PLAY) 
		{
            if (mode == LOAD)
				{
                    BackGroundFname	= playImage;
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
        case STOP : BackGroundFname	= ":/EXT/ext/ce-125eject.png";
					InitDisplay();
					update();
					if (LoadTape())
                        BackGroundFname	= loadImage;
					else
                        BackGroundFname	= ":/EXT/ext/ce-125tape.png";
					InitDisplay();
					update(); break;
        case LOAD : BackGroundFname	= ":/EXT/ext/ce-125eject.png";
					InitDisplay();
					update();
					if (LoadTape())
                        BackGroundFname	= loadImage;
					else
                        BackGroundFname	= ":/EXT/ext/ce-125tape.png";
					InitDisplay();
					update(); break;
		case RECORD:
        case PLAY : BackGroundFname	= loadImage;
					InitDisplay();
					update();
					StopPlay(); break;
		}
	}
	if (pKEYB->LastKey == K_RECORD) 
		{
            RecTape();
            BackGroundFname	= RECORD;
            InitDisplay();
            update();
		}
};

Cmp220::Cmp220(CPObject *parent):Cce125(parent)
{
    BackGroundFname	= ":/EXT/ext/mp-220.jpg";
    setcfgfname("mp220");
}

Cce125::Cce125(CPObject *parent):Cce126(parent)
{								//[constructor]
    BackGroundFname	= ":/EXT/ext/ce-125.jpg";
    setcfgfname("ce125");

    Pc_DX	= 731;
    Pc_DY	= 532;
    SnapPts = QPoint(247,280);
    pCONNECTOR->setSnap(QPoint(247,370));
    setPaperPos(QRect(377,0,207,149));

    stackBehind = true;
    KeyMap		= KeyMapce125;
    KeyMapLenght= KeyMapce125Lenght;
    delete pKEYB; pKEYB=new Ckeyb(this,"ce125.map");
}

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
    pTAPE->exit();
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


