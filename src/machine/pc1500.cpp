/*** POCKEMUL [PC1500.CPP] ********************************/
/* PC1500 emulator main class                             */
/**********************************************************/
#include	<string.h>
#include	<stdlib.h>

#include <QtGui> 

#include	"common.h"
#include	"pc1500.h"
 
#include	"Inter.h"
#include	"Debug.h"
#include	"Keyb.h"
#include    "Keyb1500.h"
#include	"ce152.h"
#include	"dialoganalog.h"

extern int	g_DasmStep;
extern bool	UpdateDisplayRunning;
 

Cpc15XX::Cpc15XX(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
	setfrequency( (int) 2600000/2);
	setcfgfname(QString("pc1500"));

	SessionHeader	= "PC1500PKM";
	SessionHeaderLen= 9;
	Initial_Session_Fname ="pc1500.pkm";
	BackGroundFname	= ":/PC1500/pc1500/pc1500.jpg";
	LcdFname		= ":/PC1500/pc1500/1500lcd.jpg";
	SymbFname		= ":/PC1500/pc1500/1500symb.jpg";
	memsize			= 0x26000;
	InitMemValue	= 0xFF;

	SlotList.clear();

	KeyMap		= KeyMap1500;
	KeyMapLenght= KeyMap1500Lenght;

	Pc_Offset_X = Pc_Offset_Y = 0;

	Pc_DX		= 572;
	Pc_DY		= 254;
		
	Lcd_X		= 80;
	Lcd_Y		= 50;
	Lcd_DX		= 156;
	Lcd_DY		= 8;
	Lcd_ratio_X	= 2;
	Lcd_ratio_Y	= 2;

	Lcd_Symb_X	= 80;
	Lcd_Symb_Y	= 43;
	Lcd_Symb_DX	= 316;
	Lcd_Symb_DY	= 5;

	DialogExtensionID = 0;//IDD_EXT_PROPERTIES_1500;

	SoundOn			= FALSE;
	lh5810_Access	= FALSE;
	ce150_Access	= FALSE;

	pLCDC		= new Clcdc_pc1500(this);
    pCPU		= new CLH5801(this); pCPU->logsw=false;
	pLH5810		= new CLH5810_PC1500(this);
	pTIMER		= new Ctimer(this);
    pCONNECTOR	= new Cconnector(this,60,"Connector 60 pins",false);	publish(pCONNECTOR);
    pKEYB		= new Ckeyb(this,"pc1500.map",scandef_pc1500);
	pce152		= new Cce152_PC15XX(this);
	delete pce152->pTIMER; pce152->pTIMER = pTIMER;
	
	ce150_connected = false;
	
	Tape_Base_Freq=2500;
	initExtension();
	extensionArray[0] = ext_60pins;
	extensionArray[1] = ext_MemSlot1;
}


void Cpc15XX::TurnON(void)
{
	AddLog(LOG_FUNC,"Cpc1500::TurnOn");

//--	remove(Initial_Session_Fname);
	pCPU->Reset();
	CpcXXXX::TurnON();
}

void	Cpc15XX::initExtension(void)
{
	// initialise ext_MemSlot1
	ext_MemSlot1 = new CExtensionArray("Memory Slot","Add memory module");
	ext_MemSlot1->setAvailable(ID_CE151,true);		ext_MemSlot1->setChecked(ID_CE151,false);
	ext_MemSlot1->setAvailable(ID_CE155,true);		ext_MemSlot1->setChecked(ID_CE155,true);
	ext_MemSlot1->setAvailable(ID_CE159,true);		ext_MemSlot1->setChecked(ID_CE159,false);
	ext_MemSlot1->setAvailable(ID_CE160,true);		ext_MemSlot1->setChecked(ID_CE160,false);
	ext_MemSlot1->setAvailable(ID_CE161,true);		ext_MemSlot1->setChecked(ID_CE161,false);

	addExtMenu(ext_MemSlot1);
}

