//#include	<string.h>
#include	<stdlib.h>

#include <QFile>
#include <QFileDialog>
#include <QPainter>
#include <QResizeEvent>

#include	"common.h"
#include	"pcxxxx.h"
#include	"Log.h"
#include	"Dasm.h"
#include	"Debug.h"
#include	"init.h"
#include	"dialoganalog.h"
#include	"extension.h"
#include	"Keyb.h"
#include    "xmlwriter.h"

#define		bREAD				0

extern bool	UpdateDisplayRunning;

CpcXXXX::CpcXXXX(CPObject *parent)	: CPObject(parent)
{								//[constructor]
	Initial_Session_Fname = "TO DO";
	pCPU		= 0;
	pCONNECTOR	= 0;
	pSIOCONNECTOR	= 0;
    KeyMapLenght = 0;

    fp_log  = 0;
	off			= TRUE;
	DialogExtensionID = 0;	
	setcfgfname(QString("pcXXXX"));
	SessionHeader	= "PCXXXXPKM";
	SessionHeaderLen= 9;

	memsize			= 0;
	InitMemValue	= 0x00;
	SoundOn			= 1;
	IO_A = IO_B = IO_C = IO_F = 0;
    RomBank=RamBank=ProtectMemory=0;

	Japan		= false;
	
	Pc_Offset_X	= 0;
	Pc_Offset_Y	= 0;

	Lcd_X		= 0;
	Lcd_Y		= 0;
	Lcd_DX		= 0;
	Lcd_DY		= 0;
	Lcd_ratio_X	= 1;
	Lcd_ratio_Y	= 1;

	Lcd_Symb_X	= 0;
	Lcd_Symb_Y	= 0;
	Lcd_Symb_DX	= 0;
	Lcd_Symb_DY	= 0;;
    Lcd_Symb_ratio_X = Lcd_Symb_ratio_Y	= 1;

	Tape_Base_Freq=4000;
	
	
	
	ext_11pins		= 0;
	ext_MemSlot1	= 0;
	ext_MemSlot2	= 0;
	ext_Serial		= 0;
	ext_60pins		= 0;
	
	setPosX(0);
	setPosY(0);

    ioFreq = 24000;
}

void CpcXXXX::UpdateFinalImage(void)
{
	// Paint FinalImage
    QRect                        destRect,srcRect;
    int x,y,z,t;

	QPainter painter;

//	if (pPC->pLCDC->Refresh) pPC->Refresh_Display = true;
			
//	if (!pPC->Refresh_Display) return;

	if ( (BackgroundImage) )//&& pPC->Refresh_Display)
	{
		painter.begin(FinalImage);
		painter.drawImage(QPoint(0,0),*BackgroundImage);
		painter.end();		
	
		if (pLCDC->Refresh)
			pLCDC->disp();

//		if (pPC->pLCDC->On)
		{
			painter.begin(FinalImage);

			x = Lcd_Symb_X + Pc_Offset_X;
			y = Lcd_Symb_Y + Pc_Offset_Y;
            z = (int) (Lcd_Symb_DX * Lcd_Symb_ratio_X);
            t = (int) (Lcd_Symb_DY * Lcd_Symb_ratio_Y);
		
			painter.drawImage(QRect(x,y,z,t),*SymbImage);	
			
			x	= Lcd_X + Pc_Offset_X;
			y	= Lcd_Y + Pc_Offset_Y;
			z	= (int) (Lcd_DX * Lcd_ratio_X);
			t	= (int) (Lcd_DY * Lcd_ratio_Y);
			painter.drawImage(QRect(x,y,z,t),*LcdImage);		
			
			painter.end();	
		}
		
		pLCDC->Refresh = FALSE;
	}
	
	Refresh_Display = false;
}
	
bool CpcXXXX::CompleteDisplay(void)
{
	return TRUE;
}


