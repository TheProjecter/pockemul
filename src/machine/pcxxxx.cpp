//#include	<string.h>
//#include	<stdlib.h>

#include <QFile>
#include <QFileDialog>
#include <QPainter>
#include <QResizeEvent>

#include "common.h"
#include "pcxxxx.h"
#include "Inter.h"
#include "Lcdc.h"
#include "cpu.h"
#include "Connect.h"
#include "Log.h"
#include "Dasm.h"
#include "Debug.h"
#include "init.h"
#include "dialoganalog.h"
#include "cextension.h"
#include "Keyb.h"
#include "xmlwriter.h"
#include "ui/cregcpu.h"

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
    off			= true;
    DialogExtensionID = 0;
    setcfgfname(QString("pcXXXX"));
    SessionHeader	= "PCXXXXPKM";


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
    ext_MemSlot3	= 0;
    ext_Serial		= 0;
    ext_60pins		= 0;

    setPosX(0);
    setPosY(0);

    ioFreq = 24000;
    DasmFlag = false;
    DasmStep = false;
    BreakPoints.clear();
    BreakSubLevel = -1;

}

CpcXXXX::~CpcXXXX()
{
    free(mem);
    delete pCONNECTOR;
    delete pSIOCONNECTOR;
    delete pCPU;
//    delete ext_MemSlot1;
//    delete ext_MemSlot2;
//    delete ext_MemSlot3;
}