bool Cpc15XX::CompleteDisplay(void)
{
	QPainter painter;

//	AddLog(LOG_FUNC,"Cpc1500::CompleteDisplay");
	
	CpcXXXX::CompleteDisplay();
	
	return TRUE;
}


bool Cpc15XX::InitDisplay(void)
{
	AddLog(LOG_FUNC,"Cpc1500::InitDisplay");

	CpcXXXX::InitDisplay();

	return(1);
}



bool Cpc15XX::LoadConfig(QFile *file)
{
	AddLog(LOG_FUNC,"Cpc1500::LoadConfig");

//--	fread(&Extension,1,sizeof(TExtension),fp);
	return(1);
}

bool Cpc15XX::SaveConfig(QFile *file)
{
	AddLog(LOG_FUNC,"Cpc1500::SaveConfig");

//--	fwrite(&Extension,1,sizeof(TExtension),fp);
	return(1);
}

bool Cpc15XX::init(void)				// initialize
{
	CpcXXXX::init();
	
#if 1
		// Init I/O Ce150 memory
		Set_8(0x1B000 , 0);
		Set_8(0x1B001 , 0);
		Set_8(0x1B002 , 0);
		Set_8(0x1B003 , 0);
		Set_8(0x1B004 , 0);
		Set_8(0x1B005 , 0);
		Set_8(0x1B006 , 0);
		Set_8(0x1B007 , 0);
		Set_8(0x1B008 , 0);
		Set_8(0x1B009 , 0);
		Set_8(0x1B00A , 0);
		Set_8(0x1B00B , 0);
		Set_8(0x1B00C , 0);
		Set_8(0x1B00D , 0);
		Set_8(0x1B00E , 0);
		Set_8(0x1B00F , 0);
#endif
#if 1
		// Init I/O LH5810 memory
		Set_8(0x1F000 , 0);
		Set_8(0x1F001 , 0);
		Set_8(0x1F002 , 0);
		Set_8(0x1F003 , 0);
		Set_8(0x1F004 , 0);
		Set_8(0x1F005 , 0);
		Set_8(0x1F006 , 0);
		Set_8(0x1F007 , 0);
		Set_8(0x1F008 , 0);
		Set_8(0x1F009 , 0);
		Set_8(0x1F00A , 0);
		Set_8(0x1F00B , 0);
		Set_8(0x1F00C , 0);
		Set_8(0x1F00D , 0);
		Set_8(0x1F00E , 0);
		Set_8(0x1F00F , 0);
#endif

	WatchPoint.remove(this);
	
	WatchPoint.add(&pCONNECTOR_value,64,60,this,"Standard 60pins connector");
	WatchPoint.add((qint64 *) &(pLH5810->lh5810.r_opa),8,8,this,"LH5810 Port A");
	WatchPoint.add((qint64 *) &(pLH5810->lh5810.r_opb),8,8,this,"LH5810 Port B");
	WatchPoint.add((qint64 *) &(pLH5810->lh5810.r_opc),8,8,this,"LH5810 Port C");

	return true;
}