bool CpcXXXX::InitDisplay(void)
{
	Refresh_Display = true;
	UpdateDisplayRunning = FALSE;
    global_w = getDX();
    global_h = getDY();
	
    BackgroundImageBackup	= LoadImage(QSize(getDX(), getDY()),BackGroundFname);
    BackgroundImage			= LoadImage(QSize(getDX(), getDY()),BackGroundFname);
    FinalImage				= LoadImage(QSize(getDX(), getDY()),BackGroundFname);
	
	LcdImage				= LoadImage(QSize(Lcd_DX, Lcd_DY),LcdFname);
	SymbImage				= LoadImage(QSize(Lcd_Symb_DX, Lcd_Symb_DY),SymbFname);

	UpdateDisplayRunning = TRUE;
	return(1);
}

void CpcXXXX::TurnOFF(void)
{
    switch (mainwindow->saveAll) {
    case ASK:
        if ( QMessageBox::question(mainwindow, "PockEmul",
                                                         "Do you want to save the session ?",
                                                         "Yes",
                                                         "No", 0, 0, 1) == 0) {
            Initial_Session_Save();
        };
        break;
    case YES: Initial_Session_Save(); break;

    }


	off = 1;
	pLCDC->TurnOFF();
    InitDisplay();
}


void CpcXXXX::TurnON(void)
{
	Initial_Session_Load();
	off = 0;
	pLCDC->TurnON();
}

void CpcXXXX::Reset(void)
{

	pCPU->Reset();
}

BYTE	CpcXXXX::Get_PortA(void) {	return (IO_A); }
BYTE	CpcXXXX::Get_PortB(void) {	return (IO_B); }
BYTE	CpcXXXX::Get_PortC(void) {	return (IO_C); }
BYTE	CpcXXXX::Get_PortF(void) {	return (IO_F); }
BYTE	CpcXXXX::Get_PortT(void) {	return (IO_T); }

BYTE	CpcXXXX::Get_Port(PORTS Port)
{
	switch (Port)
	{
		case PORT_A: return (Get_PortA());
		case PORT_B: return (Get_PortB());
		case PORT_C: return (Get_PortC());
		case PORT_F: return (Get_PortF());
		case PORT_T: return (Get_PortT());
	}
	return (0);
}

bool	CpcXXXX::Get_Port_bit(PORTS Port, int bit)
{
	return ( (Get_Port(Port) >> (bit-1)) & 1);
}

void	CpcXXXX::Set_PortA(BYTE data) {	IO_A = data; }
void	CpcXXXX::Set_PortB(BYTE data) {	IO_B = data; }
void	CpcXXXX::Set_PortF(BYTE data) {	IO_F = data; }
void	CpcXXXX::Set_PortT(BYTE data) {	IO_T = data; }
void	CpcXXXX::Set_PortC(BYTE data) {	IO_C = data; 
//										pLCDC->Refresh = ((IO_C & 0x01)?TRUE:FALSE); 
													}

void	CpcXXXX::Set_Port(PORTS Port,BYTE data)
{
	switch(Port)
	{
		case PORT_A: Set_PortA(data);	break;
		case PORT_B: Set_PortB(data);	break;
		case PORT_C: Set_PortC(data);	break;
		case PORT_F: Set_PortF(data);	break;
		case PORT_T: Set_PortT(data);	break;
	}
}


void	CpcXXXX::Set_Port_Bit(PORTS Port, int bit, BYTE data)
{
	int t;

	if (data)
	{
		t=1<<(bit-1);
		switch (Port)
		{
		case PORT_A: IO_A |= t;break;
		case PORT_B: IO_B |= t;break;
		case PORT_C: IO_C |= t;break;
		case PORT_F: IO_F |= t;break;
		case PORT_T: IO_T |= t;break;
		}
	}
	else
	{
		t=0xFF - (1<<(bit-1));
		switch (Port)
		{
		case PORT_A: IO_A &= t;break;
		case PORT_B: IO_B &= t;break;
		case PORT_C: IO_C &= t;break;
		case PORT_F: IO_F &= t;break;
		case PORT_T: IO_T &= t;break;
		}
	}
}



/*****************************************************************************/
/* Get data from mem[]														 */
/*****************************************************************************/
BYTE CpcXXXX::Get_PC(DWORD adr)
{
    if (Chk_Adr_R(&adr,bREAD))
        return(mem[adr]);
    else
        return(0);
}
BYTE CpcXXXX::Get_8(DWORD adr)
{
    if (Chk_Adr_R(&adr,bREAD))
        return(mem[adr]);
    else
        return(0);
}

