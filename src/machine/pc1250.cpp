  
#include	<stdlib.h>

#include	"common.h"
#include	"pc1250.h"
 
#include	"Inter.h"
#include	"Keyb.h"
#include	"Log.h"
 
Cpc1250::Cpc1250(CPObject *parent)	: CpcXXXX(this)
{								//[constructor]
	setfrequency( (int) 576000/3);
	setcfgfname(QString("pc1250"));

	SessionHeader	= "PC1250PKM";
	SessionHeaderLen= 9;
	Initial_Session_Fname ="pc1250.pkm";

	BackGroundFname	= ":/PC1250/pc1250/pc1250.png";
	LcdFname		= ":/PC1250/pc1250/1250lcd.png";
	SymbFname		= ":/PC1250/pc1250/1250symb.png";

	memsize		= 0x10000;

	SlotList.clear();
	SlotList.append(CSlot(8	, 0x0000 ,	":/PC1250/pc1250/cpu-1250.rom"	, "pc1250/cpu-1250.rom"	, ROM , "CPU ROM"));
	SlotList.append(CSlot(8 , 0x2000 ,	""								, "pc1250/R1-1250.ram"	, RAM , "RAM"));
	SlotList.append(CSlot(16, 0x4000 ,	":/PC1250/pc1250/bas-1250.rom"	, "pc1250/bas-1250.rom"	, ROM , "BASIC ROM"));
	SlotList.append(CSlot(32, 0x8000 ,	""								, "pc1250/R2-1250.ram" 	, RAM , "RAM"));

	KeyMap		= KeyMap1250;
	KeyMapLenght= KeyMap1250Lenght;

	PowerSwitch	= 0;
	Pc_Offset_X = Pc_Offset_Y = 0;

	Pc_DX_mm = 135;
	Pc_DY_mm = 70;
	Pc_DZ_mm = 10;
	
	Pc_DX		= 483;//409;
	Pc_DY		= 252;//213;

	Lcd_X		= (int) (45 * 1.18);
	Lcd_Y		= (int) (45 * 1.18);
	Lcd_DX		= 144 ;
	Lcd_DY		= 8;
	Lcd_ratio_X	= 2 * 1.18;
	Lcd_ratio_Y	= 2 * 1.18;

	Lcd_Symb_X	= (int) (45 * 1.18);
	Lcd_Symb_Y	= (int) (35 * 1.18);
	Lcd_Symb_DX	= 288;
	Lcd_Symb_DY	= 5;
	Lcd_Symb_ratio	= 1.18;

//		DialogExtensionID = IDD_EXT_PROPERTIES_12XX;

	PowerSwitch = 0;
	previous_key= 0;
	
	pLCDC		= new Clcdc_pc1250(this);
	pCPU		= new CSC61860(this);
	pTIMER		= new Ctimer(this);
	pCONNECTOR	= new Cconnector11(this);		publish(pCONNECTOR);
	pKEYB		= new Ckeyb_pc1250(this);

}
	
bool Cpc1250::CompleteDisplay(void)
{
	CpcXXXX::CompleteDisplay();
		
#if 0 
    RECT	CLRRect,RUNRect,PRORect,RSVRect;
    HRESULT	hRet;
	////////////////////////////////////////////////////
	// Deal with the Mode Switch
	////////////////////////////////////////////////////

	SetRect(&CLRRect, Pc_Offset_X+379, Pc_Offset_Y+27, Pc_Offset_X+379 + 14, Pc_Offset_Y+27 + 7);
	SetRect(&RUNRect, Pc_Offset_X+379, Pc_Offset_Y+41, Pc_Offset_X+379 + 14, Pc_Offset_Y+41 + 7);
	SetRect(&PRORect, Pc_Offset_X+379, Pc_Offset_Y+34, Pc_Offset_X+379 + 14, Pc_Offset_Y+34 + 7);
	SetRect(&RSVRect, Pc_Offset_X+379, Pc_Offset_Y+27, Pc_Offset_X+379 + 14, Pc_Offset_Y+27 + 7);

	hRet = pDDSurface->Blt(&RUNRect, g_pDDSOne, &CLRRect, DDBLT_WAIT, NULL);
	hRet = pDDSurface->Blt(&PRORect, g_pDDSOne, &CLRRect, DDBLT_WAIT, NULL);
	hRet = pDDSurface->Blt(&RSVRect, g_pDDSOne, &CLRRect, DDBLT_WAIT, NULL);
	switch (PowerSwitch)
	{
	case PS_RUN :	hRet = pDDSurface->Blt(&RUNRect, g_pDDSOne, &RUNRect, DDBLT_WAIT, NULL);	break;
	case PS_PRO :	hRet = pDDSurface->Blt(&PRORect, g_pDDSOne, &RUNRect, DDBLT_WAIT, NULL);	break;
	case PS_RSV :	hRet = pDDSurface->Blt(&RSVRect, g_pDDSOne, &RUNRect, DDBLT_WAIT, NULL);	break;
	}
#endif
	
	return TRUE;
}