bool Cpc15XX::run(void) 
{
	DWORD previous_pc;
	DWORD Current_PC;	

	previous_pc = pCPU->get_PC();

// ---------------------------------------------------------	
	CpcXXXX::run();
// ---------------------------------------------------------

	Current_PC = pCPU->get_PC();

	pLCDC->On = ((CLH5801 *)pCPU)->lh5801.dp;

	if (pKEYB->CheckKon()) 
		((CLH5801 *)pCPU)->lh5801.bf=1;

	if (pKEYB->Kon) lh5810_Access = TRUE;

	if (lh5810_Access)
	{
		lh5810_write();
		pLH5810->step();
		lh5810_read();
		lh5810_Access = FALSE;
	}


	if (pLH5810->INT==TRUE) 
		((CLH5801 *)pCPU)->lh5801.IR2=TRUE;

#if 0
	if (((CLH5801 *)pCPU)->lh5801.bf)
	{
		TurnON();
	}
	else
	{
//		TurnOFF();
	}
#endif

// HACK Program Counter
#define FUNC_CALL(ADDR,LIB) case ADDR: AddLog(LOG_ROM,tr(LIB).arg(((CLH5801 *)pCPU)->get_PC(),5,16,QChar('0')))

	switch (Current_PC)
	{
	case 0xBCE8:	// 2eme

//	AddLog(LOG_ROM,"Function Call [%05X]",((CLH5801 *)pCPU)->get_PC());
		//sprintf((char *) &(pPC->mem[0x7B91]),"MICRO TENNIS");
//	g_DasmStep=1;
		break;


    FUNC_CALL(0xA519,QT_TR_NOOP("Function Call [%1] - Change printer pen color"));				break;
	FUNC_CALL(0xA769,QT_TR_NOOP("Function Call [%1] - Printer motor OFF"));						break;
	FUNC_CALL(0xA781,QT_TR_NOOP("Function Call [%1] - Send ASCCI char to printer. No LF"));		break;
	FUNC_CALL(0xA8DD,QT_TR_NOOP("Function Call [%1] - Move PEN"));								break;
	FUNC_CALL(0xA9F1,QT_TR_NOOP("Function Call [%1] - Send Line Feed to printer"));				break;
	FUNC_CALL(0xAA04,QT_TR_NOOP("Function Call [%1] - Send (n) Line Feeds to printer"));		break;
	FUNC_CALL(0xAAE3,QT_TR_NOOP("Function Call [%1] - Pen UP/DOWN"));							break;
	FUNC_CALL(0xABCB,QT_TR_NOOP("Function Call [%1] - Switch Printer Graphic to Text mode"));	break;
	FUNC_CALL(0xABEF,QT_TR_NOOP("Function Call [%1] - Switch Printer Text to Graphic mode"));	break;
	FUNC_CALL(0xB8A6,QT_TR_NOOP("Function Call [%1] - CSAVE"));									break;
	FUNC_CALL(0xBF11,QT_TR_NOOP("Function Call [%1] - REMOTE ON  (Open/Close)"));				break;
	FUNC_CALL(0xBF43,QT_TR_NOOP("Function Call [%1] - REMOTE OFF"));							break;
	FUNC_CALL(0xBBF5,QT_TR_NOOP("Function Call [%1] - Termination of CMT I/O Control"));		break;
	FUNC_CALL(0xBD3C,QT_TR_NOOP("Function Call [%1] - File Transfer"));							break;
	FUNC_CALL(0xBDF3,QT_TR_NOOP("Function Call [%1] - Sortie Load Char"));						break;
	FUNC_CALL(0xBE02,QT_TR_NOOP("Function Call [%1] - Load Quarter")); 
//											pCPU->logsw=1;
											pCPU->Check_Log();
											ReadQuarterTape();
																								break;
	FUNC_CALL(0xBDF0,QT_TR_NOOP("Function Call [%1] - Load Char"));								break;
	FUNC_CALL(0xBDCC,QT_TR_NOOP("Function Call [%1] - CE150 Save one character"));				break;
	FUNC_CALL(0xBBD6,QT_TR_NOOP("Function Call [%1] - Creation of Header"));					break;
	}

	//----------------------------------
	// SOUND BUFFER (quite simple no?) 
	//----------------------------------
	fillSoundBuffer((pLH5810->lh5810.r_opc & 0x40 ? 0xff:0x00));
	//----------------------------------

	return(1); 
}