WORD CpcXXXX::Get_16(DWORD adr)
{
	DWORD	a;
	a=adr+1;
    if (Chk_Adr_R(&adr,bREAD) && Chk_Adr_R(&a,bREAD))
        return(mem[adr]+(mem[a]<<8));
    else
        return(0);
}

WORD CpcXXXX::Get_16r(DWORD adr)
{
	DWORD	a;
	a=adr+1;
    if (Chk_Adr_R(&adr,bREAD) && Chk_Adr_R(&a,bREAD))
        return((mem[adr]<<8)+mem[a]);
    else
        return(0);
}

WORD CpcXXXX::Get_16rPC(DWORD adr)
{
    DWORD	a;
    a=adr+1;
    if (Chk_Adr_R(&adr,bREAD) && Chk_Adr_R(&a,bREAD))
        return((mem[adr]<<8)+mem[a]);
    else
        return(0);
}
/*****************************************************************************/
/* Set data to mem[]														 */
/*  ENTRY :DWORD adr=RAM address, BYTE(8),WORD(16),DWORD(20,24) d=data		 */
/*****************************************************************************/
void CpcXXXX::Set_8(DWORD adr,BYTE d)
{
	if(Chk_Adr(&adr,d))
		mem[adr]=d;
}

void CpcXXXX::Set_16(DWORD adr,WORD d)
{
	DWORD	a;
	a=adr;
	if(Chk_Adr(&a,d)) mem[a]=(BYTE) d;
    a=adr+1;
    if(Chk_Adr(&a,(d>>8))) mem[a]=(BYTE) (d>>8);
}
 
void CpcXXXX::Set_16r(DWORD adr,WORD d)
{
    DWORD	a;
    a=adr;
    if(Chk_Adr(&a,(d>>8))) mem[a]=(BYTE) (d>>8);
    a=adr+1;
    if(Chk_Adr(&a,d)) mem[a]=(BYTE) d;
}

/*****************************************************************************/
/* RETURN: 0=error, 1=success												 */
/*****************************************************************************/
bool CpcXXXX::init(void)
{
#ifndef QT_NO_DEBUG
    pCPU->logsw = true;
#endif
	CPObject::init();
	
	AddLog(LOG_FUNC,"CpcXXXX::init");
	
	AddLog(LOG_MASTER,tr("Memory initialisation"));
	if((mem=(BYTE *)malloc(memsize*sizeof(BYTE)))==NULL) return(0);		/* alloc main ram */
	ClearRam(InitMemValue);
	AddLog(LOG_MASTER,tr("Success"));

	AddLog(LOG_MASTER,tr("CPU init"));
	if(!(pCPU->init())) return(0);
	AddLog(LOG_MASTER,"Success");

	AddLog(LOG_MASTER,tr("LCD init"));
	if(!(pLCDC->init())) return(0);
	AddLog(LOG_MASTER,"Success");

	AddLog(LOG_MASTER,tr("Memory loading nb slot:%1").arg(SlotList.size()));
	for (int s=0; s < SlotList.size(); ++s)
	{
		if (SlotList[s].getType() == ROM)	Mem_Load(s);
	}
	AddLog(LOG_MASTER,"Success");


	if(pKEYB)	pKEYB->init();
	if(pTIMER)	pTIMER->init();
	if(pLCDC)	pLCDC->init_screen();

	WatchPoint.add(&pCONNECTOR_value,64,11,this,"Standard 11pins connector");
	WatchPoint.add((qint64 *) &IO_A,8,8,this,"Port A");
	WatchPoint.add((qint64 *) &IO_B,8,8,this,"Port B");
	WatchPoint.add((qint64 *) &IO_C,8,8,this,"Port C");
	WatchPoint.add((qint64 *) &IO_F,8,8,this,"Port F");
	
	initsound();

	return(1);
}