bool CpcXXXX::UpdateFinalImage(void)
{

//    CPObject::UpdateFinalImage();
    // Paint FinalImage
    QRect                        destRect,srcRect;
    int x,y,z,t;

    QPainter painter;

    if (pLCDC) {
        if (pLCDC->Refresh == FALSE) return false;
    }
    if ( (BackgroundImage) )
    {
        painter.begin(FinalImage);
        painter.drawImage(QPoint(0,0),*BackgroundImage);

        //if (pPC->pLCDC->On)
        {
            if (SymbImage) {
                //painter.setRenderHint(QPainter::Antialiasing);
                x = Lcd_Symb_X + Pc_Offset_X;
                y = Lcd_Symb_Y + Pc_Offset_Y;
                z = (int) (Lcd_Symb_DX * Lcd_Symb_ratio_X);
                t = (int) (Lcd_Symb_DY * Lcd_Symb_ratio_Y);

                painter.drawImage(QRect(x,y,z,t),SymbImage->scaled(z,t,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            }
            x	= Lcd_X + Pc_Offset_X;
            y	= Lcd_Y + Pc_Offset_Y;
            z	= (int) (Lcd_DX * Lcd_ratio_X);
            t	= (int) (Lcd_DY * Lcd_ratio_Y);
            painter.drawImage(QRect(x,y,z,t),LcdImage->scaled(z,t,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

        }
        painter.end();

        if (pLCDC) pLCDC->Refresh = FALSE;
    }
    Refresh_Display = false;

    return true;
}
	
bool CpcXXXX::CompleteDisplay(void)
{
	return TRUE;
}


bool CpcXXXX::InitDisplay(void)
{
    CPObject::InitDisplay();

    Refresh_Display = true;
    UpdateDisplayRunning = FALSE;
    global_w = getDX();
    global_h = getDY();

    delete LcdImage;
    LcdImage				= LoadImage(QSize(Lcd_DX, Lcd_DY),LcdFname);
    if (!SymbFname.isEmpty()) {
        delete SymbImage;
        SymbImage	= LoadImage(QSize(Lcd_Symb_DX, Lcd_Symb_DY),SymbFname);
    }
    UpdateDisplayRunning = TRUE;
    return(1);
}

void CpcXXXX::TurnOFF(void)
{
#ifdef EMSCRIPTEN
    mainwindow->saveAll=NO;
#endif
    switch (mainwindow->saveAll) {
    case ASK:
        if ( QMessageBox::question(this, "PockEmul",
                                                         "Do you want to save the session ?",
                                                         "Yes",
                                                         "No", 0, 0, 1) == 0) {
            Initial_Session_Save();
        };
        break;
    case YES: Initial_Session_Save(); break;
    default : break;

    }


    off = 1;
    Power = false;
    PowerSwitch = PS_OFF;
    if (pLCDC) pLCDC->TurnOFF();
    InitDisplay();
}


void CpcXXXX::TurnON(void)
{
    if (pKEYB->LastKey == 0) pKEYB->LastKey = K_POW_ON;
    if ( (pKEYB->LastKey == K_POW_ON) ||
         (Power && pKEYB->LastKey == K_OF) ||
         (!Power && pKEYB->LastKey == K_BRK))
    {
        AddLog(LOG_MASTER,"Power ON");
        if (!hardreset) {
            Initial_Session_Load();
        }
        else hardreset = false;
        off = 0;
        Power = true;
        PowerSwitch = PS_RUN;
        if (pLCDC) pLCDC->TurnON();
    }
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
    DWORD extValue = 0;
    if (Chk_Adr_R(&adr,&extValue))
        return(mem[adr]);
    else
        return(extValue);
}
BYTE CpcXXXX::Get_8(DWORD adr)
{
    DWORD extValue = 0;
    if (Chk_Adr_R(&adr,&extValue))
        return(mem[adr]);
    else
        return(extValue);
}

WORD CpcXXXX::Get_16(DWORD adr)
{
    DWORD extValue1 = 0;
    DWORD extValue2 = 0;
	DWORD	a;
	a=adr+1;
    if (Chk_Adr_R(&adr,&extValue1) && Chk_Adr_R(&a,&extValue2))
        return(mem[adr]+(mem[a]<<8));
    else
        return(extValue1+(extValue2<<8));
}

WORD CpcXXXX::Get_16r(DWORD adr)
{
    DWORD extValue1 = 0;
    DWORD extValue2 = 0;
	DWORD	a;
	a=adr+1;
    if (Chk_Adr_R(&adr,&extValue1) && Chk_Adr_R(&a,&extValue2))
        return((mem[adr]<<8)+mem[a]);
    else
        return((extValue1<<8)+extValue2);
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

DWORD CpcXXXX::Get_20(DWORD adr)
{
    Chk_Adr_R(&adr,bREAD);
    DWORD data = (mem[adr]+(mem[adr+1]<<8)+(mem[adr+2]<<16))&MASK_20;
    return(data);
}

DWORD CpcXXXX::Get_24(DWORD adr)
{
    Chk_Adr_R(&adr,bREAD);
    return((mem[adr]+(mem[adr+1]<<8)+(mem[adr+2]<<16))&MASK_24);
}

DWORD CpcXXXX::get_mem(DWORD adr,int size)
{
    switch(size)
    {
    case SIZE_8 :return(Get_PC(adr));
    case SIZE_16:return(Get_PC(adr)+(Get_PC(adr+1)<<8));
    case SIZE_20:return((Get_PC(adr)+(Get_PC(adr+1)<<8)+(Get_PC(adr+2)<<16))&MASK_20);
    case SIZE_24:return((Get_PC(adr)+(Get_PC(adr+1)<<8)+(Get_PC(adr+2)<<16))&MASK_24);
    }
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

void CpcXXXX::Set_20(DWORD adr, DWORD d)
{
    DWORD	a;
    a=adr;
    if(Chk_Adr(&a,d)) mem[a]=d;
    a=++adr;
    if(Chk_Adr(&a,(d>>8))) mem[a]=(d>>8);
    a=++adr;
    if(Chk_Adr(&a,(d>>16)&MASK_4)) mem[a]=(d>>16)&MASK_4;
}

void CpcXXXX::Set_24(DWORD adr, DWORD d)
{
    DWORD	a;
    a=adr;
    if(Chk_Adr(&a,d)) mem[a]=d;
    a=++adr;
    if(Chk_Adr(&a,(d>>8))) mem[a]=(d>>8);
    a=++adr;
    if(Chk_Adr(&a,(d>>16))) mem[a]=(d>>16);
}

void CpcXXXX::set_mem(DWORD adr,int size,DWORD data)
{
    switch(size)
    {
    case SIZE_8 :
        Set_8(adr , (BYTE) data);
        break;
    case SIZE_16:
        Set_8(adr , (BYTE) data);
        Set_8(adr+1 , (BYTE) (data>>8));
        break;
    case SIZE_20:
        Set_8(adr , (BYTE) data);
        Set_8(adr+1 , (BYTE) (data>>8));
        Set_8(adr+2 , (BYTE) ((data>>16)&MASK_4));
        break;
    case SIZE_24:
        Set_8(adr , (BYTE) data);
        Set_8(adr+1 , (BYTE) (data>>8));
        Set_8(adr+2 , (BYTE) (data>>16));
        break;
    }
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
	


	AddLog(LOG_MASTER,tr("LCD init"));
    if(pLCDC && !(pLCDC->init())) return(0);



    AddLog(LOG_MASTER,tr("CPU init"));
    if (!pCPU) return 0;
    if(!(pCPU->init())) return(0);
	if(pKEYB)	pKEYB->init();
	if(pTIMER)	pTIMER->init();
	if(pLCDC)	pLCDC->init_screen();
    AddLog(LOG_MASTER,"Success");

    QHash<int,QString> lbl;
    lbl[1]="MT_OUT2";
    lbl[2]="GND";
    lbl[3]="VGG";
    lbl[4]="BUSY";
    lbl[5]="D_OUT";
    lbl[6]="MT_IN";
    lbl[7]="MT_OUT1";
    lbl[8]="D_IN";
    lbl[9]="ACK";
    lbl[10]="SEL2";
    lbl[11]="SEL1";
    WatchPoint.add(&pCONNECTOR_value,64,11,this,"Standard 11pins connector",lbl);
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
    CPObject::run();

	old_state = pTIMER->state;

    Get_Connector();
    // Read the connectors
    if (pCONNECTOR) pCONNECTOR_value = pCONNECTOR->Get_values();

    if (DasmFlag) return true;

	if(!pCPU->halt && !off)
	{
        if ( (pCPU->logsw) && (pCPU->fp_log) ) {
            pCPU->pDEBUG->DisAsm_1(pCPU->get_PC());
            fprintf(pCPU->fp_log,"[%lld] ",pTIMER->state);
            fprintf(pCPU->fp_log,"[%02i]",pCPU->prevCallSubLevel);
            for (int g=0;g<pCPU->prevCallSubLevel;g++) fprintf(pCPU->fp_log,"\t");

            pCPU->step();
            Regs_Info(1);

            fprintf(pCPU->fp_log,"%-40s   %s  \n",pCPU->pDEBUG->Buffer,pCPU->Regs_String);
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

//            fflush(pCPU->fp_log);
        }
        else {
            if (!off) pCPU->step();
#ifndef QT_NO_DEBUG
            Regs_Info(0);
#endif
        }

        if (BreakPoints.value(pCPU->get_PC(),Qt::Unchecked) == Qt::Checked)
        {
                DasmStep = true;
                BreakSubLevel = 99999;
        }
        if (DasmStep)
        {
            if ( (pCPU->get_PC() != DasmLastAdr) &&
                 (BreakSubLevel >= pCPU->CallSubLevel)){
                BreakSubLevel = pCPU->CallSubLevel;
                DasmLastAdr = pCPU->get_PC();
                pCPU->pDEBUG->DisAsm_1(DasmLastAdr);
                emit RefreshDasm();
                DasmFlag = true;
                DasmStep = false;
            }
        }

	}
    else {
        if (!off) {
            pCPU->step();
            pTIMER->state+=10;// = pTIMER->currentState();//qint64) ( mainwindow->rawclk * (pTIMER->CPUSpeed *(getfrequency() / 1000L)) );
        }
    }

    Set_Connector();		//Write the connectors
    if (pCONNECTOR) {
        pCONNECTOR_value = pCONNECTOR->Get_values();
    }

//	if (DasmStep)		// Stop after 1 step processed (DASM)
//	{
//		pCPU->halt = 1;
//		DasmStep = 0;
//	}

	return(1);
}



bool CpcXXXX::SaveSession_File(QXmlStreamWriter *xmlOut) {
    xmlOut->writeStartElement("session");
        xmlOut->writeAttribute("version", "2.0");
        xmlOut->writeAttribute("model", SessionHeader );
        SaveConfig(xmlOut);
        SaveExt(xmlOut);
        pCPU->save_internal(xmlOut);
        xmlOut->writeStartElement("memory");
            for (int s=0; s<SlotList.size(); s++)				// Save Memory
            {
                if (SlotList[s].getType() == RAM)	Mem_Save(xmlOut,s);
            }
        xmlOut->writeEndElement();  // memory
    xmlOut->writeEndElement();  // session
//    SaveExtra(&xw);									// Save all other data  (virtual)
    return true;
}

bool CpcXXXX::SaveSession_File(QFile *file)
{
	QDataStream out(file);	

    out.writeRawData( (char*)(SessionHeader.toLocal8Bit().data() ),SessionHeader.length());	// Write Header
	SaveConfig(file);									// Write PC configuration
	pCPU->save_internal(file);							// Save cpu status
	for (int s=0; s<SlotList.size(); s++)				// Save Memory
	{
		if (SlotList[s].getType() == RAM)	Mem_Save(file,s);
	}
	
	SaveExtra(file);									// Save all other data  (virtual)
	return(1);
}

bool CpcXXXX::LoadSession_File(QXmlStreamReader *xmlIn) {

    if ((xmlIn->name()=="session") || (xmlIn->readNextStartElement())) {
        if ( (xmlIn->name() == "session") &&
             (xmlIn->attributes().value("model") == SessionHeader) ) {
            QString version = xmlIn->attributes().value("version").toString();
            if (!LoadConfig(xmlIn)) {
                emit msgError("ERROR Loading Session Config:"+SessionHeader);
                return false;
            }
            if ( (version == "2.0") && !LoadExt(xmlIn)) {
                MSG_ERROR("ERROR Loading Session Extensions");
                return false;
            }

            pCPU->Load_Internal(xmlIn);
            AddLog(LOG_MASTER,"Loadmemory:"+xmlIn->name().toString());
            if (xmlIn->readNextStartElement() && xmlIn->name() == "memory" ) {
                AddLog(LOG_MASTER,"Load Memory");
                for (int s=0; s<SlotList.size(); s++)				// Save Memory
                {
                    if (SlotList[s].getType() == RAM) {
                        AddLog(LOG_MASTER,"    Load Slot"+SlotList[s].getLabel());
                        Mem_Load(xmlIn,s);
                    }
                }
            }
        }
    }

    updateMenuFromExtension();
    return true;
}

bool CpcXXXX::LoadSession_File(QFile *file)
{
	char t[20];

	// Read Header
    file->read( t, SessionHeader.length() );
    t[SessionHeader.length()] = '\0';
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
    qint8 ioA,ioB,ioC,ioF,romb,ramb,protect;
		
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

bool CpcXXXX::LoadConfig(QXmlStreamReader *xmlIn)
{
    if (xmlIn->readNextStartElement()) {
        if (xmlIn->name() == "config" && xmlIn->attributes().value("version") == "1.0") {
            if (xmlIn->readNextStartElement() && xmlIn->name() == "internal" ) {
                QString s= xmlIn->attributes().value("IO_A").toString();
                IO_A = s.toInt(0,16);
                IO_B = xmlIn->attributes().value("IO_B").toString().toInt(0,16);
                IO_C = xmlIn->attributes().value("IO_C").toString().toInt(0,16);
                IO_F = xmlIn->attributes().value("IO_F").toString().toInt(0,16);
                RomBank = xmlIn->attributes().value("RomBank").toString().toInt(0,16);
                RamBank = xmlIn->attributes().value("RamBank").toString().toInt(0,16);
                ProtectMemory = xmlIn->attributes().value("ProtectMemory").toString().toInt(0,16);
                //Japan = xml->attributes().value("ProtectMemory").toString().toInt(0,16);
                closed = xmlIn->attributes().value("closed").toString().toInt(0,16);
                xmlIn->skipCurrentElement();
            }

        }
        xmlIn->skipCurrentElement();
    }

    return true;
}

// Save PC Configuration
bool CpcXXXX::SaveConfig(QFile *file) {
	QDataStream out(file);
		
	out << (qint8)IO_A << (qint8)IO_B << (qint8)IO_C << (qint8)IO_F;
    out << (qint8)RomBank << (qint8)RamBank << (qint8)ProtectMemory;
	out << Japan;
//    if (pKEYB) out << (qint8) pKEYB->KStrobe;

	return true;
}	

bool CpcXXXX::SaveConfig(QXmlStreamWriter *xmlOut)
{
    xmlOut->writeStartElement("config");
    xmlOut->writeAttribute("version", "1.0");
        xmlOut->writeStartElement("internal");
            xmlOut->writeAttribute("IO_A",QString("%1").arg(IO_A,2,16));
            xmlOut->writeAttribute("IO_B",QString("%1").arg(IO_B,2,16));
            xmlOut->writeAttribute("IO_C",QString("%1").arg(IO_C,2,16));
            xmlOut->writeAttribute("IO_F",QString("%1").arg(IO_F,2,16));
            xmlOut->writeAttribute("RomBank",QString("%1").arg(RomBank,2,16));
            xmlOut->writeAttribute("RamBank",QString("%1").arg(RamBank,2,16));
            xmlOut->writeAttribute("ProtectMemory",QString("%1").arg(ProtectMemory));
            xmlOut->writeAttribute("Japan",QString("%1").arg(Japan));
            xmlOut->writeAttribute("closed",QString("%1").arg(closed));
        xmlOut->writeEndElement();
    xmlOut->writeEndElement();

    return true;
}

bool CpcXXXX::LoadExt(QXmlStreamReader *xmlIn)
{
    AddLog(LOG_MASTER,"LoadExt");
    if (xmlIn->readNextStartElement()) {
        AddLog(LOG_MASTER,"Loadext name1:"+xmlIn->name().toString());
        if (xmlIn->name() == "extarray" && xmlIn->attributes().value("version") == "1.0") {

            while (xmlIn->readNextStartElement()) {
                AddLog(LOG_MASTER,"Loadext name2:"+xmlIn->name().toString());
                if ( xmlIn->name() == "ext" ) {

                    int i = xmlIn->attributes().value("idarray").toString().toInt(0,10);
                    QString Id = xmlIn->attributes().value("idext").toString();
                    AddLog(LOG_MASTER,"Found : "+Id);
                    for (int j = 0;j<NB_EXT;j++) {
                        if (extensionArray[i]->ExtArray[j]->Id == Id) {
                            extensionArray[i]->ExtArray[j]->IsChecked = true;
                            AddLog(LOG_MASTER,tr("Found : %1").arg(j));
                        }
                    }
                }
//                else
                    xmlIn->skipCurrentElement();
            }
            AddLog(LOG_MASTER,"Loadext end name:"+xmlIn->name().toString());
//            if (found) xmlIn->skipCurrentElement();
        }

    }

    return true;
}

bool CpcXXXX::SaveExt(QXmlStreamWriter *xmlOut)
{
    xmlOut->writeStartElement("extarray");
    xmlOut->writeAttribute("version", "1.0");
    for (int i = 0; i<4;i++) {
        if (extensionArray[i]) {
            for (int j = 0; j<NB_EXT;j++) {
                CExtension* e = extensionArray[i]->ExtArray[j];
                if (e && e->IsChecked) {
                    xmlOut->writeStartElement("ext");
                        xmlOut->writeAttribute("idarray",QString("%1").arg(i));
                        xmlOut->writeAttribute("idext",e->Id);
                        xmlOut->writeAttribute("desc",e->Description);
                    xmlOut->writeEndElement();
                }
            }
        }
    }
    xmlOut->writeEndElement();

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

    QXmlStreamReader xmlIn;

    xmlIn.setDevice(&file);
    if (LoadSession_File(&xmlIn) && pLCDC) pLCDC->Update();

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

    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QString s;
        QXmlStreamWriter *xmlOut = new QXmlStreamWriter(&s);
        xmlOut->setAutoFormatting(true);
        SaveSession_File(xmlOut);
        QTextStream out(&file);
        out << s;
    }

	file.close();							// Close the file
}



/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:bool (1=RAM,0=ROM)												 */
/*****************************************************************************/
// Virtual Fonction
bool CpcXXXX::Chk_Adr(DWORD *d,DWORD data) { return(1); }
bool CpcXXXX::Chk_Adr_R(DWORD *d,DWORD *data) { return(1);}






bool CpcXXXX::Initial_Session_Load()
{
	QFile file(Initial_Session_Fname);

	if (file.open(QIODevice::ReadOnly))
	{
        QXmlStreamReader xmlIn;
        xmlIn.setDevice(&file);
        if (LoadSession_File(&xmlIn) && pLCDC)
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
        QString s;
        QXmlStreamWriter *xmlOut = new QXmlStreamWriter(&s);
        xmlOut->setAutoFormatting(true);
        SaveSession_File(xmlOut);
        QTextStream out(&file);
        out << s;

        //SaveSession_File(&file);
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

/*!
 \brief

 \fn CpcXXXX::loadMem
 \param adr
 \param data
 \return bool
*/


QByteArray CpcXXXX::getmem()
{
    return (QByteArray((const char*) mem,memsize));
}