FILE *fp_tmp1;
void Cpc15XX::ReadQuarterTape(void)
{
	u_long quarter=0;
	u_long trans=0;
	BYTE bit;

	if (fp_tmp1==NULL) 
		fp_tmp1=fopen("LOG TAPE1500.txt","wb");
	// Loop until bit start
	do 
	{
		pce152->ReadBitFromWav(&trans,&pce152->info);
		if (trans > BIT_MID)
			bit = 1;     /* Bit a 1 */
		else
			bit = 0;     /* Bit a 0 */
		
		if (pCPU->fp_log) fprintf(pCPU->fp_log,"%c",(bit==1?'1':'0'));

	}
	while (bit==1);

	if (pCPU->fp_log) fprintf(pCPU->fp_log," ");

	for (int jj=0;jj<4;jj++) 
	{
		pce152->ReadBitFromWav(&trans,&pce152->info);
		if (trans > BIT_MID)
			bit = 1;     /* Bit a 1 */
		else
			bit = 0;     /* Bit a 0 */
		if (pCPU->fp_log) fprintf(pCPU->fp_log,"%c",(bit==1?'1':'0'));

		quarter |= (bit << (jj)); 
	}

	// 6 Bits stop
//	for (int ii=0;ii<6;ii++) 
//		ce152.ReadBitFromWav(&trans,&ce152.info);


	//		ce152.ReadByteFromWav(1,&tmpL,&ce152.info);
	//		ce152.ReadQuaterFromWav (&tmpL, &ce152.info) ;
	//		tmpL=((tmpL>>4)&0x0F) | ((tmpL&0x0F)<<4);


	if (fp_tmp1) fprintf(fp_tmp1,"   ** LOAD QUARTER BIT : %02X\n",quarter<<4);

 	((CLH5801 *)pCPU)->LDA(quarter<<4);
	((CLH5801 *)pCPU)->UnSet_C() ;
	((CLH5801 *)pCPU)->RTN();

}


INLINE bool Cpc15XX::lh5810_write(void)
{
//	AddLog(LOG_FUNC,"Cpc1500::lh5810_write");


	pLH5810->SetReg(LH5810_RESET,	Get_8(0x1F004));
	pLH5810->SetReg(LH5810_U,		Get_8(0x1F005));
	pLH5810->SetReg(LH5810_L,		Get_8(0x1F006));
	pLH5810->SetReg(LH5810_OPC,	Get_8(0x1F008));
	pLH5810->SetReg(LH5810_G  ,	Get_8(0x1F009));
	pLH5810->SetReg(LH5810_MSK,	Get_8(0x1F00A));
	pLH5810->SetReg(LH5810_IF ,	Get_8(0x1F00B));
	pLH5810->SetReg(LH5810_DDA,	Get_8(0x1F00C));
	pLH5810->SetReg(LH5810_DDB,	Get_8(0x1F00D));
	pLH5810->SetReg(LH5810_OPA,	Get_8(0x1F00E));
	pLH5810->SetReg(LH5810_OPB,	Get_8(0x1F00F));

	return(1);
}

INLINE bool Cpc15XX::lh5810_read(void)
{
//	AddLog(LOG_FUNC,"Cpc1500::lh5810_read");


	mem[0x1F005] = pLH5810->GetReg(LH5810_U);
	mem[0x1F006] = pLH5810->GetReg(LH5810_L);

	mem[0x1F008] = pLH5810->GetReg(LH5810_OPC);
	mem[0x1F009] = pLH5810->GetReg(LH5810_G);
	mem[0x1F00A] = pLH5810->GetReg(LH5810_MSK);
	mem[0x1F00B] = pLH5810->GetReg(LH5810_IF);
	mem[0x1F00C] = pLH5810->GetReg(LH5810_DDA);
	mem[0x1F00D] = pLH5810->GetReg(LH5810_DDB);
	mem[0x1F00E] = pLH5810->GetReg(LH5810_OPA);
	mem[0x1F00F] = pLH5810->GetReg(LH5810_OPB);

	pKEYB->Set_KS(Get_8(0x1F00C));
	
	return(1);
}

bool Cpc15XX::Mem_Mirror(DWORD *d) 
{
#if 1
	if ( (*d>=0x7000) && (*d<=0x71FF) )	{ *d+=0x600; return(1); }
	if ( (*d>=0x7200) && (*d<=0x73FF) )	{ *d+=0x400; return(1); }
	if ( (*d>=0x7400) && (*d<=0x75FF) )	{ *d+=0x200; return(1); }
#else
	if ( (*d>=0x7000) && (*d<=0x75FF) )	{ *d+=0x600; return(1); }
#endif
	if ( (*d>=0x7C00) && (*d<=0x7FFF) ) { *d-=0x400; return(1); }
	
	return(1);
} 