/*****************************************************************************/
/* RETURN: 0=error, 1=success												 */
/*****************************************************************************/
bool CpcXXXX::exit(void)
{
	TurnOFF();

	if (pLCDC)		pLCDC->exit();
	if (pCPU)		pCPU->exit();
	if (pTIMER)		pTIMER->exit();
	if (pCONNECTOR) pCONNECTOR->exit();
	if (pKEYB)		pKEYB->exit();

	exitsound();
	return(1);
}


/*****************************************************************************/
/* Execute XXXX emulator (while pPC->pCPU->end==0)							 */
/* RETURN: 0=error, 1=success												 */
/*****************************************************************************/
bool CpcXXXX::run(void)
{	
	
	old_state = pTIMER->state;

	// Read the connectors
	pCONNECTOR_value = pCONNECTOR->Get_values();

	Get_Connector();

	if(!pCPU->halt && !off)
	{
//		if (g_DasmStep)
//		{
//			pCPU->pDEBUG->DisAsm_1(pCPU->get_PC());
//			RefreshDasm();
//		}

        if (pCPU->logsw) {
			pCPU->pDEBUG->DisAsm_1(pCPU->get_PC());
        }

		pCPU->step();

#if 1
		if ( (pCPU->logsw) && (pCPU->fp_log) )
		{
            fprintf(pCPU->fp_log,"[%lld] ",pTIMER->state);
            Regs_Info(1);

            fprintf(pCPU->fp_log,"[%02i]",pCPU->prevCallSubLevel);
            for (int g=0;g<pCPU->prevCallSubLevel;g++) fprintf(pCPU->fp_log,"\t");

            fprintf(pCPU->fp_log,"%-40s   %s  ",
				pCPU->pDEBUG->Buffer,pCPU->Regs_String);
#if 0
            for (int r=0x00;r<0x5c;r++){
                if ((r&0x08)==0x08) fprintf(pCPU->fp_log," R[%02x]:",r);
                fprintf(pCPU->fp_log,"%02x ",pCPU->imem[r]);

            }
#endif
            fprintf(pCPU->fp_log," a:%02x b:%02x c:%02x f:%02x\n",
                    pCPU->imem[0x5C],
                    pCPU->imem[0x5D],
                    pCPU->imem[0x5F],
                    pCPU->imem[0x5E]
                    );

            if (pCPU->prevCallSubLevel < pCPU->CallSubLevel) {
                for (int g=0;g<pCPU->prevCallSubLevel;g++) fprintf(pCPU->fp_log,"\t");
                fprintf(pCPU->fp_log,"{\n");
            }
            if (pCPU->prevCallSubLevel > pCPU->CallSubLevel) {
                for (int g=0;g<(pCPU->prevCallSubLevel-1);g++) fprintf(pCPU->fp_log,"\t");
                fprintf(pCPU->fp_log,"}\n");
            }
            if (pCPU->CallSubLevel <0) pCPU->CallSubLevel=0;
            pCPU->prevCallSubLevel = pCPU->CallSubLevel;
            fflush(pCPU->fp_log);
		}

		if (g_BreakPointAdr == (pCPU->get_PC()))
		{
			pCPU->pDEBUG->DisAsm_1(pCPU->get_PC());
//--			ListBox_ResetContent(g_hWndListDasm);
			g_DasmFlag = 1;
			RefreshDasm();
			pCPU->halt = 1;
		}
#endif
	}
    else pTIMER->state+=100;// = pTIMER->currentState();//qint64) ( mainwindow->rawclk * (pTIMER->CPUSpeed *(getfrequency() / 1000L)) );



//	if (g_DasmStep)		// Stop after 1 step processed (DASM)
//	{
//		pCPU->halt = 1;
//		g_DasmStep = 0;
//	}


	Set_Connector();		//Write the connectors
	
	return(1);
}

//////////////////////////////////////////////
// Save Memory to file						//
//  ENTRY :BYTE s=Slot No.(SLOT1-3)			//
//  RETURN:none								//
//////////////////////////////////////////////
void CpcXXXX::Mem_Save(QFile *file,BYTE s)
{
	QDataStream out(file);	
	out.writeRawData( (char *) &mem[SlotList[s].getAdr()],SlotList[s].getSize() * 1024 );
}