bool Cpc1250::InitDisplay(void)
{

	CpcXXXX::InitDisplay();

	return(1);
}

// PIN_MT_OUT2	1
// PIN_GND		2
// PIN_VGG		3
// PIN_BUSY		4
// PIN_D_OUT	5
// PIN_MT_IN	6
// PIN_MT_OUT1	7
// PIN_D_IN		8
// PIN_ACK		9
// PIN_SEL2		10
// PIN_SEL1		11
//#define GET_PORT_BIT(Port,Bit) ((Get_Port(Port) & (1<<((Bit)-1))) ? 1:0)

void Cpc1250::TurnON(void)
{
	CpcXXXX::TurnON();
}

bool Cpc1250::Set_Connector(void)
{
	pCONNECTOR->Set_pin(PIN_MT_OUT2	,0);
	pCONNECTOR->Set_pin(PIN_VGG		,1);
	pCONNECTOR->Set_pin(PIN_BUSY	,GET_PORT_BIT(PORT_F,1));		// F01
	pCONNECTOR->Set_pin(PIN_D_OUT	,GET_PORT_BIT(PORT_F,2));		// F02
	pCONNECTOR->Set_pin(PIN_MT_OUT1	,pCPU->Get_Xout());
	pCONNECTOR->Set_pin(PIN_SEL2	,GET_PORT_BIT(PORT_B,6));		// B06
	pCONNECTOR->Set_pin(PIN_SEL1	,GET_PORT_BIT(PORT_B,5));		// B05

	return(1);
}

bool Cpc1250::Get_Connector(void)
{
	Set_Port_Bit(PORT_B,8,pCONNECTOR->Get_pin(PIN_D_IN));	// DIN	:	IB7
	Set_Port_Bit(PORT_B,7,pCONNECTOR->Get_pin(PIN_ACK));	// ACK	:	IB8
	pCPU->Set_Xin(pCONNECTOR->Get_pin(PIN_MT_IN));

	return(1);
}


/*****************************************************************************/
/* Check Address ROM or RAM ?												 */
/*  ENTRY :DWORD d=Address													 */
/*  RETURN:bool (1=RAM,0=ROM)												 */
/*****************************************************************************/
// Virtual Fonction

bool Cpc1250::Mem_Mirror(DWORD *d) 
{
	if ( (*d>=0x8000) && (*d<=0x9FFF) )	{ /*AddLog(LOG_RAM,"RAM: Mirror %04X -> %04X",*d,*d+0x2000);*/	*d+=0x2000;return(1);	}
	if ( (*d>=0xD000) && (*d<=0xD7FF) )	{ /*AddLog(LOG_RAM,"RAM: Mirror %04X -> %04X",*d,*d-0x1000);*/	*d-=0x1000;return(1); }

	// VRAM Mirror
	if ( (*d>=0xF900) && (*d<=0xF9FF) )	{ /*AddLog(LOG_RAM,"RAM: Mirror %04X -> %04X",*d,*d-0x100);*/	*d-=0x100;return(1);}
	if ( (*d>=0xFA00) && (*d<=0xFAFF) )	{ /*AddLog(LOG_RAM,"RAM: Mirror %04X -> %04X",*d,*d-0x200);*/	*d-=0x200;return(1);}
	if ( (*d>=0xFB00) && (*d<=0xFBFF) )	{ /*AddLog(LOG_RAM,"RAM: Mirror %04X -> %04X",*d,*d-0x300);*/	*d-=0x300;return(1);}
	if ( (*d>=0xFC00) && (*d<=0xFCFF) )	{ /*AddLog(LOG_RAM,"RAM: Mirror %04X -> %04X",*d,*d-0x400);*/	*d-=0x400;return(1);}
	if ( (*d>=0xFD00) && (*d<=0xFDFF) )	{ /*AddLog(LOG_RAM,"RAM: Mirror %04X -> %04X",*d,*d-0x500);*/	*d-=0x500;return(1);}
	if ( (*d>=0xFE00) && (*d<=0xFEFF) )	{ /*AddLog(LOG_RAM,"RAM: Mirror %04X -> %04X",*d,*d-0x600);*/	*d-=0x600;return(1);}
	if ( (*d>=0xFF00) && (*d<=0xFFFF) )	{ /*AddLog(LOG_RAM,"RAM: Mirror %04X -> %04X",*d,*d-0x700);*/	*d-=0x700;return(1);}
	
	return(1);
}