bool Cpc1500A::Mem_Mirror(DWORD *d) 
{
	if ( (*d>=0x7000) && (*d<=0x71FF) )	{ *d+=0x600; return(1); }
	if ( (*d>=0x7200) && (*d<=0x73FF) )	{ *d+=0x400; return(1); }
	if ( (*d>=0x7400) && (*d<=0x75FF) )	{ *d+=0x200; return(1); }
	
	return(1);
}

bool Cpc15XX::Chk_Adr(DWORD *d,DWORD data) 
{
	Mem_Mirror(d);

	if ( (*d>=0x0000) && (*d<=0x1FFF) )	{ return(EXTENSION_CE161_CHECK); }						// ROM area(0000-3FFF) 16K
	if ( (*d>=0x2000) && (*d<=0x37FF) )	{ return(EXTENSION_CE161_CHECK | EXTENSION_CE159_CHECK); }	// ROM area(0000-3FFF) 16K
	if ( (*d>=0x3800) && (*d<=0x3FFF) )	{ return(EXTENSION_CE161_CHECK | EXTENSION_CE159_CHECK| EXTENSION_CE155_CHECK); }		// ROM area(0000-3FFF) 16K
	if ( (*d>=0x4000) && (*d<=0x47FF) )	{ return(1); }										// RAM area(0000-3FFF) 16K
	if ( (*d>=0x4800) && (*d<=0x57FF) )	{ return(EXTENSION_CE155_CHECK | EXTENSION_CE151_CHECK); }	// RAM area(0000-3FFF) 16K
	if ( (*d>=0x5800) && (*d<=0x5FFF) )	{ return(EXTENSION_CE155_CHECK); }						// RAM area(0000-3FFF) 16K
	if ( (*d>=0x6000) && (*d<=0x6FFF) )	{ return(0); }										// ROM area(0000-3FFF) 16K
	if ( (*d>=0x7000) && (*d<=0x75FF) ) { return(0); }										// INHIBITED MIRRORING
	if ( (*d>=0x7600) && (*d<=0x77FF) ) { pLCDC->SetDirtyBuf(*d-0x7600);return(1);}
	if ( (*d>=0x7800) && (*d<=0x7BFF) ) { return(1); }										// RAM area(7800-7BFF)
	if ( (*d>=0x7C00) && (*d<=0x7FFF) ) { return(0); }										// INHIBITED MIRRORING
	if ( (*d>=0x8000) && (*d<=0x9FFF) ) { return(0); }										// RAM area(4000-7FFFF)
	if ( (*d>=0xA000) && (*d<=0xBFFF) ) { return(0); }										// RAM area(4000-7FFFF)
	if ( (*d>=0xC000) && (*d<=0xFFFF) ) { return(0); }										// RAM area(4000-7FFFF)
		
	if ( (*d>=0x18000)&&(*d<=0x1800F) ) { return(1); }
	if ( (*d>=0x1B000)&&(*d<=0x1B00F) ) { ce150_Access = TRUE;	return(1); }
	if ( (*d>=0x1D000)&&(*d<=0x1D00F) ) { return(1); }
	if ( (*d>=0x1F000)&&(*d<=0x1F00F) )	{ lh5810_Access = TRUE;if (*d==0x1F006) pLH5810->New_L=TRUE;return(1);}	// I/O area(LH5810)

	// else it's ROM
	return(0);
}