void CpcXXXX::Mem_Save_XML(QFile *file,BYTE s)
{
    QDataStream out(file);

    QByteArray ba((char *) &mem[SlotList[s].getAdr()],SlotList[s].getSize() * 1024 );
    out.writeRawData(ba.toBase64().data(),ba.size());
}

void CpcXXXX::Mem_Save(BYTE s)
{
	QFile file( SlotList[s].getFileName() );

	if (file.open(QIODevice::WriteOnly))
	{
		Mem_Save(&file,s);
		file.close();	
		return;
	}
}

bool CpcXXXX::SaveSession_XML(QFile *file)
{

    XmlWriter xw(file);


    xw.setAutoNewLine( true );
    xw.writeRaw( "<!DOCTYPE PKM>");
    xw.writeRaw("<PKM version=\"1.0\">" );
    xw.newLine();
    xw.writeTaggedString( "model", SessionHeader );
    xw.writeOpenTag( "config" );

    //SaveConfig(&xw);									// Write PC configuration
    pCPU->save_internal(file);							// Save cpu status
    for (int s=0; s<SlotList.size(); s++)				// Save Memory
    {
        if (SlotList[s].getType() == RAM)	Mem_Save_XML(file,s);
    }

    //SaveExtra(&xw);									// Save all other data  (virtual)
    return(1);
}

bool CpcXXXX::SaveSession_File(QFile *file)
{
	QDataStream out(file);	

	out.writeRawData( (char*)(SessionHeader.toLocal8Bit().data() ),SessionHeaderLen);	// Write Header
	SaveConfig(file);									// Write PC configuration
	pCPU->save_internal(file);							// Save cpu status
	for (int s=0; s<SlotList.size(); s++)				// Save Memory
	{
		if (SlotList[s].getType() == RAM)	Mem_Save(file,s);
	}
	
	SaveExtra(file);									// Save all other data  (virtual)
	return(1);
}

bool CpcXXXX::LoadSession_File(QFile *file)
{
	char t[20];

	// Read Header
	file->read( t, SessionHeaderLen );
	t[SessionHeaderLen] = '\0';
	//MSG_ERROR(QString(t))
	if(QString(t) != SessionHeader)
	{		//bad image
		MSG_ERROR(tr("Not the correct file format"));
		return false;
	}
	else
	{
		LoadConfig(file);												// Write PC configuration

		// Load cpu status	
		pCPU->Load_Internal(file);
		// Save Memory
		for (int s=0; s<SlotList.size(); s++)
		{
            if (SlotList[s].getType() == RAM)
                Mem_Load(file,s);
		}
		// Close the file
		return(1);
	}

	return false;
}

// Load PC Configuration
bool CpcXXXX::LoadConfig(QFile *file)
{
	QDataStream in(file);
    qint8 ioA,ioB,ioC,ioF,romb,ramb,protect,ks;
		
    in >> ioA >> ioB >> ioC >> ioF >> romb >> ramb >> protect >> Japan;
	IO_A = ioA;
	IO_B = ioB;
	IO_C = ioC;
	IO_F = ioF;
	RomBank = romb;
	RamBank = ramb;
    ProtectMemory = protect;

//    if (pKEYB) {
//        in >> ks;
//        pKEYB->Set_KS(ks);
//    }
	return true;
}

// Save PC Configuration
bool CpcXXXX::SaveConfig(QFile *file)
{
	QDataStream out(file);
		
	out << (qint8)IO_A << (qint8)IO_B << (qint8)IO_C << (qint8)IO_F;
    out << (qint8)RomBank << (qint8)RamBank << (qint8)ProtectMemory;
	out << Japan;
//    if (pKEYB) out << (qint8) pKEYB->KStrobe;

	return true;
}	

bool CpcXXXX::LoadExtra(QFile *file) 	{ return true; }
bool CpcXXXX::SaveExtra(QFile *file)	{ return true; }