bool Cpc1250::Chk_Adr(DWORD *d,DWORD data)
{
//	Mem_Mirror(d);

	if ( (*d>=0xC000) && (*d<=0xC7FF) )	return(true);		// RAM area(c000-c800)
	if ( (*d>=0xF800) && (*d<=0xF87B) )	{ pLCDC->SetDirtyBuf(*d-0xF800); return(true);	}
	if ( (*d>=0xF800) && (*d<=0xF8FF) )	return(true);		// VRAM area(f800-f8ff)

	return(false);
}

bool Cpc1250::Chk_Adr_R(DWORD *d,DWORD data)
{
//	Mem_Mirror(d);
	return(true);
}


BYTE	Cpc1250::Get_PortA(void)
{
	int data = 0;

	if (IO_B & 1) {
		if (KEY('-'))			data|=0x01;
		if (KEY(K_CLR))			data|=0x02;
		if (KEY('*'))			data|=0x04;
		if (KEY('/'))			data|=0x08;
		if (KEY(K_DA))			data|=0x10;
		if (KEY('E'))			data|=0x20;
		if (KEY('D'))			data|=0x40;
		if (KEY('C'))			data|=0x80;
	}
	if (IO_B & 2) {
		if (KEY('+'))			data|=0x01;
		if (KEY('9'))			data|=0x02;
		if (KEY('3'))			data|=0x04;
		if (KEY('6'))			data|=0x08;
		if (KEY(K_SHT))			data|=0x10;
		if (KEY('W'))			data|=0x20;
		if (KEY('S'))			data|=0x40;
		if (KEY('X'))			data|=0x80;
	}
	if (IO_B & 4) {
		if (KEY('.'))			data|=0x01;
		if (KEY('8'))			data|=0x02;
		if (KEY('2'))			data|=0x04;
		if (KEY('5'))			data|=0x08;
		if (KEY(K_DEF))			data|=0x10;
		if (KEY('Q'))			data|=0x20;
		if (KEY('A'))			data|=0x40;
		if (KEY('Z'))			data|=0x80;
	}
	if (IO_A & 1) {
		if (KEY('7'))			data|=0x02;
		if (KEY('1'))			data|=0x04;
		if (KEY('4'))			data|=0x08;
		if (KEY(K_UA))			data|=0x10;
		if (KEY('R'))			data|=0x20;
		if (KEY('F'))			data|=0x40;
		if (KEY('V'))			data|=0x80;
	}
	if (IO_A & 2) {
		if (KEY('='))			data|=0x04;
		if (KEY('P'))			data|=0x08;
		if (KEY(K_LA))			data|=0x10;
		if (KEY('T'))			data|=0x20;
		if (KEY('G'))			data|=0x40;
		if (KEY('B'))			data|=0x80;
	}
	if (IO_A & 4) {
		if (KEY('O'))			data|=0x08;
		if (KEY(K_RA))			data|=0x10;
		if (KEY('Y'))			data|=0x20;
		if (KEY('H'))			data|=0x40;
		if (KEY('N'))			data|=0x80;
	}
	if (IO_A & 8) {
		if (KEY('U'))			data|=0x20;
		if (KEY('J'))			data|=0x40;
		if (KEY('M'))			data|=0x80;
	}
	if (IO_A & 0x10) {
		if (KEY('I'))			data|=0x20;
		if (KEY('K'))			data|=0x40;
		if (KEY(' '))			data|=0x80;
	}
	if (IO_A & 0x20) {
		if (KEY('L'))			data|=0x40;
		if (KEY(K_RET))			data|=0x80;
	}
	if (IO_A & 0x40) {
		if (KEY('0'))			data|=0x80;
	}

	return data;
}

BYTE	Cpc1250::Get_PortB(void)
{
	int data=IO_B & 0x0f;
	int retval = 0;

	if (KEY(K_MOD) && ( previous_key != K_MOD) )
	{
		switch (PowerSwitch)
		{
		case PS_RUN : PowerSwitch = PS_PRO; break;
		case PS_PRO : PowerSwitch = PS_RSV; break;
		case PS_RSV : PowerSwitch = PS_OFF; break;
		}
	}
	previous_key = pKEYB->LastKey;

	if (IO_B&8)
	{
		switch (PowerSwitch)
		{
		case PS_PRO : retval|=2; break;
		case PS_RSV : retval|=1; break;
		}
	}

	if ( (IO_B&2) && (PowerSwitch==PS_PRO) ) retval|=8;
	if ( (IO_B&1) && (PowerSwitch==PS_RSV) ) retval|=8;

	int ret = (retval & ~data) | (IO_B & 0xf0);
	
	return (ret);
}

bool Cpc1250::LoadExtra(QFile *file)
{
#if 0
	QDataStream in(file);
	in >> PowerSwitch;
#endif
	return true;
}
bool Cpc1250::SaveExtra(QFile *file)	
{
#if 0
	QDataStream out(file);
	out << PowerSwitch;
#endif
	return true;
}

 