bool Cpc1500A::Chk_Adr(DWORD *d,DWORD data) 
{
	Mem_Mirror(d);

	if ( (*d>=0x0000) && (*d<=0x1FFF) )	{ return(EXTENSION_CE161_CHECK); }						// ROM area(0000-3FFF) 16K
	if ( (*d>=0x2000) && (*d<=0x37FF) )	{ return(EXTENSION_CE161_CHECK | EXTENSION_CE159_CHECK); }	// ROM area(0000-3FFF) 16K
	if ( (*d>=0x3800) && (*d<=0x3FFF) )	{ return(EXTENSION_CE161_CHECK | EXTENSION_CE159_CHECK | EXTENSION_CE155_CHECK); }		// ROM area(0000-3FFF) 16K
	if ( (*d>=0x4000) && (*d<=0x57FF) )	{ return(1); }										// RAM area(0000-3FFF) 16K
	if ( (*d>=0x5800) && (*d<=0x67FF) )	{ return(EXTENSION_CE155_CHECK | EXTENSION_CE151_CHECK); }	// RAM area(0000-3FFF) 16K
	if ( (*d>=0x6800) && (*d<=0x6FFF) )	{ return(EXTENSION_CE155_CHECK); }						// RAM area(0000-3FFF) 16K
	if ( (*d>=0x7600) && (*d<=0x77FF) ) { pLCDC->SetDirtyBuf(*d-0x7600);return(1);}
	if ( (*d>=0x7800) && (*d<=0x7FFF) ) { return(1); }										// RAM area(7800-7BFF)
	if ( (*d>=0x8000) && (*d<=0x9FFF) ) { return(0); }										// RAM area(4000-7FFFF)
	if ( (*d>=0xA000) && (*d<=0xBFFF) ) { return(0); }										// RAM area(4000-7FFFF)
	if ( (*d>=0xC000) && (*d<=0xFFFF) ) { return(0); }										// RAM area(4000-7FFFF)
	if ( (*d>=0x1B000)&&(*d<=0x1B00F) ) { ce150_Access = TRUE;	return(1); }
	if ( (*d>=0x1F000)&&(*d<=0x1F00F) )	{ lh5810_Access = true;
										  if (*d==0x1F006) pLH5810->New_L=true;
										  return(1);}										// I/O area(LH5810)

	if ( (*d>=0x1D000)&&(*d<=0x1D00F) ) { return(1); }
	if ( (*d>=0x18000)&&(*d<=0x1800F) ) { return(1); }

	return(0);
}


bool Cpc15XX::Chk_Adr_R(DWORD *d,DWORD data) 
{ 
	Mem_Mirror(d);
	if (*d == 0x4000) AddLog(LOG_MASTER,tr("read 0x04000"));
	if (*d == 0x4001) AddLog(LOG_MASTER,tr("read 0x04001"));

	if ( (*d>=0x1B000) && (*d<=0x1B00F) ) {	ce150_Access = TRUE;	return(1);	}
	if ( (*d>=0x1F000) && (*d<=0x1F00F) ) {	lh5810_Access = TRUE;	return(1);	}

	if ( (ce150_connected) && (*d>=0xA000) && (*d<=0xBFFF) &&  (((CLH5801 *)pCPU)->lh5801.pv==0) )
	{ 
		*d+=0x1A000;
		return(1); 
	}
	return(1); 
}

 
bool Cpc1500A::Chk_Adr_R(DWORD *d,DWORD data) 
{ 
	Cpc15XX::Chk_Adr_R(d,data);
	return(1); 
}

void Cpc15XX::Set_Port(PORTS Port,BYTE data){};
BYTE Cpc15XX::Get_Port(PORTS Port){return(0);};

#define KS		( pKEYB->Get_KS()) 
#define KEY(c)	( toupper(pKEYB->LastKey) == toupper(c) )