void CpcXXXX::LoadSession(void)
{
	QString fileName = QFileDialog::getOpenFileName(
										mainwindow,
										tr("Choose a file"),
										".",
										tr("PockEmul sessions (*.pkm)"));
                    
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(mainwindow,tr("PockEmul"),
								tr("Cannot read file %1:\n%2.")
								.arg(file.fileName())
								.arg(file.errorString()));
		return ;
	}

	if (LoadSession_File(&file)) pLCDC->Update();
	file.close();							// Close the file

}

void CpcXXXX::SaveSession(void)
{
QString fileName = QFileDialog::getSaveFileName(
                    mainwindow,
                    tr("Choose a file"),
                    ".",
                    tr("PockEmul sessions (*.pkm)"));
                    
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(mainwindow,tr("PockEmul"),
								tr("Cannot write file %1:\n%2.")
								.arg(file.fileName())
								.arg(file.errorString()));
		return ;
	}

	SaveSession_File(&file);

	file.close();							// Close the file
}



/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:bool (1=RAM,0=ROM)												 */
/*****************************************************************************/
// Virtual Fonction
bool CpcXXXX::Chk_Adr(DWORD *d,DWORD data) { return(1); }
bool CpcXXXX::Chk_Adr_R(DWORD *d,DWORD data) { return(1);}


//////////////////////////////////////////////
// Load Memory from file					//
//  ENTRY :BYTE s=Slot No.(SLOT1-3)			//
//  RETURN:1:success, 0:error				//
//////////////////////////////////////////////
void CpcXXXX::Mem_Load(QFile *file,BYTE s)
{
	QDataStream in(file);	
	in.readRawData ((char *) &mem[SlotList[s].getAdr()],SlotList[s].getSize() * 1024 );
}

bool CpcXXXX::Mem_Load(BYTE s)
{
	QFile file;

	file.setFileName(SlotList[s].getFileName());
	
    if (file.exists())
    {
		if (file.size() == (SlotList[s].getSize() * 1024) )
		{
            file.open(QIODevice::ReadOnly);
			Mem_Load(&file,s);
			AddLog(LOG_MASTER,tr("loaded from file : %1").arg(SlotList[s].getFileName()));
			return true;
		}
//		else
//			AddLog(LOG_MASTER,tr("FileSize[%1] = %2  instead of %3 ").arg(SlotName[s]).arg(file.size().arg((SlotSize[s] * 1024)));

	}
	else
	{
		file.setFileName(SlotList[s].getResID());
	    if (file.exists())
		{
			file.open(QIODevice::ReadOnly);
			Mem_Load(&file,s);
			AddLog(LOG_MASTER,"Loaded from ressource");
			return true;
		}
	}
	return false;
}





bool CpcXXXX::Initial_Session_Load()
{
	QFile file(Initial_Session_Fname);

	if (file.open(QIODevice::ReadOnly))
	{
        if (LoadSession_File(&file))
            pLCDC->Update();
		file.close();	
		return true;
	}

	return false;
}

bool CpcXXXX::Initial_Session_Save()
{
	QFile file(Initial_Session_Fname);
	if (file.open(QIODevice::WriteOnly))
	{
		SaveSession_File(&file);
		file.close();	
		return true;
	}

	return false;
}

void CpcXXXX::Regs_Info(UINT8 Type)
{
	switch(Type)
	{
	case 0:			// Monitor Registers Dialog
			pCPU->Regs_Info(0);
			strcpy(Regs_String,pCPU->Regs_String);
			break;
    case 1:			// Monitor Registers Dialog
            pCPU->Regs_Info(1);
            strcpy(Regs_String,pCPU->Regs_String);
            break;
	}
}

void CpcXXXX::manageEmptyExtensions(void)
{
	emptyExtensionArray((QAction *)sender());
	updateMenuFromExtension();

}
void CpcXXXX::manageLoadExtensions(void)
{
    emptyExtensionArray((QAction *)sender());
    updateMenuFromExtension();

}
void CpcXXXX::manageSaveExtensions(void)
{
    emptyExtensionArray((QAction *)sender());
    updateMenuFromExtension();

}
void CpcXXXX::manageExtensions(QAction * action)
{
	updateExtension(action);
	updateMenuFromExtension();

}