UINT8 Cpc15XX::in(UINT8 address)
{
//return 1^0xff;
//	static int cnt=0;
	UINT8 data=0;
	if (KS&1) {
		if (KEY('2'))			data|=1;
		if (KEY('5'))			data|=2;
		if (KEY('8'))			data|=4;
		if (KEY('H'))			data|=8;
		if (KEY(K_SHT))			data|=0x10;
		if (KEY('Y'))			data|=0x20;
		if (KEY('N'))			data|=0x40;
		if (KEY(K_UA))			data|=0x80;			// UP ARROW
	}
	if (KS&2) {
		if (KEY('.'))			data|=1;
		if (KEY('-'))			data|=2;
		if (KEY(K_OF))			data|=4;			// OFF
		if (KEY('S'))			data|=8;
		if (KEY(K_F1))			data|=0x10;
		if (KEY('W'))			data|=0x20;
		if (KEY('X'))			data|=0x40;
		if (KEY(K_RSV))			data|=0x80;
	}
	if (KS&4) {
		if (KEY('1'))			data|=1;
		if (KEY('4'))			data|=2;
		if (KEY('7'))			data|=4;
		if (KEY('J'))			data|=8;
		if (KEY(K_F5))			data|=0x10;
		if (KEY('U'))			data|=0x20;
		if (KEY('M'))			data|=0x40;
		if (KEY('0'))			data|=0x80;
	}
	if (KS&8) {
		if (KEY(')'))			data|=1;
		if (KEY('L'))			data|=2;
		if (KEY('O'))			data|=4;
		if (KEY('K'))			data|=8;
		if (KEY(K_F6))			data|=0x10;
		if (KEY('I'))			data|=0x20;
		if (KEY('('))			data|=0x40;
		if (KEY(K_RET))			data|=0x80;
	}
	if (KS&0x10) {
		if (KEY('+'))			data|=1;			// +
		if (KEY('*'))			data|=2;			// *
		if (KEY('/'))			data|=4;			// /
		if (KEY('D'))			data|=8;
		if (KEY(K_F2))			data|=0x10;			// Key F2
		if (KEY('E'))			data|=0x20;
		if (KEY('C'))			data|=0x40;
		if (KEY(K_RCL))			data|=0x80;
	}
	if (KS&0x20) {
		if (KEY('='))			data|=1;			// =
		if (KEY(K_LA))			data|=2;			// LEFT ARROW
		if (KEY('P'))			data|=4;
		if (KEY('F'))			data|=8;
		if (KEY(K_F3))			data|=0x10;
		if (KEY('R'))			data|=0x20;
		if (KEY('V'))			data|=0x40;
		if (KEY(' '))			data|=0x80;
	}
	if (KS&0x40) {
		if (KEY(K_RA))			data|=1;			// R ARROW
		if (KEY(K_MOD))			data|=2;			// MODE
		if (KEY(K_CLR))			data|=4;			// CLS
		if (KEY('A'))			data|=8;
		if (KEY(K_DEF))			data|=0x10;
		if (KEY('Q'))			data|=0x20;
		if (KEY('Z'))			data|=0x40;
		if (KEY(K_SML))			data|=0x80;
	}
	if (KS&0x80) {
		if (KEY('3'))			data|=1;
		if (KEY('6'))			data|=2;
		if (KEY('9'))			data|=4;
		if (KEY('G'))			data|=8;
		if (KEY(K_F4))			data|=0x10;			// Key F4
		if (KEY('T'))			data|=0x20;
		if (KEY('B'))			data|=0x40;
		if (KEY(K_DA))			data|=0x80;			// DOWN ARROW
	}

	return data^0xff;
}


void Cpc15XX::Regs_Info(UINT8 Type)
{
	strcat(Regs_String,	"");
	CpcXXXX::Regs_Info(0);
	pLH5810->Regs_Info(0);
	pCPU->Regs_Info(0);
	strcat(Regs_String,	"\r\n");
	strcat(Regs_String,pLH5810->Regs_String);
}


bool Cpc15XX::Set_Connector(void)
{
	pCONNECTOR->Set_pin(1	,0);
	return true;
}
bool Cpc15XX::Get_Connector(void)
{
	ce150_connected = pCONNECTOR->Get_pin(1);	
	return true;
}

bool	CLH5810_PC1500::init(void)
{
	SetRegBit(LH5810_OPB,3,TRUE);	// Export model vs domestic model
	SetRegBit(LH5810_OPB,4,FALSE);	// PB4 to GND

	return(1);
};						//initialize



bool CLH5810_PC1500::step()
{
	////////////////////////////////////////////////////////////////////
	//	INT FROM connector to IRQ
	////////////////////////////////////////////////////////////////////
	IRQ = pPC->pCONNECTOR->Get_pin(30);
		
	////////////////////////////////////////////////////////////////////
	//	Send Data to PD1990AC -- TIMER
	////////////////////////////////////////////////////////////////////

	UINT8 t = lh5810.r_opc;
	pPD1990AC->Set_data(READ_BIT(t,0));		// PC0
	pPD1990AC->Set_stb(READ_BIT(t,1));		// PC1
	pPD1990AC->Set_clk(READ_BIT(t,2));		// PC2
	pPD1990AC->Set_out_enable(READ_BIT(t,3));	// PC3
	pPD1990AC->Set_c0(READ_BIT(t,3));			// PC3
	pPD1990AC->Set_c1(READ_BIT(t,4));			// PC4
	pPD1990AC->Set_c2(READ_BIT(t,5));			// PC5

	pPD1990AC->step();

	// PB5 = TP
	// PB6 = DATA
	SetRegBit(LH5810_OPB,5,pPD1990AC->Get_tp());
	SetRegBit(LH5810_OPB,6,pPD1990AC->Get_data());

	
	////////////////////////////////////////////////////////////////////
	//	ON/Break
	////////////////////////////////////////////////////////////////////
    //SetRegBit(LH5810_OPB,7,pPC->pKEYB->Kon);

    if (pPC->pKEYB->Kon)
    {
        SetRegBit(LH5810_OPB,7,TRUE);
    } else
    {
        SetRegBit(LH5810_OPB,7,FALSE);
    }

	////////////////////////////////////////////////////////////////////
	//	TAPE READER
	////////////////////////////////////////////////////////////////////
	if ( ((Cpc15XX *)pPC)->ce150_connected)
	{
//		int bit=((Cpc15XX *)pPC)->pce152->GetBit();
//		SetRegBit(LH5810_OPB,2,bit);
	}

	////////////////////////////////////////////////////////////////////
	//	TAPE WRITER
	////////////////////////////////////////////////////////////////////
	//	if IF & 0x08  = 0 the CPU store a BYTE to transmit
	//	send the Byte to TAPE
	//	set IF | 0x08 to 1
	if ( !(lh5810.r_if & 0x08))
	{
		Cpc15XX *pPC15XX = (Cpc15XX *) pPC;
			
		AddLog(LOG_TAPE,tr("Write Tape %1").arg(lh5810.r_l,2,16,QChar('0')));
		pPC15XX->pce152->WriteBitToWav(0,&pPC15XX->pce152->info);				// Start bit
		for (int ii=0;ii<8;ii++)						// 8 data bits
		{
			pPC15XX->pce152->WriteBitToWav(((lh5810.r_l >>ii)&0x01),&pPC15XX->pce152->info);
		}
		pPC15XX->pce152->WriteBitToWav(1,&pPC15XX->pce152->info);				// Stop bit
		pPC15XX->pce152->WriteBitToWav(1,&pPC15XX->pce152->info);				// Stop bit

		SetRegBit(LH5810_IF,3,true);
	}


	SetRegBit(LH5810_OPB,3,TRUE);	// Export model vs domestic model
	SetRegBit(LH5810_OPB,4,FALSE);	// PB4 to GND

	  //----------------------//
	 // Standard LH5810 STEP //
	//----------------------//
	CLH5810::step();

	return(1);
}

void Cpc15XX::contextMenuEvent ( QContextMenuEvent * event )
{
	QMenu menu(this);
	
	BuildContextMenu(&menu);
	
	menu.addSeparator();

	    //-----------------------------------------------//
	   // Specific Tape menu for the PC1500				//
	  // I use a hack to manage tape read and write	   //
	 // as i need to improve LH5810 serial emulation  //
	//-----------------------------------------------//
	QMenu * menuTape = menu.addMenu(tr("Tape"));
		menuTape->addAction(tr("Load..."),pce152,SLOT(LoadTape()));
		menuTape->addAction(tr("Play"),pce152,SLOT(Play()));
		menuTape->addAction(tr("Stop"),pce152,SLOT(StopPlay()));
		menuTape->addAction(tr("Record"),pce152,SLOT(RecTape()));
	//--------------------------------------------------
	
	menu.exec(event->globalPos () );
}

