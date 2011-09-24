/*** PC-E500 Emulator [SC62015.CC] ************************/
/* SC62015 CPU emulation                                  */
/*                       Copyright (C) Matsumo 1998,1999  */
/**********************************************************/
#if 1


#include <stdlib.h>
#include <string.h>
#include "sc62015.h"
#include "pcxxxx.h"
#include "inter.h"
#include "debug.h"
#include "keyb.h"
//#include "rtc.h"
//#include "opt11.h"
//#include "ssfdc.h"
#include "sio.h"
#include "lcdc.h"

//#define	UN_DEFINE	printf("Undefined Code !!(pc=%05X)\n",reg.x.p);
#define		UN_DEFINE	debug.isdebug=1;
#define		OPR1			0		/* first operand by i-mem access */
#define		OPR2			1		/* second operand by i-mem access */
#define		Get_BP			imem[0xec]	/*	i-mem BP */
#define		Get_PX			imem[0xed]	/*	i-mem PX */
#define		Get_PY			imem[0xee]	/*	i-mem PY */
#define		Get_i8d(a)		imem[a&255]			//get i-mem direct
#define		Set_i8d(a,d)	imem[a&255]=d		//set i-mem direct
#define		Reset_Pre()		pre_1=1;pre_2=1	/* reset pre byte mode */
//#define		AddState(n)		timer.state+=(n);
#define		GetBank()		(((imem[IMEM_EOL]>>5)&1)|((imem[IMEM_KOH]>>6)&2))
#define		SLOT1			0
#define		SLOT2			1
#define		SLOT3			2
#define		EMS				3
#define		SLOT3EXT		4

// レジスタ構造体
typedef
union {
	struct a{
		BYTE	a,b,il,ih;
		WORD	pc,ps,xl,xh,yl,yh,ul,uh,sl,sh;
		BYTE	c:1;
		BYTE	z:1;
	}r;
	struct b{
		WORD	ba,i;
		DWORD	p,x,y,u,s;
		BYTE	f;
	}x;
} SCREG;

BYTE	pre_1,pre_2;		/* pre byte mode */
SCREG	reg;				/* register structure */
BYTE	*mem, *imem;		/* memory */

DWORD	BASE_128[]={0x20000, 0x100000, 0x120000, 0x140000};
DWORD	BASE_64[] ={0x10000, 0x160000, 0x170000, 0x180000};
DWORD	SlotAdr[]={0x80000,0x40000,0xc0000,0x10000,0x20000};
WORD	SlotSize[]={256,256,256,192,128};

//Csc sc;

Csc62015::Csc62015(CPObject *parent)	: CCPU(parent)
{									//[constructor]
    halt=0;				//halt?(0:none, 1:halting)
    off=0;				//off?(0:none, 1:off)
    end=0;				//program end?(0:none, 1:end)
    save=0;				//end with memory save?(0:no, 1:yes)
//    SlotName[0]="s1.ram"; SlotName[1]="s2.ram";
//    SlotName[2]="s3.rom"; SlotName[3]="ems.ram";
    e6=0;				//E650 mode?(0:no, 1:yes)
    emsmode=0;			//EMS mode(0:none, 1-5:Delta 1-4 or Super)
    log=0;				//execute log?(0:off, 1:on)
    logsw=0;				//log mode?(0:off, 1:on)
    usestatus=0;
    fp_status=0;
    fn_status="e500e.sta";
}

inline void Csc62015::AddState(BYTE n)
{
    pPC->pTIMER->state+=(n);
/*
    ticks+=(n);
    ticks2+=(n);

#if 1
    div500  = (ticks >= XTICKS);
    div2    = (ticks2>= XTICK2);
#else
    if (ticks >= XTICKS)
    {
        div500 = true;
        ticks =0;
    }
    if (ticks2 >= XTICK2)
    {
        div2 = true;
        ticks2 =0;
    }
#endif

    if (resetFlag) {
        ticksReset+=(n);
        if (ticksReset >= XTICKRESET)
        {
            resetFlag = false;
            ticksReset = 0;
        }
    }

    backgroundTasks();
    */
}

/*****************************************************************************/
/* Convert to internal-RAM address with PRE byte							 */
/*  ENTRY :BYTE d=internal RAM address										 */
/*  RETURN:BYTE (internal RAM real address)									 */
/*****************************************************************************/
static inline BYTE Conv_imemAdr(BYTE d,bool m)
{
/*printf("mode=%d\n",m);*/
	register BYTE	r;
	if(m==0){
		switch(pre_1){					/* first operand */
		case	0:r=d;			   break;	/* (n) */
		case	1:r=d+Get_BP;	   break;	/* (BP+n) */
		case	2:r=d+Get_PX;	   break;	/* (PX+n) */
		case	3:r=Get_BP+Get_PX; break;	/* (BP+PX) */
		}
	}else{
		switch(pre_2){					/* second operand */
		case	0:r=d;			   break;	/* (n) */
		case	1:r=d+Get_BP;	   break;	/* (BP+n) */
		case	2:r=d+Get_PY;	   break;	/* (PY+n) */
		case	3:r=Get_BP+Get_PY; break;	/* (BP+PY) */
		}
	}
	return(r);
}

/*****************************************************************************/
/* Check for I/O access by internal-RAM address								 */
/*  ENTRY :BYTE d=internal RAM address, BYTE len=access bytes				 */
/*  RETURN:none																 */
/*****************************************************************************/
inline void Csc62015::Chk_imemAdr(BYTE d,BYTE len)
{
	register BYTE i;
	for(i=0;i<len;i++){
		switch(d++){
//		case	IMEM_KOL:
//		case	IMEM_KOH:keyb.access=1; break;	// key matrix
//		case	IMEM_RxD:sio.si=1; break;		// sio RxD
//		case	IMEM_TxD:sio.so=1; break;		// sio TxD
//		case	IMEM_SCR:snd.scr=1; break;		// sound
//		case	IMEM_EOL:opt11.eio=IMEM_EOL; break;
//		case	IMEM_EIL:opt11.eio=IMEM_EIL; break;
		}
	}
}




/*****************************************************************************/
/* Get data from imem[]														 */
/*  ENTRY :BYTE a=internal RAM address										 */
/*  RETURN:(BYTE(8),WORD(16),DWORD(20,24) data)								 */
/*****************************************************************************/
BYTE Csc62015::Get_i8(BYTE a,bool m)
{
	register BYTE	adr;
	adr=Conv_imemAdr(a,m);
	Chk_imemAdr(adr,SIZE_8);
	return(imem[adr]);
}
WORD Csc62015::Get_i16(BYTE a,bool m)
{
	register BYTE	adr;
	adr=Conv_imemAdr(a,m);
	Chk_imemAdr(adr,SIZE_16);
	return(imem[adr]+(imem[adr+1]<<8));
}
DWORD Csc62015::Get_i20(BYTE a,bool m)
{
	register BYTE	adr;
	adr=Conv_imemAdr(a,m);
	Chk_imemAdr(adr,SIZE_20);
	return((imem[adr]+(imem[adr+1]<<8)+(imem[adr+2]<<16))&MASK_20);
}
DWORD Csc62015::Get_i24(BYTE a,bool m)
{
	register BYTE	adr;
	adr=Conv_imemAdr(a,m);
	Chk_imemAdr(adr,SIZE_20);
	return((imem[adr]+(imem[adr+1]<<8)+(imem[adr+2]<<16))&MASK_24);
}

/*****************************************************************************/
/* Set data to imem[]														 */
/*  ENTRY :BYTE a=internal RAM address, BYTE(8),WORD(16),DWORD(20,24) d=data */
/*****************************************************************************/
void Csc62015::Set_i8(BYTE a,BYTE d,bool m)
{
	register BYTE	adr;
	adr=Conv_imemAdr(a,m);
	Chk_imemAdr(adr,SIZE_8);
	imem[adr]=d;
}
void Csc62015::Set_i16(BYTE a,WORD d,bool m)
{
	register BYTE	adr;
	adr=Conv_imemAdr(a,m);
	Chk_imemAdr(adr,SIZE_16);
	imem[adr++]=d;
	imem[adr]=(d>>8);
}
void Csc62015::Set_i20(BYTE a,DWORD d,bool m)
{
	register BYTE	adr;
	adr=Conv_imemAdr(a,m);
	Chk_imemAdr(adr,SIZE_20);
	imem[adr++]=d;
	imem[adr++]=(d>>8);
	imem[adr]=(d>>16)&MASK_4;
}
void Csc62015::Set_i24(BYTE a,DWORD d,bool m)
{
	register BYTE	adr;
	adr=Conv_imemAdr(a,m);
	Chk_imemAdr(adr,SIZE_20);
	imem[adr++]=d;
	imem[adr++]=(d>>8);
	imem[adr]=(d>>16);
}

/*****************************************************************************/
/* Get register data														 */
/*  ENTRY :BYTE r=register No.(0-7)											 */
/*  RETURN:(BYTE(8),WORD(16),DWORD(20) data)								 */
/*****************************************************************************/
inline DWORD Csc62015::Get_r(BYTE r)
{
	switch(r&7){
	case	0:return(reg.r.a ); break;
	case	1:return(reg.r.il); break;
	case	2:return(reg.x.ba); break;
	case	3:return(reg.x.i ); break;
	case	4:return(reg.x.x ); break;
	case	5:return(reg.x.y ); break;
	case	6:return(reg.x.u ); break;
	case	7:return(reg.x.s ); break;
	}
}

/*****************************************************************************/
/* Get register data (modified to r2,r3)									 */
/*  ENTRY :BYTE r=register No.(0-7)											 */
/*  RETURN:(WORD(16),DWORD(20) data)										 */
/*****************************************************************************/
inline DWORD Csc62015::Get_r2(BYTE r)
{
	switch(r&7){
	case	0:return(reg.x.ba); break;
	case	1:return(reg.x.i ); break;
	case	2:return(reg.x.ba); break;
	case	3:return(reg.x.i ); break;
	case	4:return(reg.x.x ); break;
	case	5:return(reg.x.y ); break;
	case	6:return(reg.x.u ); break;
	case	7:return(reg.x.s ); break;
	}
}

/*****************************************************************************/
/* Set register data														 */
/*  ENTRY :BYTE r=register No.(0-7), BYTE(8),WORD(16),DWORD(20) d=data		 */
/*****************************************************************************/
inline void Csc62015::Set_r(BYTE r,DWORD d)
{
	switch(r&7){
	case	0:reg.r.a = d		  ; break;
	case	1:reg.x.i =(d&MASK_8) ; break;
	case	2:reg.x.ba= d		  ; break;
	case	3:reg.x.i = d		  ; break;
	case	4:reg.x.x =(d&MASK_20); break;
	case	5:reg.x.y =(d&MASK_20); break;
	case	6:reg.x.u =(d&MASK_20); break;
	case	7:reg.x.s =(d&MASK_20); break;
	}
}

/*****************************************************************************/
/* Set register data (modified to r2,r3)									 */
/*  ENTRY :BYTE r=register No.(0-7), WORD(16),DWORD(20) d=data				 */
/*****************************************************************************/
inline void Csc62015::Set_r2(BYTE r,DWORD d)
{
	switch(r&7){
	case	0:reg.x.ba= d		  ; break;
	case	1:reg.x.i = d		  ; break;
	case	2:reg.x.ba= d		  ; break;
	case	3:reg.x.i = d		  ; break;
	case	4:reg.x.x =(d&MASK_20); break;
	case	5:reg.x.y =(d&MASK_20); break;
	case	6:reg.x.u =(d&MASK_20); break;
	case	7:reg.x.s =(d&MASK_20); break;
	}
}

/*****************************************************************************/
/* Check flags(Zero & Carry)												 */
/*  ENTRY :DWORD d=data, BYTE len=data length(8bit=1,16bit=2,20bit=3)		 */
/*****************************************************************************/
inline void Csc62015::Chk_Flag(DWORD d,BYTE len)
{
	DWORD	mask_z[]={0,MASK_8,MASK_16,MASK_20,MASK_24};
	DWORD	mask_c[]={0,MASK_8+1,MASK_16+1,MASK_20+1,MASK_24+1};
//	if(len>4) return;
	reg.r.z=(d&mask_z[len])==0?1:0;
	reg.r.c=(d&mask_c[len])==0?0:1;
}
inline void Csc62015::Chk_Zero(DWORD d,BYTE len)
{
	DWORD	mask_z[]={0,MASK_8,MASK_16,MASK_20,MASK_24};
//	if(len>4) return;
	reg.r.z=(d&mask_z[len])==0?1:0;
}

/*****************************************************************************/
/* Make effective address from current reg.x.p	for MV r,[r3] or MV [r3],r	 */
/*  RETURN:DWORD address													 */
/*****************************************************************************/
DWORD Csc62015::Get_d(BYTE len)
{
    register BYTE	t;
    register DWORD	a;
    t=pPC->Get_8(reg.x.p);
    reg.r.pc++;
    a=Get_r(t);												/* a=r3 ([r3]) */
    switch(t>>4){
    case   2:Set_r(t,(a+len)); break;							/* [r3++] */
    case   3:a-=len; Set_r(t,a); AddState(1); break;			/* [--r3] */
    case   8:a+=pPC->Get_8(reg.x.p);reg.r.pc++; AddState(2); break;	/* [r3+n] */
    case 0xc:a-=pPC->Get_8(reg.x.p);reg.r.pc++; AddState(2); break;	/* [r3-n] */
    }
    return(a&MASK_20);
}

/*****************************************************************************/
/* Make effective address from current reg.x.p & Get i-mem address			 */
/*	for MV (n),[r3] or MV [r3],(n)											 */
/*  RETURN:DWORD address, BYTE r=i-mem address								 */
/*****************************************************************************/
DWORD Csc62015::Get_d2(BYTE len, BYTE *r)
{
    register BYTE	t;
    register DWORD	a;
    t=pPC->Get_8(reg.x.p);
    reg.r.pc++;
    *r=pPC->Get_8(reg.x.p);								/* r=(i-mem address) */
    reg.r.pc++;
    a=Get_r(t);												/* a=r3 ([r3])*/
    switch(t>>4){
    case   2:Set_r(t,(a+len)); break;							/* [r3++] */
    case   3:a-=len; Set_r(t,a); AddState(1); break;			/* [--r3] */
    case   8:a+=pPC->Get_8(reg.x.p);reg.r.pc++;AddState(2);break;	/* [r3+n] */
    case 0xc:a-=pPC->Get_8(reg.x.p);reg.r.pc++;AddState(2);break;	/* [r3-n] */
    }
    return(a&MASK_20);
}

/*****************************************************************************/
/* Make effective address from current reg.x.p	for MV r,[(n)] or MV [(n)],r */
/*  RETURN:DWORD address													 */
/*****************************************************************************/
DWORD Csc62015::Get_i(void)
{
	register BYTE	t;
	register DWORD	a;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    a=Get_i20(pPC->Get_8(reg.x.p),OPR1);								/* a=(n) */
	reg.r.pc++;
	switch(t>>4){
    case   8:a+=pPC->Get_8(reg.x.p);reg.r.pc++;AddState(2);break;	/* [(m)+n] */
    case 0xc:a-=pPC->Get_8(reg.x.p);reg.r.pc++;AddState(2);break;	/* [(m)-n] */
	}
	return(a&MASK_20);
}

/*****************************************************************************/
/* Make effective address from current reg.x.p								 */
/*	for MV (m),[(n)] or MV [(m)],(n)										 */
/*  ENTRY :BYTE r=mode(0:MV (m),[(n)] , 1:MV [(m)],(n) )					 */
/*  RETURN:DWORD address, BYTE r=i-mem address								 */
/*****************************************************************************/
DWORD Csc62015::Get_i2(BYTE *r)
{
	register BYTE	t,u,v,w;
	register DWORD	a;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    u=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    v=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	if(*r){w=u;u=v;v=w;}		/* swap u,v */
	a=Get_i20(v,1-*r);								/* a=(n) */
	*r=u;											/* *r=(i-mem address) */
	switch(t>>4){
    case   8:a+=pPC->Get_8(reg.x.p);reg.r.pc++;AddState(2);break;	/* [(m)+n] */
    case 0xc:a-=pPC->Get_8(reg.x.p);reg.r.pc++;AddState(2);break;	/* [(m)-n] */
	}
	return(a&MASK_20);
}

/*****************************************************************************/
/* Convert from BCD to HEX													 */
/*  ENTRY :BYTE d=BCD data(00h-99h)											 */
/*  RETURN:BYTE HEX data(00h-63h)											 */
/*****************************************************************************/
inline BYTE Csc62015::bcd2hex(BYTE d)
{
	return((d>>4)*10+(d&15));
}

/*****************************************************************************/
/* Convert from HEX to BCD													 */
/*  ENTRY :BYTE d=HEX data(00h-ffh)											 */
/*  RETURN:BYTE BCD data(00h-255h)											 */
/*****************************************************************************/
inline WORD Csc62015::hex2bcd(BYTE d)
{
	register BYTE	a,b,c;
	a=d/100;
	b=d-(a*100);
	c=b/10;
	return((a<<8)+(c<<4)+b-(c*10));
}

/*---------------------------------------------------------------------------*/
/* NOP */
void Csc62015::Op_00(void)
{
	AddState(1);
}
/* RETI */
void Csc62015::Op_01(void)
{
    imem[IMEM_IMR]=pPC->Get_8(reg.x.s);
	reg.x.s++;
    reg.x.f=pPC->Get_8(reg.x.s);
	reg.x.s++;
    reg.x.p=pPC->Get_20(reg.x.s);
	reg.x.s+=SIZE_20;
	AddState(7);
}
/* JP mn */
void Csc62015::Op_02(void)
{
    reg.r.pc=pPC->Get_16(reg.x.p);
	AddState(4);
}
/* JPF lmn */
void Csc62015::Op_03(void)
{
    reg.x.p=pPC->Get_20(reg.x.p);
	AddState(5);
}
/* CALL mn */
void Csc62015::Op_04(void)
{
	register WORD	t;
    t=pPC->Get_16(reg.x.p);
	reg.r.pc+=SIZE_16;
	reg.x.s-=SIZE_16;
    pPC->Set_16(reg.x.s, reg.r.pc);
	reg.r.pc=t;
	AddState(6);
}
/* CALLF lmn */
void Csc62015::Op_05(void)
{
	register DWORD	t;
    t=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
	reg.x.s-=SIZE_20;
    pPC->Set_20(reg.x.s, reg.x.p);
	reg.x.p=t;
	AddState(8);
}
/* RET */
void Csc62015::Op_06(void)
{
    reg.r.pc=pPC->Get_16(reg.x.s);
	reg.x.s+=SIZE_16;
	AddState(4);
}
/* RETF */
void Csc62015::Op_07(void)
{
    reg.x.p=pPC->Get_20(reg.x.s);
	reg.x.s+=SIZE_20;
	AddState(5);
}
/* MV A,n */
void Csc62015::Op_08(void)
{
    reg.r.a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	AddState(2);
}
/* MV IL,n */
void Csc62015::Op_09(void)
{
    reg.r.il=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	reg.r.ih=0;
	AddState(3);
}
/* MV BA,mn */
void Csc62015::Op_0a(void)
{
    reg.x.ba=pPC->Get_16(reg.x.p);
	reg.r.pc+=SIZE_16;
	AddState(3);
}
/* MV I,mn */
void Csc62015::Op_0b(void)
{
    reg.x.i=pPC->Get_16(reg.x.p);
	reg.r.pc+=SIZE_16;
	AddState(3);
}
/* MV X,lmn */
void Csc62015::Op_0c(void)
{
    reg.x.x=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
	AddState(4);
}
/* MV Y,lmn */
void Csc62015::Op_0d(void)
{
    reg.x.y=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
	AddState(4);
}
/* MV U,lmn */
void Csc62015::Op_0e(void)
{
    reg.x.u=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
	AddState(4);
}
/* MV S,lmn */
void Csc62015::Op_0f(void)
{
    reg.x.s=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
	AddState(4);
}
/* JP (n) */
void Csc62015::Op_10(void)
{
    reg.x.p=Get_i20(pPC->Get_8(reg.x.p),OPR1);
	AddState(6);
}
/* JP r3 */
void Csc62015::Op_11(void)
{
    reg.x.p=Get_r(pPC->Get_8(reg.x.p));
	AddState(4);
}
/* JR +n */
void Csc62015::Op_12(void)
{
    reg.r.pc+=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	AddState(3);
}
/* JR -n */
void Csc62015::Op_13(void)
{
    reg.r.pc-=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	AddState(3);
}
/* JPZ mn */
void Csc62015::Op_14(void)
{
	register WORD	t;
    t=pPC->Get_16(reg.x.p);
	reg.r.pc+=SIZE_16;
	if(reg.r.z){reg.r.pc=t; AddState(1);}
	AddState(3);
}
/* JPNZ mn */
void Csc62015::Op_15(void)
{
	register WORD	t;
    t=pPC->Get_16(reg.x.p);
	reg.r.pc+=SIZE_16;
	if(!reg.r.z){ reg.r.pc=t; AddState(1);}
	AddState(3);
}
/* JPC mn */
void Csc62015::Op_16(void)
{
	register WORD	t;
    t=pPC->Get_16(reg.x.p);
	reg.r.pc+=SIZE_16;
	if(reg.r.c){ reg.r.pc=t; AddState(1);}
	AddState(3);
}
/* JPNC mn */
void Csc62015::Op_17(void)
{
	register WORD	t;
    t=pPC->Get_16(reg.x.p);
	reg.r.pc+=SIZE_16;
	if(!reg.r.c){ reg.r.pc=t; AddState(1);}
	AddState(3);
}
/* JRZ +n */
void Csc62015::Op_18(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	if(reg.r.z){ reg.r.pc+=t; AddState(1);}
	AddState(2);
}
/* JRZ -n */
void Csc62015::Op_19(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	if(reg.r.z){ reg.r.pc-=t; AddState(1);}
	AddState(2);
}
/* JRNZ +n */
void Csc62015::Op_1a(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	if(!reg.r.z){ reg.r.pc+=t; AddState(1);}
	AddState(2);
}
/* JRNZ -n */
void Csc62015::Op_1b(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	if(!reg.r.z){ reg.r.pc-=t; AddState(1);}
	AddState(2);
}
/* JRC +n */
void Csc62015::Op_1c(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	if(reg.r.c){ reg.r.pc+=t; AddState(1);}
	AddState(2);
}
/* JRC -n */
void Csc62015::Op_1d(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	if(reg.r.c){ reg.r.pc-=t; AddState(1);}
	AddState(2);
}
/* JRNC +n */
void Csc62015::Op_1e(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	if(!reg.r.c){ reg.r.pc+=t; AddState(1);}
	AddState(2);
}
/* JRNC -n */
void Csc62015::Op_1f(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	if(!reg.r.c){ reg.r.pc-=t; AddState(1);}
	AddState(2);
}
/* Undefined Code */
void Csc62015::Op_20(void)
{
//UN_DEFINE
	AddState(1);
}
/* PRE 21h */
void Csc62015::Op_21(void)
{
	pre_1=1;
	pre_2=3;
	AddState(1);
	Step_sc62015_();
}
/* PRE 22h */
void Csc62015::Op_22(void)
{
	pre_1=1;
	pre_2=0;
	AddState(1);
	Step_sc62015_();
}
/* PRE 23h */
void Csc62015::Op_23(void)
{
	pre_1=1;
	pre_2=2;
	AddState(1);
	Step_sc62015_();
}
/* PRE 24h */
void Csc62015::Op_24(void)
{
	pre_1=3;
	pre_2=1;
	AddState(1);
	Step_sc62015_();
}
/* PRE 25h */
void Csc62015::Op_25(void)
{
	pre_1=3;
	pre_2=3;
	AddState(1);
	Step_sc62015_();
}
/* PRE 26h */
void Csc62015::Op_26(void)
{
	pre_1=3;
	pre_2=0;
	AddState(1);
	Step_sc62015_();
}
/* PRE 27h */
void Csc62015::Op_27(void)
{
	pre_1=3;
	pre_2=2;
	AddState(1);
	Step_sc62015_();
}
/* PUSHU A */
void Csc62015::Op_28(void)
{
	reg.x.u--;
    pPC->Set_8(reg.x.u,reg.r.a);
	AddState(3);
}
/* PUSH IL */
void Csc62015::Op_29(void)
{
	reg.x.u--;
    pPC->Set_8(reg.x.u,reg.r.il);
	AddState(3);
}
/* PUSHU BA */
void Csc62015::Op_2a(void)
{
	reg.x.u-=SIZE_16;
    pPC->Set_16(reg.x.u,reg.x.ba);
	AddState(4);
}
/* PUSHU I */
void Csc62015::Op_2b(void)
{
	reg.x.u-=SIZE_16;
    pPC->Set_16(reg.x.u,reg.x.i);
	AddState(4);
}
/* PUSHU X */
void Csc62015::Op_2c(void)
{
	reg.x.u-=SIZE_20;
    pPC->Set_20(reg.x.u,reg.x.x);
	AddState(5);
}
/* PUSHU Y */
void Csc62015::Op_2d(void)
{
	reg.x.u-=SIZE_20;
    pPC->Set_20(reg.x.u,reg.x.y);
	AddState(5);
}
/* PUSHU F */
void Csc62015::Op_2e(void)
{
	reg.x.u--;
    pPC->Set_8(reg.x.u,reg.x.f);
	AddState(3);
}
/* PUSHU IMR */
void Csc62015::Op_2f(void)
{
	reg.x.u--;
    pPC->Set_8(reg.x.u,imem[IMEM_IMR]);
	imem[IMEM_IMR]&=0x7f;			/* disable interrupt */
	AddState(3);
}
/* PRE 30h */
void Csc62015::Op_30(void)
{
	pre_1=0;
	pre_2=1;
	AddState(1);
	Step_sc62015_();
}
/* PRE 31h */
void Csc62015::Op_31(void)
{
	pre_1=0;
	pre_2=3;
	AddState(1);
	Step_sc62015_();
}
/* PRE 32h */
void Csc62015::Op_32(void)
{
	pre_1=0;
	pre_2=0;
	AddState(1);
	Step_sc62015_();
}
/* PRE 33h */
void Csc62015::Op_33(void)
{
	pre_1=0;
	pre_2=2;
	AddState(1);
	Step_sc62015_();
}
/* PRE 34h */
void Csc62015::Op_34(void)
{
	pre_1=2;
	pre_2=1;
	AddState(1);
	Step_sc62015_();
}
/* PRE 35h */
void Csc62015::Op_35(void)
{
	pre_1=2;
	pre_2=3;
	AddState(1);
	Step_sc62015_();
}
/* PRE 36h */
void Csc62015::Op_36(void)
{
	pre_1=2;
	pre_2=0;
	AddState(1);
	Step_sc62015_();
}
/* PRE 37h */
void Csc62015::Op_37(void)
{
	pre_1=2;
	pre_2=2;
	AddState(1);
	Step_sc62015_();
}
/* POPU A */
void Csc62015::Op_38(void)
{
    reg.r.a=pPC->Get_8(reg.x.u);
	reg.x.u++;
	AddState(2);
}
/* POPU IL */
void Csc62015::Op_39(void)
{
    reg.r.il=pPC->Get_8(reg.x.u);
	reg.x.u++;
	reg.r.ih=0;
	AddState(3);
}
/* POPU BA */
void Csc62015::Op_3a(void)
{
    reg.x.ba=pPC->Get_16(reg.x.u);
	reg.x.u+=SIZE_16;
	AddState(3);
}
/* POPU I */
void Csc62015::Op_3b(void)
{
    reg.x.i=pPC->Get_16(reg.x.u);
	reg.x.u+=SIZE_16;
	AddState(3);
}
/* POPU X */
void Csc62015::Op_3c(void)
{
    reg.x.x=pPC->Get_20(reg.x.u);
	reg.x.u+=SIZE_20;
	AddState(4);
}
/* POPU Y */
void Csc62015::Op_3d(void)
{
    reg.x.y=pPC->Get_20(reg.x.u);
	reg.x.u+=SIZE_20;
	AddState(4);
}
/* POPU F */
void Csc62015::Op_3e(void)
{
    reg.x.f=pPC->Get_8(reg.x.u);
	reg.x.u++;
	AddState(2);
}
/* POPU IMR */
void Csc62015::Op_3f(void)
{
    imem[IMEM_IMR]=pPC->Get_8(reg.x.u);
	reg.x.u++;
	AddState(2);
}
/* ADD A,n */
void Csc62015::Op_40(void)
{
	register DWORD	t;
    t=reg.r.a+pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	reg.r.a=t;
	AddState(3);
}
/* ADD (m),n */
void Csc62015::Op_41(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)+pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* ADD A,(n) */
void Csc62015::Op_42(void)
{
	register DWORD	t;
    t=reg.r.a+Get_i8(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	reg.r.a=t;
	AddState(4);
}
/* ADD (n),A */
void Csc62015::Op_43(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	t=reg.r.a+Get_i8(a,OPR1);
	Chk_Flag(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* ADD r2,r2' */
void Csc62015::Op_44(void)
{
	register BYTE	t;
	register DWORD	d;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	d=(Get_r(t>>4)&MASK_16)+(Get_r(t)&MASK_16);
	Chk_Flag(d,SIZE_16);
	Set_r(t>>4,d);
	AddState(5);
}
/* ADD r3,r' */
void Csc62015::Op_45(void)
{
	register BYTE	t;
	register DWORD	d;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	d=Get_r(t>>4)+Get_r(t);
	Chk_Flag(d,SIZE_20);
	Set_r(t>>4,d);
	AddState(7);
}
/* ADD r1,r1' */
void Csc62015::Op_46(void)
{
	register BYTE	t;
	register DWORD	d;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	d=(Get_r(t>>4)&MASK_8)+(Get_r(t)&MASK_8);
	Chk_Flag(d,SIZE_8);
	Set_r(t>>4,d);
	AddState(3);
}
/* PMDF (m),n */
void Csc62015::Op_47(void)
{
	register BYTE	a;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    Set_i8(a,Get_i8(a,OPR1)+pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	AddState(4);
}
/* SUB A,n */
void Csc62015::Op_48(void)
{
	register DWORD	t;
    t=reg.r.a-pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	reg.r.a=t;
	AddState(3);
}
/* SUB (m),n */
void Csc62015::Op_49(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)-pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* SUB A,(n) */
void Csc62015::Op_4a(void)
{
	register DWORD	t;
    t=reg.r.a-Get_i8(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	reg.r.a=t;
	AddState(4);
}
/* SUB (n),A */
void Csc62015::Op_4b(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	t=Get_i8(a,OPR1)-reg.r.a;
	Chk_Flag(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* SUB r2,r2' */
void Csc62015::Op_4c(void)
{
	register BYTE	t;
	register DWORD	d;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	d=(Get_r(t>>4)&MASK_16)-(Get_r(t)&MASK_16);
	Chk_Flag(d,SIZE_16);
	Set_r(t>>4,d);
	AddState(5);
}
/* SUB r3,r' */
void Csc62015::Op_4d(void)
{
	register BYTE	t;
	register DWORD	d;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	d=Get_r(t>>4)-Get_r(t);
	Chk_Flag(d,SIZE_20);
	Set_r(t>>4,d);
	AddState(7);
}
/* SUB r1,r1' */
void Csc62015::Op_4e(void)
{
	register BYTE	t;
	register DWORD	d;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	d=(Get_r(t>>4)&MASK_8)-(Get_r(t)&MASK_8);
	Chk_Flag(d,SIZE_8);
	Set_r(t>>4,d);
	AddState(3);
}
/* PUSHS F */
void Csc62015::Op_4f(void)
{
	reg.x.s--;
    pPC->Set_8(reg.x.s,reg.x.f);
	AddState(3);
}
/* ADC A,n */
void Csc62015::Op_50(void)
{
	register DWORD	t;
    t=reg.r.a+pPC->Get_8(reg.x.p)+reg.r.c;
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	reg.r.a=t;
	AddState(3);
}
/* ADC (m),n */
void Csc62015::Op_51(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)+pPC->Get_8(reg.x.p)+reg.r.c;
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* ADC A,(n) */
void Csc62015::Op_52(void)
{
	register DWORD	t;
    t=reg.r.a+Get_i8(pPC->Get_8(reg.x.p),OPR1)+reg.r.c;
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	reg.r.a=t;
	AddState(4);
}
/* ADC (n),A */
void Csc62015::Op_53(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	t=Get_i8(a,OPR1)+reg.r.a+reg.r.c;
	Chk_Flag(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* ADCL (m),(n) */
void Csc62015::Op_54(void)
{
	register BYTE	m,n;
	register DWORD	t,b;
	AddState(5+2*reg.x.i);
    m=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    n=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	t=Get_i8d(m)+Get_i8d(n++);
	Chk_Flag(t,SIZE_8);
	Set_i8d(m++,t);
	b=t;
	while(--reg.x.i!=0){
		t=Get_i8d(m)+Get_i8d(n++)+reg.r.c;
		Chk_Flag(t,SIZE_8);
		Set_i8d(m++,t);
		b|=t;
	}
	Chk_Zero(b,SIZE_8);
}
/* ADCL (n),A */
void Csc62015::Op_55(void)
{
	register BYTE	a;
	register DWORD	t,b;
	AddState(4+reg.x.i);
    a=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	t=Get_i8d(a)+reg.r.a;
	Chk_Flag(t,SIZE_8);
	Set_i8d(a++,t);
	b=t;
	while(--reg.x.i!=0){
		t=Get_i8d(a)+reg.r.c;
		Chk_Flag(t,SIZE_8);
		Set_i8d(a++,t);
		b|=t;
	}
	Chk_Zero(b,SIZE_8);
}
/* MVL (m),[r3+-n] */
void Csc62015::Op_56(void)
{
	register BYTE	d;
	register DWORD	s;
	AddState(5+2*reg.x.i);
	s=Get_d2(SIZE_8,&d);
	d=Conv_imemAdr(d,OPR1);
    while(--reg.x.i!=0) Set_i8d(d++,pPC->Get_8(s++));
    Set_i8d(d,pPC->Get_8(s));
}
/* PMDF (n),A */
void Csc62015::Op_57(void)
{
	register BYTE	a;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Set_i8(a,reg.r.a+Get_i8(a,OPR1),OPR1);
	AddState(4);
}
/* SBC A,n */
void Csc62015::Op_58(void)
{
	register DWORD	t;
    t=reg.r.a-pPC->Get_8(reg.x.p)-reg.r.c;
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	reg.r.a=t;
	AddState(3);
}
/* SBC (m),n */
void Csc62015::Op_59(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)-pPC->Get_8(reg.x.p)-reg.r.c;
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* SBC A,(n) */
void Csc62015::Op_5a(void)
{
	register DWORD	t;
    t=reg.r.a-Get_i8(pPC->Get_8(reg.x.p),OPR1)-reg.r.c;
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	reg.r.a=t;
	AddState(4);
}
/* SBC (n),A */
void Csc62015::Op_5b(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	t=Get_i8(a,OPR1)-reg.r.a-reg.r.c;
	Chk_Flag(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* SBCL (m),(n) */
void Csc62015::Op_5c(void)
{
	register BYTE	m,n;
	register DWORD	t,b;
	AddState(5+2*reg.x.i);
    m=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    n=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	t=Get_i8d(m)-Get_i8d(n++);
	Chk_Flag(t,SIZE_8);
	Set_i8d(m++,t);
	b=t;
	while(--reg.x.i!=0){
		t=Get_i8d(m)-Get_i8d(n++)-reg.r.c;
		Chk_Flag(t,SIZE_8);
		Set_i8d(m++,t);
		b|=t;
	}
	Chk_Zero(b,SIZE_8);
}
/* SBCL (n),A */
void Csc62015::Op_5d(void)
{
	register BYTE	a;
	register DWORD	t,b;
	AddState(4+reg.x.i);
    a=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	t=Get_i8d(a)-reg.r.a;
	Chk_Flag(t,SIZE_8);
	Set_i8d(a++,t);
	b=t;
	while(--reg.x.i!=0){
		t=Get_i8d(a)-reg.r.c;
		Chk_Flag(t,SIZE_8);
		Set_i8d(a++,t);
		b|=t;
	}
	Chk_Zero(b,SIZE_8);
}
/* MVL [r3+-n],(m) */
void Csc62015::Op_5e(void)
{
	register BYTE	s;
	register DWORD	d;
	AddState(5+2*reg.x.i);
	d=Get_d2(SIZE_8,&s);
	s=Conv_imemAdr(s,OPR1);
    while(--reg.x.i!=0) pPC->Set_8(d++,Get_i8d(s++));
    pPC->Set_8(d,Get_i8d(s));
}
/* POPS F */
void Csc62015::Op_5f(void)
{
    reg.x.f=pPC->Get_8(reg.x.s);
	reg.x.s++;
	AddState(2);
}
/* CMP A,n */
void Csc62015::Op_60(void)
{
	register DWORD	t;
    t=reg.r.a-pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	AddState(3);
}
/* CMP (m),n */
void Csc62015::Op_61(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)-pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	AddState(4);
}
/* CMP [klm],n */
void Csc62015::Op_62(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(pPC->Get_20(reg.x.p));
	reg.r.pc+=SIZE_20;
    t=a-pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	AddState(6);
}
/* CMP (n),A */
void Csc62015::Op_63(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	t=Get_i8(a,OPR1)-reg.r.a;
	Chk_Flag(t,SIZE_8);
	AddState(4);
}
/* TEST A,n */
void Csc62015::Op_64(void)
{
	register DWORD	t;
    t=reg.r.a&pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	AddState(3);
}
/* TEST (m),n */
void Csc62015::Op_65(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)&pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	AddState(4);
}
/* TEST [klm],n */
void Csc62015::Op_66(void)
{
	register DWORD	t,a;
    a=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
    t=pPC->Get_8(a)&pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	AddState(6);
}
/* TEST (n),A */
void Csc62015::Op_67(void)
{
	register DWORD	t;
    t=reg.r.a&Get_i8(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	AddState(4);
}
/* XOR A,n */
void Csc62015::Op_68(void)
{
	register DWORD	t;
    t=reg.r.a^pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	reg.r.a=t;
	AddState(3);
}
/* XOR (m),n */
void Csc62015::Op_69(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)^pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* XOR [klm],n */
void Csc62015::Op_6a(void)
{
	register DWORD	t,a;
    a=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
    t=pPC->Get_8(a)^pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
    pPC->Set_8(a,t);
	AddState(7);
}
/* XOR (n),A */
void Csc62015::Op_6b(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	t=reg.r.a^Get_i8(a,OPR1);
	Chk_Zero(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* INC r */
void Csc62015::Op_6c(void)
{
	BYTE	reg_size[]={SIZE_8 ,SIZE_8 ,SIZE_16,SIZE_16,
						SIZE_20,SIZE_20,SIZE_20,SIZE_20};
	register BYTE	t;
	register DWORD	d;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	d=Get_r(t);
	Chk_Zero(++d,reg_size[t&7]);
	Set_r(t,d);
	AddState(3);
}
/* INC (n) */
void Csc62015::Op_6d(void)
{
	register BYTE	t;
	register DWORD	d;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	d=Get_i8(t,OPR1);
	Chk_Zero(++d,SIZE_8);
	Set_i8(t,d,OPR1);
	AddState(3);
}
/* XOR (m),(n) */
void Csc62015::Op_6e(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)^Get_i8(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(6);
}
/* XOR A,(n) */
void Csc62015::Op_6f(void)
{
	register BYTE	a;
	register DWORD	t;
    t=reg.r.a^Get_i8(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	reg.r.a=t;
	AddState(4);
}
/* AND A,n */
void Csc62015::Op_70(void)
{
	register DWORD	t;
    t=reg.r.a&pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	reg.r.a=t;
	AddState(3);
}
/* AND (m),n */
void Csc62015::Op_71(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)&pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* AND [klm],n */
void Csc62015::Op_72(void)
{
	register DWORD	t,a;
    a=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
    t=pPC->Get_8(a)&pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
    pPC->Set_8(a,t);
	AddState(7);
}
/* AND (n),A */
void Csc62015::Op_73(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	t=reg.r.a&Get_i8(a,OPR1);
	Chk_Zero(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* MV A,B */
void Csc62015::Op_74(void)
{
	reg.r.a=reg.r.b;
	AddState(1);
}
/* MV B,A */
void Csc62015::Op_75(void)
{
	reg.r.b=reg.r.a;
	AddState(1);
}
/* AND (m),(n) */
void Csc62015::Op_76(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)&Get_i8(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(6);
}
/* AND A,(n) */
void Csc62015::Op_77(void)
{
	register BYTE	a;
	register DWORD	t;
    t=reg.r.a&Get_i8(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	reg.r.a=t;
	AddState(4);
}
/* OR A,n */
void Csc62015::Op_78(void)
{
	register DWORD	t;
    t=reg.r.a|pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	reg.r.a=t;
	AddState(3);
}
/* OR (m),n */
void Csc62015::Op_79(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)|pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* OR [klm],n */
void Csc62015::Op_7a(void)
{
	register DWORD	t,a;
    a=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
    t=pPC->Get_8(a)|pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
    pPC->Set_8(a,t);
	AddState(7);
}
/* OR (n),A */
void Csc62015::Op_7b(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	t=reg.r.a|Get_i8(a,OPR1);
	Chk_Zero(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(4);
}
/* DEC r */
void Csc62015::Op_7c(void)
{
	BYTE	reg_size[]={SIZE_8 ,SIZE_8 ,SIZE_16,SIZE_16,
						SIZE_20,SIZE_20,SIZE_20,SIZE_20};
	register BYTE	t;
	register DWORD	d;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	d=Get_r(t);
	Chk_Zero(--d,reg_size[t&7]);
	Set_r(t,d);
	AddState(3);
}
/* DEC (n) */
void Csc62015::Op_7d(void)
{
	register BYTE	t;
	register DWORD	d;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	d=Get_i8(t,OPR1);
	Chk_Zero(--d,SIZE_8);
	Set_i8(t,d,OPR1);
	AddState(3);
}
/* OR (m),(n) */
void Csc62015::Op_7e(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i8(a,OPR1)|Get_i8(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	Set_i8(a,t,OPR1);
	AddState(6);
}
/* OR A,(n) */
void Csc62015::Op_7f(void)
{
	register BYTE	a;
	register DWORD	t;
    t=reg.r.a|Get_i8(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	Chk_Zero(t,SIZE_8);
	reg.r.a=t;
	AddState(4);
}
/* MV A,(n) */
void Csc62015::Op_80(void)
{
    reg.r.a=Get_i8(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	AddState(3);
}
/* MV IL,(n) */
void Csc62015::Op_81(void)
{
    reg.r.il=Get_i8(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	reg.r.ih=0;
	AddState(4);
}
/* MV BA,(n) */
void Csc62015::Op_82(void)
{
    reg.x.ba=Get_i16(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	AddState(4);
}
/* MV I,(n) */
void Csc62015::Op_83(void)
{
    reg.x.i=Get_i16(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	AddState(4);
}
/* MV X,(n) */
void Csc62015::Op_84(void)
{
    reg.x.x=Get_i20(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	AddState(5);
}
/* MV Y,(n) */
void Csc62015::Op_85(void)
{
    reg.x.y=Get_i20(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	AddState(5);
}
/* MV U,(n) */
void Csc62015::Op_86(void)
{
    reg.x.u=Get_i20(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	AddState(5);
}
/* MV S,(n) */
void Csc62015::Op_87(void)
{
    reg.x.s=Get_i20(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	AddState(5);
}
/* MV A,[lmn] */
void Csc62015::Op_88(void)
{
    reg.r.a=pPC->Get_8(pPC->Get_20(reg.x.p));
	reg.r.pc+=SIZE_20;
	AddState(6);
}
/* MV IL,[lmn] */
void Csc62015::Op_89(void)
{
    reg.r.il=pPC->Get_8(pPC->Get_20(reg.x.p));
	reg.r.pc+=SIZE_20;
	reg.r.ih=0;
	AddState(6);
}
/* MV BA,[lmn] */
void Csc62015::Op_8a(void)
{
    reg.x.ba=pPC->Get_16(pPC->Get_20(reg.x.p));
	reg.r.pc+=SIZE_20;
	AddState(7);
}
/* MV I,[lmn] */
void Csc62015::Op_8b(void)
{
    reg.x.i=pPC->Get_16(pPC->Get_20(reg.x.p));
	reg.r.pc+=SIZE_20;
	AddState(7);
}
/* MV X,[lmn] */
void Csc62015::Op_8c(void)
{
    reg.x.x=pPC->Get_20(pPC->Get_20(reg.x.p));
	reg.r.pc+=SIZE_20;
	AddState(8);
}
/* MV Y,[lmn] */
void Csc62015::Op_8d(void)
{
    reg.x.y=pPC->Get_20(pPC->Get_20(reg.x.p));
	reg.r.pc+=SIZE_20;
	AddState(8);
}
/* MV U,[lmn] */
void Csc62015::Op_8e(void)
{
    reg.x.u=pPC->Get_20(pPC->Get_20(reg.x.p));
	reg.r.pc+=SIZE_20;
	AddState(8);
}
/* MV S,[lmn] */
void Csc62015::Op_8f(void)
{
    reg.x.s=pPC->Get_20(pPC->Get_20(reg.x.p));
	reg.r.pc+=SIZE_20;
	AddState(8);
}
/* MV A,[r3] */
void Csc62015::Op_90(void)
{
    reg.r.a=pPC->Get_8(Get_d(SIZE_8));
	AddState(4);
}
/* MV IL,[r3] */
void Csc62015::Op_91(void)
{
    reg.x.i=pPC->Get_8(Get_d(SIZE_8));
	AddState(5);
}
/* MV BA,[r3] */
void Csc62015::Op_92(void)
{
    reg.x.ba=pPC->Get_16(Get_d(SIZE_16));
	AddState(5);
}
/* MV I,[r3] */
void Csc62015::Op_93(void)
{
    reg.x.i=pPC->Get_16(Get_d(SIZE_16));
	AddState(5);
}
/* MV X,[r3] */
void Csc62015::Op_94(void)
{
    reg.x.x=pPC->Get_20(Get_d(SIZE_20));
	AddState(6);
}
/* MV Y,[r3] */
void Csc62015::Op_95(void)
{
    reg.x.y=pPC->Get_20(Get_d(SIZE_20));
	AddState(6);
}
/* MV U,[r3] */
void Csc62015::Op_96(void)
{
    reg.x.u=pPC->Get_20(Get_d(SIZE_20));
	AddState(6);
}
/* SC */
void Csc62015::Op_97(void)
{
	reg.r.c=1;
	AddState(1);
}
/* MV A,[(n)] */
void Csc62015::Op_98(void)
{
    reg.r.a=pPC->Get_8(Get_i());
	AddState(9);
}
/* MV IL,[(n)] */
void Csc62015::Op_99(void)
{
    reg.r.il=pPC->Get_8(Get_i());
	reg.r.ih=0;
	AddState(10);
}
/* MV BA,[(n)] */
void Csc62015::Op_9a(void)
{
    reg.x.ba=pPC->Get_16(Get_i());
	AddState(10);
}
/* MV I,[(n)] */
void Csc62015::Op_9b(void)
{
    reg.x.i=pPC->Get_16(Get_i());
	AddState(10);
}
/* MV X,[(n)] */
void Csc62015::Op_9c(void)
{
    reg.x.x=pPC->Get_20(Get_i());
	AddState(11);
}
/* MV Y,[(n)] */
void Csc62015::Op_9d(void)
{
    reg.x.y=pPC->Get_20(Get_i());
	AddState(11);
}
/* MV U,[(n)] */
void Csc62015::Op_9e(void)
{
    reg.x.u=pPC->Get_20(Get_i());
	AddState(11);
}
/* RC */
void Csc62015::Op_9f(void)
{
	reg.r.c=0;
	AddState(1);
}
/* MV (n),A */
void Csc62015::Op_a0(void)
{
    Set_i8(pPC->Get_8(reg.x.p),reg.r.a,OPR1);
	reg.r.pc++;
	AddState(3);
}
/* MV (n),IL */
void Csc62015::Op_a1(void)
{
    Set_i8(pPC->Get_8(reg.x.p),reg.r.il,OPR1);
	reg.r.pc++;
	AddState(3);
}
/* MV (n),BA */
void Csc62015::Op_a2(void)
{
    Set_i16(pPC->Get_8(reg.x.p),reg.x.ba,OPR1);
	reg.r.pc++;
	AddState(4);
}
/* MV (n),I */
void Csc62015::Op_a3(void)
{
    Set_i16(pPC->Get_8(reg.x.p),reg.x.i,OPR1);
	reg.r.pc++;
	AddState(4);
}
/* MV (n),X */
void Csc62015::Op_a4(void)
{
    Set_i20(pPC->Get_8(reg.x.p),reg.x.x,OPR1);
	reg.r.pc++;
	AddState(5);
}
/* MV (n),Y */
void Csc62015::Op_a5(void)
{
    Set_i20(pPC->Get_8(reg.x.p),reg.x.y,OPR1);
	reg.r.pc++;
	AddState(5);
}
/* MV (n),U */
void Csc62015::Op_a6(void)
{
    Set_i20(pPC->Get_8(reg.x.p),reg.x.u,OPR1);
	reg.r.pc++;
	AddState(5);
}
/* MV (n),S */
void Csc62015::Op_a7(void)
{
    Set_i20(pPC->Get_8(reg.x.p),reg.x.s,OPR1);
	reg.r.pc++;
	AddState(5);
}
/* MV [lmn],A */
void Csc62015::Op_a8(void)
{
    pPC->Set_8(pPC->Get_20(reg.x.p),reg.r.a);
	reg.r.pc+=SIZE_20;
	AddState(5);
}
/* MV [lmn],IL */
void Csc62015::Op_a9(void)
{
    pPC->Set_8(pPC->Get_20(reg.x.p),reg.r.il);
	reg.r.pc+=SIZE_20;
	AddState(5);
}
/* MV [lmn],BA */
void Csc62015::Op_aa(void)
{
    pPC->Set_16(pPC->Get_20(reg.x.p),reg.x.ba);
	reg.r.pc+=SIZE_20;
	AddState(6);
}
/* MV [lmn],I */
void Csc62015::Op_ab(void)
{
    pPC->Set_16(pPC->Get_20(reg.x.p),reg.x.i);
	reg.r.pc+=SIZE_20;
	AddState(6);
}
/* MV [lmn],X */
void Csc62015::Op_ac(void)
{
    pPC->Set_20(pPC->Get_20(reg.x.p),reg.x.x);
	reg.r.pc+=SIZE_20;
	AddState(7);
}
/* MV [lmn],Y */
void Csc62015::Op_ad(void)
{
    pPC->Set_20(pPC->Get_20(reg.x.p),reg.x.y);
	reg.r.pc+=SIZE_20;
	AddState(7);
}
/* MV [lmn],U */
void Csc62015::Op_ae(void)
{
    pPC->Set_20(pPC->Get_20(reg.x.p),reg.x.u);
	reg.r.pc+=SIZE_20;
	AddState(7);
}
/* MV [lmn],S */
void Csc62015::Op_af(void)
{
    pPC->Set_20(pPC->Get_20(reg.x.p),reg.x.s);
	reg.r.pc+=SIZE_20;
	AddState(7);
}
/* MV [r3],A */
void Csc62015::Op_b0(void)
{
    pPC->Set_8(Get_d(SIZE_8),reg.r.a);
	AddState(4);
}
/* MV [r3],IL */
void Csc62015::Op_b1(void)
{
    pPC->Set_8(Get_d(SIZE_8),reg.r.il);
	AddState(4);
}
/* MV [r3],BA */
void Csc62015::Op_b2(void)
{
    pPC->Set_16(Get_d(SIZE_16),reg.x.ba);
	AddState(5);
}
/* MV [r3],I */
void Csc62015::Op_b3(void)
{
    pPC->Set_16(Get_d(SIZE_16),reg.x.i);
	AddState(5);
}
/* MV [r3],X */
void Csc62015::Op_b4(void)
{
    pPC->Set_20(Get_d(SIZE_20),reg.x.x);
	AddState(6);
}
/* MV [r3],Y */
void Csc62015::Op_b5(void)
{
    pPC->Set_20(Get_d(SIZE_20),reg.x.y);
	AddState(6);
}
/* MV [r3],U */
void Csc62015::Op_b6(void)
{
    pPC->Set_20(Get_d(SIZE_20),reg.x.u);
	AddState(6);
}
/* CMP (m),(n) */
void Csc62015::Op_b7(void)
{
	register DWORD	t;
    t=Get_i8(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    t-=Get_i8(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	Chk_Flag(t,SIZE_8);
	AddState(6);
}
/* MV [(n)],A */
void Csc62015::Op_b8(void)
{
    pPC->Set_8(Get_i(),reg.r.a);
	AddState(9);
}
/* MV [(n)],IL */
void Csc62015::Op_b9(void)
{
    pPC->Set_8(Get_i(),reg.r.il);
	AddState(9);
}
/* MV [(n)],BA */
void Csc62015::Op_ba(void)
{
    pPC->Set_16(Get_i(),reg.x.ba);
	AddState(10);
}
/* MV [(n)],I */
void Csc62015::Op_bb(void)
{
    pPC->Set_16(Get_i(),reg.x.i);
	AddState(10);
}
/* MV [(n)],X */
void Csc62015::Op_bc(void)
{
    pPC->Set_20(Get_i(),reg.x.x);
	AddState(11);
}
/* MV [(n)],Y */
void Csc62015::Op_bd(void)
{
    pPC->Set_20(Get_i(),reg.x.y);
	AddState(11);
}
/* MV [(n)],U */
void Csc62015::Op_be(void)
{
    pPC->Set_20(Get_i(),reg.x.u);
	AddState(11);
}
/* Undefined Code */
void Csc62015::Op_bf(void)
{
//UN_DEFINE
	AddState(1);
}
/* EX (m),(n) */
void Csc62015::Op_c0(void)
{
	register BYTE	d,s;
	register BYTE	w;
    d=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    s=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	w=Get_i8(d,OPR1); Set_i8(d,Get_i8(s,OPR2),OPR1); Set_i8(s,w,OPR2);
	AddState(7);
}
/* EXW (m),(n) */
void Csc62015::Op_c1(void)
{
	register BYTE	d,s;
	register WORD	w;
    d=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    s=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	w=Get_i16(d,OPR1); Set_i16(d,Get_i16(s,OPR2),OPR1); Set_i16(s,w,OPR2);
	AddState(10);
}
/* EXP (m),(n) */
void Csc62015::Op_c2(void)
{
	register BYTE	d,s;
	register DWORD	w;
    d=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    s=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	w=Get_i24(d,OPR1); Set_i24(d,Get_i24(s,OPR2),OPR1); Set_i24(s,w,OPR2);
	AddState(13);
}
/* EXL (m),(n) */
void Csc62015::Op_c3(void)
{
	register BYTE	d,s,w;
	AddState(5+3*reg.x.i);
    d=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    s=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	while(--reg.x.i!=0){
		w=Get_i8d(d); Set_i8d(d,Get_i8d(s)); Set_i8d(s,w);
		s++; d++;
	}
	w=Get_i8d(d); Set_i8d(d,Get_i8d(s)); Set_i8d(s,w);
}
/* DADL (m),(n) */
void Csc62015::Op_c4(void)
{
	register BYTE	d,s,b;
	register WORD	w;
	AddState(5+2*reg.x.i);
    d=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    s=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	w=hex2bcd(bcd2hex(Get_i8d(d))+bcd2hex(Get_i8d(s--)));
	Chk_Flag(w,SIZE_8);
	Set_i8d(d--,w);
	b=w;
	while(--reg.x.i!=0){
		w=hex2bcd(bcd2hex(Get_i8d(d))+bcd2hex(Get_i8d(s--))+reg.r.c);
		Chk_Flag(w,SIZE_8);
		Set_i8d(d--,w);
		b|=w;
	}
	Chk_Zero(b,SIZE_8);
//printf("DADL (m),(n)\n");
}
/* DADL (n),A */
void Csc62015::Op_c5(void)
{
	register BYTE	d,b;
	register WORD	w;
	AddState(4+reg.x.i);
    d=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	w=hex2bcd(bcd2hex(Get_i8d(d))+bcd2hex(reg.r.a));
	Chk_Flag(w,SIZE_8);
	Set_i8d(d--,w);
	b=w;
	while(--reg.x.i!=0){
		w=hex2bcd(bcd2hex(Get_i8d(d))+reg.r.c);
		Chk_Flag(w,SIZE_8);
		Set_i8d(d--,w);
		b|=w;
	}
	Chk_Zero(b,SIZE_8);
//printf("DADL (n),A\n");
}
/* CMPW (m),(n) */
void Csc62015::Op_c6(void)
{
	register DWORD	t;
    t=Get_i16(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    t-=Get_i16(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	Chk_Flag(t,SIZE_16);
	AddState(8);
}
/* CMPP (m),(n) */
void Csc62015::Op_c7(void)
{
	register DWORD	t;
    t=Get_i24(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    t-=Get_i24(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	Chk_Flag(t,SIZE_24);
	AddState(10);
}
/* MV (m),(n) */
void Csc62015::Op_c8(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    Set_i8(t,Get_i8(pPC->Get_8(reg.x.p),OPR2),OPR1);
	reg.r.pc++;
	AddState(6);
}
/* MVW (m),(n) */
void Csc62015::Op_c9(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    Set_i16(t,Get_i16(pPC->Get_8(reg.x.p),OPR2),OPR1);
	reg.r.pc++;
	AddState(8);
}
/* MVP (m),(n) */
void Csc62015::Op_ca(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    Set_i24(t,Get_i24(pPC->Get_8(reg.x.p),OPR2),OPR1);
	reg.r.pc++;
	AddState(10);
}
/* MVL (n),(m) */
void Csc62015::Op_cb(void)
{
	register BYTE	d,s;
	AddState(5+2*reg.x.i);
    d=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    s=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	while(--reg.x.i!=0) Set_i8d(d++,Get_i8d(s++));
	Set_i8d(d,Get_i8d(s));
}
/* MV (m),n */
void Csc62015::Op_cc(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    Set_i8(t,pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	AddState(3);
}
/* MVW (l),mn */
void Csc62015::Op_cd(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    Set_i16(t,pPC->Get_16(reg.x.p),OPR1);
	reg.r.pc+=SIZE_16;
	AddState(4);
}
/* TCL ??? */
void Csc62015::Op_ce(void)
{
//UN_DEFINE
	AddState(1);
}
/* MVLD (m),(n) */
void Csc62015::Op_cf(void)
{
	register BYTE	d,s;
	AddState(5+2*reg.x.i);
    d=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    s=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	while(--reg.x.i!=0) Set_i8d(d--,Get_i8d(s--));
	Set_i8d(d,Get_i8d(s));
}
/* MV (k),[lmn] */
void Csc62015::Op_d0(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    Set_i8(t,pPC->Get_8(pPC->Get_20(reg.x.p)),OPR1);
	reg.r.pc+=SIZE_20;
	AddState(7);
}
/* MVW (k),[lmn] */
void Csc62015::Op_d1(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    Set_i16(t,pPC->Get_16(pPC->Get_20(reg.x.p)),OPR1);
	reg.r.pc+=SIZE_20;
	AddState(8);
}
/* MVP (k),[lmn] */
void Csc62015::Op_d2(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    Set_i24(t,pPC->Get_24(pPC->Get_20(reg.x.p)),OPR1);
	reg.r.pc+=SIZE_20;
	AddState(9);
}
/* MVL (k),[lmn] */
void Csc62015::Op_d3(void)
{
	register BYTE	d;
	register DWORD	s;
	AddState(6+2*reg.x.i);
    d=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    s=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
    while(--reg.x.i!=0) Set_i8d(d++,pPC->Get_8(s++));
    Set_i8d(d,pPC->Get_8(s));
}
/* DSBL (m),(n) */
void Csc62015::Op_d4(void)
{
	register BYTE	d,s,b;
	register WORD	w;
	AddState(5+2*reg.x.i);
    d=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    s=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR2);
	reg.r.pc++;
	w=hex2bcd(100+bcd2hex(Get_i8d(d))-bcd2hex(Get_i8d(s--)));
	reg.r.c=1-(w>>8);
	Set_i8d(d--,w);
	b=w;
	while(--reg.x.i!=0){
		w=hex2bcd(100+bcd2hex(Get_i8d(d))-bcd2hex(Get_i8d(s--))-reg.r.c);
		reg.r.c=1-(w>>8);
		Set_i8d(d--,w);
		b|=w;
	}
	Chk_Zero(b,SIZE_8);
//printf("DSBL (m),(n)\n");
}
/* DSBL (n),A */
void Csc62015::Op_d5(void)
{
	register BYTE	d,b;
	register WORD	w;
	AddState(4+reg.x.i);
    d=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	w=hex2bcd(100+bcd2hex(Get_i8d(d))-bcd2hex(reg.r.a));
	reg.r.c=1-(w>>8);
	Set_i8d(d--,w);
	b=w;
	while(--reg.x.i!=0){
		w=hex2bcd(100+bcd2hex(Get_i8d(d))-reg.r.c);
		reg.r.c=1-(w>>8);
		Set_i8d(d--,w);
		b|=w;
	}
	Chk_Zero(b,SIZE_8);
//printf("DSBL (n),A\n");
}
/* CMPW (n),r2 */
void Csc62015::Op_d6(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i16(pPC->Get_8(reg.x.p),OPR1)-(Get_r(a)&MASK_16);
	reg.r.pc++;
	Chk_Flag(t,SIZE_16);
	AddState(7);
}
/* CMPP (n),r3 */
void Csc62015::Op_d7(void)
{
	register BYTE	a;
	register DWORD	t;
    a=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    t=Get_i20(pPC->Get_8(reg.x.p),OPR1)-Get_r(a);
	reg.r.pc++;
	Chk_Flag(t,SIZE_20);
	AddState(9);
}
/* MV [klm],(n) */
void Csc62015::Op_d8(void)
{
	register DWORD	t;
    t=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
    pPC->Set_8(t,Get_i8(pPC->Get_8(reg.x.p),OPR1));
	reg.r.pc++;
	AddState(6);
}
/* MVW [klm],(n) */
void Csc62015::Op_d9(void)
{
	register DWORD	t;
    t=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
    pPC->Set_16(t,Get_i16(pPC->Get_8(reg.x.p),OPR1));
	reg.r.pc++;
	AddState(7);
}
/* MVP [klm],(n) */
void Csc62015::Op_da(void)
{
	register DWORD	t;
    t=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
    pPC->Set_24(t,Get_i24(pPC->Get_8(reg.x.p),OPR1));
	reg.r.pc++;
	AddState(8);
}
/* MVL [klm],(n) */
void Csc62015::Op_db(void)
{
	register BYTE	s;
	register DWORD	d;
	AddState(6+2*reg.x.i);
    d=pPC->Get_20(reg.x.p);
	reg.r.pc+=SIZE_20;
    s=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
    while(--reg.x.i!=0) pPC->Set_8(d++,Get_i8d(s++));
    pPC->Set_8(d,Get_i8d(s));
}
/* MVP (k),lmn */
void Csc62015::Op_dc(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    Set_i24(t,pPC->Get_24(reg.x.p),OPR1);
	reg.r.pc+=SIZE_20;
	AddState(5);
}
/* EX A,B */
void Csc62015::Op_dd(void)
{
	register BYTE	t;
	t=reg.r.a; reg.r.a=reg.r.b; reg.r.b=t;
	AddState(3);
}
/* HALT */
void Csc62015::Op_de(void)
{
//printf("System HALT!!\n");/*exit(1);*/
//debug.isdebug=1;
    halt=1;
	AddState(2);
}
/* OFF */
void Csc62015::Op_df(void)
{
//printf("Software POWER OFF!!\n");/*exit(1);*/
//debug.isdebug=1;
	imem[IMEM_ISR]=0;
    off=1;
//	sc.save=1;
	AddState(2);
}
/* MV (n),[r3] */
void Csc62015::Op_e0(void)
{
	register BYTE	t;
	register DWORD	a;
	a=Get_d2(SIZE_8, &t);
    Set_i8(t,pPC->Get_8(a),OPR1);
	AddState(6);
}
/* MVW (n),[r3] */
void Csc62015::Op_e1(void)
{
	register BYTE	t;
	register DWORD	a;
	a=Get_d2(SIZE_16, &t);
    Set_i16(t,pPC->Get_16(a),OPR1);
	AddState(7);
}
/* MVP (n),[r3] */
void Csc62015::Op_e2(void)
{
	register BYTE	t;
	register DWORD	a;
	a=Get_d2(SIZE_20, &t);
    Set_i24(t,pPC->Get_24(a),OPR1);
	AddState(8);
}
/* MVL (n),[r3] */
void Csc62015::Op_e3(void)
{
	register BYTE	d,t;
	register DWORD	s;
	AddState(10+2*reg.x.i);
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    d=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	s=Get_r(t);
    while(--reg.x.i!=0) Set_i8d(((t&0x10)==0?d++:d--),pPC->Get_8(((t&0x10)==0?s++:--s)));
    Set_i8d(d,pPC->Get_8(((t&0x10)==0?s++:--s)));
	Set_r(t,s);
}
/* ROR A */
void Csc62015::Op_e4(void)
{
	register BYTE	a,c;
	a=reg.r.a;
	c=a&1;					/* b0 */
	a=(a>>1)|(c<<7);
	reg.r.c=c;
	Chk_Zero(a,SIZE_8);
	reg.r.a=a;
	AddState(2);
}
/* ROR (n) */
void Csc62015::Op_e5(void)
{
	register BYTE	a,c,t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	a=Get_i8(t,OPR1);
	c=a&1;					/* b0 */
	a=(a>>1)|(c<<7);
	reg.r.c=c;
	Chk_Zero(a,SIZE_8);
	Set_i8(t,a,OPR1);
	AddState(3);
}
/* ROL A */
void Csc62015::Op_e6(void)
{
	register BYTE	a,c;
	a=reg.r.a;
	c=(a>>7)&1;				/* b7 */
	a=(a<<1)|c;
	reg.r.c=c;
	Chk_Zero(a,SIZE_8);
	reg.r.a=a;
	AddState(2);
}
/* ROL (n) */
void Csc62015::Op_e7(void)
{
	register BYTE	a,c,t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	a=Get_i8(t,OPR1);
	c=(a>>7)&1;				/* b7 */
	a=(a<<1)|c;
	reg.r.c=c;
	Chk_Zero(a,SIZE_8);
	Set_i8(t,a,OPR1);
	AddState(3);
}
/* MV [r3],(n) */
void Csc62015::Op_e8(void)
{
	register BYTE	t;
	register DWORD	a;
	a=Get_d2(SIZE_8, &t);
    pPC->Set_8(a,Get_i8(t,OPR1));
	AddState(6);
}
/* MVW [r3],(n) */
void Csc62015::Op_e9(void)
{
	register BYTE	t;
	register DWORD	a;
	a=Get_d2(SIZE_16, &t);
    pPC->Set_16(a,Get_i16(t,OPR1));
	AddState(7);
}
/* MVP [r3],(n) */
void Csc62015::Op_ea(void)
{
	register BYTE	t;
	register DWORD	a;
	a=Get_d2(SIZE_20, &t);
    pPC->Set_24(a,Get_i24(t,OPR1));
	AddState(8);
}
/* MVL [r3],(n) */
void Csc62015::Op_eb(void)
{
	register BYTE	s,t;
	register DWORD	d;
	AddState(10+2*reg.x.i);
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
    s=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	d=Get_r(t);
    while(--reg.x.i!=0) pPC->Set_8(((t&0x10)==0?d++:--d),Get_i8d(((t&0x10)==0?s++:s--)));
    pPC->Set_8(((t&0x10)==0?d++:--d),Get_i8d(s));
	Set_r(t,d);
}
/* DSLL (n) */
void Csc62015::Op_ec(void)
{
	register BYTE	n;
	register DWORD	s,t,u,b;
	AddState(4+reg.x.i);
    n=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	t=Get_i8d(n);
	s=t<<4;
	Set_i8d(n--,s);
	u=t>>4;
	b=s;
	while(--reg.x.i!=0){
		t=Get_i8d(n);
		s=t<<4|u;
		Set_i8d(n--,s);
		u=t>>4;
		b|=s;
	}
	Chk_Zero(b,SIZE_8);
}
/* EX r,r' */
void Csc62015::Op_ed(void)
{
	register BYTE	t;
	register DWORD	w;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	w=Get_r2(t); Set_r2(t,Get_r2(t>>4)); Set_r2(t>>4,w);
	AddState(4);
}
/* SWAP A */
void Csc62015::Op_ee(void)
{
	reg.r.a=(reg.r.a>>4)|(reg.r.a<<4);
	Chk_Zero(reg.r.a,SIZE_8);
	AddState(3);
}
/* WAIT */
void Csc62015::Op_ef(void)
{
	AddState(1+reg.x.i);
	while(--reg.x.i!=0);				/* dummy !!! */
//	reg.x.i=0;							// for a little speed up!!
}
/* MV (m),[(n)] */
void Csc62015::Op_f0(void)
{
	register BYTE	t=0;
	register DWORD	a;
	a=Get_i2(&t);
    Set_i8(t,pPC->Get_8(a),OPR1);
	AddState(11);
}
/* MVW (m),[(n)] */
void Csc62015::Op_f1(void)
{
	register BYTE	t=0;
	register DWORD	a;
	a=Get_i2(&t);
    Set_i16(t,pPC->Get_16(a),OPR1);
	AddState(12);
}
/* MVP (m),[(n)] */
void Csc62015::Op_f2(void)
{
	register BYTE	t=0;
	register DWORD	a;
	a=Get_i2(&t);
    Set_i24(t,pPC->Get_24(a),OPR1);
	AddState(13);
}
/* MVL (l),[(n)] */
void Csc62015::Op_f3(void)
{
	register BYTE	d=0;
	register DWORD	s;
	AddState(10+2*reg.x.i);
	s=Get_i2(&d);
	d=Conv_imemAdr(d,OPR1);
    while(--reg.x.i!=0) Set_i8d(d++,pPC->Get_8(s++));
    Set_i8d(d,pPC->Get_8(s));
}
/* SHR A */
void Csc62015::Op_f4(void)
{
	register BYTE	a,c;
	a=reg.r.a;
	c=reg.r.c;
	reg.r.c=a&1;
	a=(a>>1)|(c<<7);
	Chk_Zero(a,SIZE_8);
	reg.r.a=a;
	AddState(2);
}
/* SHR (n) */
void Csc62015::Op_f5(void)
{
	register BYTE	a,c,t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	a=Get_i8(t,OPR1);
	c=reg.r.c;
	reg.r.c=a&1;
	a=(a>>1)|(c<<7);
	Chk_Zero(a,SIZE_8);
	Set_i8(t,a,OPR1);
	AddState(3);
}
/* SHL A */
void Csc62015::Op_f6(void)
{
	register BYTE	a,c;
	a=reg.r.a;
	c=reg.r.c;
	reg.r.c=a>>7;
	a=(a<<1)|c;
	Chk_Zero(a,SIZE_8);
	reg.r.a=a;
	AddState(2);
}
/* SHL (n) */
void Csc62015::Op_f7(void)
{
	register BYTE	a,c,t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	a=Get_i8(t,OPR1);
	c=reg.r.c;
	reg.r.c=a>>7;
	a=(a<<1)|c;
	Chk_Zero(a,SIZE_8);
	Set_i8(t,a,OPR1);
	AddState(3);
}
/* MV [(m)],(n) */
void Csc62015::Op_f8(void)
{
	register BYTE	t=1;
	register DWORD	a;
	a=Get_i2(&t);
    pPC->Set_8(a,Get_i8(t,OPR2));
	AddState(11);
}
/* MVW [(m)].(n) */
void Csc62015::Op_f9(void)
{
	register BYTE	t=1;
	register DWORD	a;
	a=Get_i2(&t);
    pPC->Set_16(a,Get_i16(t,OPR2));
	AddState(12);
}
/* MVP [(m)],(n) */
void Csc62015::Op_fa(void)
{
	register BYTE	t=1;
	register DWORD	a;
	a=Get_i2(&t);
    pPC->Set_24(a,Get_i24(t,OPR2));
	AddState(13);
}
/* MVL [(m)],(n) */
void Csc62015::Op_fb(void)
{
	register BYTE	s=1;
	register DWORD	d;
	AddState(10+2*reg.x.i);
	d=Get_i2(&s);
	s=Conv_imemAdr(s,OPR2);
    while(--reg.x.i!=0) pPC->Set_8(d++,Get_i8d(s++));
    pPC->Set_8(d,Get_i8d(s));
}
/* DSRL (n) */
void Csc62015::Op_fc(void)
{
	register BYTE	n;
	register DWORD	s,t,u,b;
	AddState(4+reg.x.i);
    n=Conv_imemAdr(pPC->Get_8(reg.x.p),OPR1);
	reg.r.pc++;
	t=Get_i8d(n);
	s=t>>4;
	Set_i8d(n++,s);
	u=t&MASK_4;
	b=s;
	while(--reg.x.i!=0){
		t=Get_i8d(n);
		s=t>>4|u<<4;
		Set_i8d(n++,s);
		u=t&MASK_4;
		b|=s;
	}
	Chk_Zero(b,SIZE_8);
}
/* MV r,r' */
void Csc62015::Op_fd(void)
{
	register BYTE	t;
    t=pPC->Get_8(reg.x.p);
	reg.r.pc++;
	Set_r2(t>>4,Get_r2(t));
	AddState(2);
}
/* IR */
void Csc62015::Op_fe(void)
{
	reg.x.s-=SIZE_20;
    pPC->Set_20(reg.x.s,reg.x.p-1);
	reg.x.s--;
    pPC->Set_8(reg.x.s,reg.x.f);
	reg.x.s--;
    pPC->Set_8(reg.x.s,imem[IMEM_IMR]);
    reg.x.p=pPC->Get_20(VECT_IR);
	AddState(1);
}
/* RESET */
void Csc62015::Op_ff(void)
{
    reg.x.p=pPC->Get_20(VECT_RESET);
	AddState(1);
}



/* execute one operation code(for after pre byte) */
inline short Csc62015::Step_sc62015_(void)
{
	register DWORD t;
	t=reg.x.p;
	reg.r.pc++;
    (this->*OpTbl[mem[t]])();
	Reset_Pre();
}
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
/* Load Memory from file													 */
/*  ENTRY :BYTE s=Slot No.(SLOT1-3)											 */
/*  RETURN:1:success, 0:error												 */
/*****************************************************************************/
bool Csc62015::Mem_Load(BYTE s)
{
#if 0
    bool r=0;
	FILE *fp;
/*	char *MacName[]={"???","E500","E500","E500"
					,"1480U","???","1490U","E550(E500)"
					,"E650?","U6000?"};*/
	printf(" SLOT%c:Loading...",0x31+s);
	if((fp=fopen(SlotName[s],"rb"))!=NULL){
		if(s==SLOT3){
			fread(&mem[SlotAdr[s]],1024,SlotSize[s],fp);
/*			printf("PC-%s(Ver.%d.%d)",MacName[mem[0xffff0]&15]
				,mem[0xffff0],mem[0xffff1]);*/
			printf("[ROM Ver.%d.%d]",mem[0xffff0],mem[0xffff1]);
			if(mem[0xffff0]>7){
				e6=1;							// E650&U6000mode
				emsmode=(emsmode&16)+(emsmode&7)<4?0:4;
				fread(&mem[SlotAdr[SLOT3EXT]],1024,SlotSize[SLOT3EXT],fp);
			}
			printf("success.\n"); r=1;
		}else{
			if(check_filesize(fp,SlotSize[s],UNIT_KB)){
				fread(&mem[SlotAdr[s]],1024,SlotSize[s],fp);
				printf("success.\n"); r=1;
			}else printf("skipped.\n");
		}
		fclose(fp);
	}else printf("file not found(%s).\n",SlotName[s]);
	return(r);
#endif
}
/*****************************************************************************/
/* Load EMS from file														 */
/*  ENTRY :none																 */
/*  RETURN:1:success, 0:error												 */
/*****************************************************************************/
bool Csc62015::EMS_Load(void)
{
#if 0
	int  i;
    bool r=0;
	FILE *fp;
//	char *bankstr[]={"192KB","2x128KB","4x128KB","4x64KB","4x192KB"};
	int banksize[]={0,192,256,512,256,768};
//	printf(" EMS  :Loading...[%s]",bankstr[(emsmode&7)-1]);
	printf(" EMS  :Loading...");
	if((fp=fopen(SlotName[EMS],"rb"))!=NULL){
		if(check_filesize(fp,banksize[emsmode&7],UNIT_KB)){
			switch((emsmode&7)){
			case 1:									//  192K(for DELTA1)
				fread(&mem[SlotAdr[EMS]],1024,SlotSize[EMS],fp); break;
			case 2:									//2x128K(for DELTA2)
				fread(&mem[BASE_128[1]],1024,128,fp);
				fread(&mem[BASE_128[0]],1024,128,fp); break;
			case 3:									//4x128K(for DELTA3)
				for(i=0;i<4;i++){
					fread(&mem[BASE_128[3-i]],1024,128,fp);
				} break;
			case 4:									//4x 64K(for DELTA4)
				for(i=0;i<4;i++){
					fread(&mem[BASE_64[3-i]],1024,64,fp);
				} break;
			case 5:									//4x192K(for super-DELTA)
				for(i=0;i<4;i++){
					fread(&mem[BASE_64[3-i]],1024,64,fp);
					fread(&mem[BASE_128[3-i]],1024,128,fp);
				} break;
			}
			printf("success.\n"); r=1;
		}else printf("skipped.\n");
		fclose(fp);
	}else printf("file not found(%s).\n",SlotName[EMS]);
	return(r);
#endif
}
/*****************************************************************************/
/* Save Memory to file														 */
/*  ENTRY :BYTE s=Slot No.(SLOT1-3)											 */
/*  RETURN:none																 */
/*****************************************************************************/
void Csc62015::Mem_Save(BYTE s)
{
#if 0
	FILE *fp;
	printf(" SLOT%c:Saving...",0x31+s);
	if((fp=fopen(SlotName[s],"wb"))!=NULL){
		fwrite(&mem[SlotAdr[s]],1024,SlotSize[s],fp);
		fclose(fp);
		printf("done.\n");
	}else printf("file cannot open.\n");
#endif
}
/*****************************************************************************/
/* Save EMS to file															 */
/*  ENTRY :none																 */
/*  RETURN:none																 */
/*****************************************************************************/
void Csc62015::EMS_Save(void)
{
#if 0
	int  i;
	FILE *fp;
	printf(" EMS  :Saving...");
	if((fp=fopen(SlotName[EMS],"wb"))!=NULL){
		switch((emsmode&7)){
		case 1:										//  192K(for DELTA1)
			fwrite(&mem[SlotAdr[EMS]],1024,SlotSize[EMS],fp); break;
		case 2:										//2x128K(for DELTA2)
			fwrite(&mem[BASE_128[1]],1024,128,fp);
			fwrite(&mem[BASE_128[0]],1024,128,fp); break;
		case 3:										//4x128K(for DELTA3)
			for(i=0;i<4;i++){
				fwrite(&mem[BASE_128[3-i]],1024,128,fp);
			} break;
		case 4:										//4x 64K(for DELTA4)
			for(i=0;i<4;i++){
				fwrite(&mem[BASE_64[3-i]],1024,64,fp);
			} break;
		case 5:										//4x192K(for super-DELTA)
			for(i=0;i<4;i++){
				fwrite(&mem[BASE_64[3-i]],1024,64,fp);
				fwrite(&mem[BASE_128[3-i]],1024,128,fp);
			} break;
		}
		fclose(fp);
		printf("done.\n");
	}else printf("file cannot open.\n");
#endif
}

/*---------------------------------------------------------------------------*/
/*****************************************************************************/
/* Initialize SC62015 CPU emulator (memory, register)						 */
/* RETURN: 0=error, 1=success												 */
/*****************************************************************************/
bool Csc62015::init(void)
{
	printf("MEMORY initializing...");
	if((mem=(BYTE *)malloc(MAX_MEM))==NULL) return(0);		/* alloc main ram */
//	if((imem=(BYTE *)malloc(MAX_IMEM))==NULL) return(0);	/* internal ram*/
	memset((void *)mem,255,MAX_MEM);			//initialize memory
	printf("done.\n");
//	printf("main memory(alloc:%p),internal memory(alloc:%p)\n",mem,imem);

	char t[16];
	if(usestatus){
		if((fp_status=fopen(fn_status,"rb"))!=NULL){
			fread(t, 1, HEAD_LEN, fp_status);
			if(strncmp(t,HEAD_STR,HEAD_LEN)!=0){		//bad image
				fclose(fp_status); fp_status=0;
			}
		}
	}
	if(fp_status){
		printf(" Restore status...");
		fseek(fp_status, REG_BGN, SEEK_SET);		//restore status
		fread(&reg,1,REG_LEN,fp_status);
		fseek(fp_status, IMEM_BGN, SEEK_SET);
		fread(imem,1,IMEM_LEN,fp_status);
//		imem[IMEM_SSR]|=8;
		fclose(fp_status); fp_status=0;
		printf("done.\n");
	}else{
		memset((void *)imem,0,MAX_IMEM);
		reg.x.f=0;									//initialize registers
		reg.x.ba=0;
		reg.x.i=0;
		reg.x.x=0;
		reg.x.y=0;
		reg.x.u=0xb0000;
		reg.x.s=0xc0000;
		reg.x.p=0;
	}

	Mem_Load(SLOT1);
	Mem_Load(SLOT2);
//	if(!Mem_Load(SLOT3)) debug.isdebug=1;
	if(emsmode!=0) EMS_Load();

	if(logsw) fp_log=fopen("sc62015.log","at");			//open log file



    /* Operation Code Table */
    void(Csc62015::*OpTbl[])(void)={
        &Csc62015::Op_00, &Csc62015::Op_01, &Csc62015::Op_02, &Csc62015::Op_03, &Csc62015::Op_04, &Csc62015::Op_05, &Csc62015::Op_06, &Csc62015::Op_07,

    };
    /*Op_08, Op_09, Op_0a, Op_0b, Op_0c, Op_0d, Op_0e, Op_0f,
        Op_10, Op_11, Op_12, Op_13, Op_14, Op_15, Op_16, Op_17,
        Op_18, Op_19, Op_1a, Op_1b, Op_1c, Op_1d, Op_1e, Op_1f,
        Op_20, Op_21, Op_22, Op_23, Op_24, Op_25, Op_26, Op_27,
        Op_28, Op_29, Op_2a, Op_2b, Op_2c, Op_2d, Op_2e, Op_2f,
        Op_30, Op_31, Op_32, Op_33, Op_34, Op_35, Op_36, Op_37,
        Op_38, Op_39, Op_3a, Op_3b, Op_3c, Op_3d, Op_3e, Op_3f,
        Op_40, Op_41, Op_42, Op_43, Op_44, Op_45, Op_46, Op_47,
        Op_48, Op_49, Op_4a, Op_4b, Op_4c, Op_4d, Op_4e, Op_4f,
        Op_50, Op_51, Op_52, Op_53, Op_54, Op_55, Op_56, Op_57,
        Op_58, Op_59, Op_5a, Op_5b, Op_5c, Op_5d, Op_5e, Op_5f,
        Op_60, Op_61, Op_62, Op_63, Op_64, Op_65, Op_66, Op_67,
        Op_68, Op_69, Op_6a, Op_6b, Op_6c, Op_6d, Op_6e, Op_6f,
        Op_70, Op_71, Op_72, Op_73, Op_74, Op_75, Op_76, Op_77,
        Op_78, Op_79, Op_7a, Op_7b, Op_7c, Op_7d, Op_7e, Op_7f,
        Op_80, Op_81, Op_82, Op_83, Op_84, Op_85, Op_86, Op_87,
        Op_88, Op_89, Op_8a, Op_8b, Op_8c, Op_8d, Op_8e, Op_8f,
        Op_90, Op_91, Op_92, Op_93, Op_94, Op_95, Op_96, Op_97,
        Op_98, Op_99, Op_9a, Op_9b, Op_9c, Op_9d, Op_9e, Op_9f,
        Op_a0, Op_a1, Op_a2, Op_a3, Op_a4, Op_a5, Op_a6, Op_a7,
        Op_a8, Op_a9, Op_aa, Op_ab, Op_ac, Op_ad, Op_ae, Op_af,
        Op_b0, Op_b1, Op_b2, Op_b3, Op_b4, Op_b5, Op_b6, Op_b7,
        Op_b8, Op_b9, Op_ba, Op_bb, Op_bc, Op_bd, Op_be, Op_bf,
        Op_c0, Op_c1, Op_c2, Op_c3, Op_c4, Op_c5, Op_c6, Op_c7,
        Op_c8, Op_c9, Op_ca, Op_cb, Op_cc, Op_cd, Op_ce, Op_cf,
        Op_d0, Op_d1, Op_d2, Op_d3, Op_d4, Op_d5, Op_d6, Op_d7,
        Op_d8, Op_d9, Op_da, Op_db, Op_dc, Op_dd, Op_de, Op_df,
        Op_e0, Op_e1, Op_e2, Op_e3, Op_e4, Op_e5, Op_e6, Op_e7,
        Op_e8, Op_e9, Op_ea, Op_eb, Op_ec, Op_ed, Op_ee, Op_ef,
        Op_f0, Op_f1, Op_f2, Op_f3, Op_f4, Op_f5, Op_f6, Op_f7,
        Op_f8, Op_f9, Op_fa, Op_fb, Op_fc, Op_fd, Op_fe, Op_ff
    };
*/
	return(1);
}

/*****************************************************************************/
/* Exitting SC62015 CPU emulator (save memory, register)					 */
/*****************************************************************************/
bool Csc62015::exit(void)
{
	Mem_Save(SLOT1);
	Mem_Save(SLOT2);
//	Mem_Save(SLOT3);
	if(emsmode>0&&emsmode<8) EMS_Save();
	if(logsw) fclose(fp_log);							//close log file

	if(usestatus){
		if(off){
			printf(" Save status...");
			if((fp_status=fopen(fn_status,"wb"))!=NULL){		//save status
				fwrite(HEAD_STR, 1, HEAD_LEN, fp_status);	//header
				fseek(fp_status, REG_BGN, SEEK_SET);
				fwrite(&reg,1,REG_LEN,fp_status);			//reg
				fseek(fp_status, IMEM_BGN, SEEK_SET);
				fwrite(imem,1,IMEM_LEN,fp_status);			//i-mem
				fclose(fp_status);
				printf("done.\n");
			}else printf("write eror!\n");
		}else{
			if((fp_status=fopen(fn_status,"wb"))!=NULL){		//clear status
				fclose(fp_status);
			}
		}
	}else{
		if((fp_status=fopen(fn_status,"wb"))!=NULL){			//clear status
			fclose(fp_status);
		}
	}
}

/*****************************************************************************/
/* execute one operation code												 */
/*****************************************************************************/
void Csc62015::step(void)
{
	register DWORD t;

//	if(timer.state<timer.chkspeed)
    {					//check for overrun
/*		if(reg.x.p==0xfffe8){						//hook IOCS
			if(imem[0xd6]==2 && reg.x.i==0x26) debug.isdebug=1;
		}*/
		if(logsw){
			if(log && fp_log!=0){					//check log mode
				if(reg.x.p==0xfffe8)				//check calling IOCS
					fprintf(fp_log,"IOCS:(cx)=%04X,i=%04X\n",imem[0xd6]+imem[0xd7]*256,reg.x.i);
//				debug.DisAsm_1(reg.x.p,fp_log);		//write execute log
			}
		}
		t=reg.x.p;
		reg.r.pc++;
        (this->*OpTbl[mem[t]])();
	}
}

/*---------------------------------------------------------------------------*/
/*****************************************************************************/
/* Get data from register													 */
/*  ENTRY :REGNAME regname=REG_xx											 */
/*  RETURN:DWORD value														 */
/*****************************************************************************/
DWORD Csc62015::get_reg(REGNAME regname)
{
	switch(regname){
	case REG_A :return(reg.r.a);
	case REG_IL:return(reg.r.il);
	case REG_BA:return(reg.x.ba);
	case REG_I :return(reg.x.i);
	case REG_X :return(reg.x.x);
	case REG_Y :return(reg.x.y);
	case REG_U :return(reg.x.u);
	case REG_S :return(reg.x.s);
	case REG_P :return(reg.x.p);
	case REG_F :return(reg.x.f);
    case REG_PC :return(reg.r.pc);
	}
}

/*****************************************************************************/
/* Set data to register														 */
/*  ENTRY :REGNAME regname=REG_xx, DWORD data=value							 */
/*  RETURN:none																 */
/*****************************************************************************/
void Csc62015::set_reg(REGNAME regname,DWORD data)
{
	switch(regname){
	case REG_A :reg.r.a =data; break;
	case REG_IL:reg.r.il=data; break;
	case REG_BA:reg.x.ba=data; break;
	case REG_I :reg.x.i =data; break;
	case REG_X :reg.x.x =data; break;
	case REG_Y :reg.x.y =data; break;
	case REG_U :reg.x.u =data; break;
	case REG_S :reg.x.s =data; break;
	case REG_P :reg.x.p =data; break;
	case REG_F :reg.x.f =data; break;
	}
}



/*****************************************************************************/
/* Oparation memory															 */
/*  ENTRY :DOWRD adr=address, OPRMODE opr=OPR_xxx DWORD data=value			 */
/*  RETURN:none																 */
/*****************************************************************************/
void Csc62015::opr_mem(DWORD adr,OPRMODE opr,BYTE data)
{
	switch(opr){
	case OPR_AND:mem[adr]&=data; break;
	case OPR_OR :mem[adr]|=data; break;
	case OPR_XOR:mem[adr]^=data; break;
	case OPR_ADD:mem[adr]+=data; break;
	case OPR_SUB:mem[adr]-=data; break;
	}
}
/*****************************************************************************/
/* Get data from internal memory											 */
/*  ENTRY :BYTE adr=address													 */
/*  RETURN:BYTE value														 */
/*****************************************************************************/
DWORD Csc62015::get_imem(BYTE adr)
{
	return(imem[adr]);
}
/*****************************************************************************/
/* Set data to internal memory												 */
/*  ENTRY :BYTE adr=address, BYTE data=value								 */
/*  RETURN:none																 */
/*****************************************************************************/
void Csc62015::set_imem(BYTE adr,BYTE data)
{
	imem[adr]=data;
}
/*****************************************************************************/
/* Oparation internal memory												 */
/*  ENTRY :BYTE adr=address, OPRMODE opr=OPR_xxx DWORD data=value			 */
/*  RETURN:none																 */
/*****************************************************************************/
void Csc62015::opr_imem(BYTE adr,OPRMODE opr,BYTE data)
{
	switch(opr){
	case OPR_AND:imem[adr]&=data; break;
	case OPR_OR :imem[adr]|=data; break;
	case OPR_XOR:imem[adr]^=data; break;
	case OPR_ADD:imem[adr]+=data; break;
	case OPR_SUB:imem[adr]-=data; break;
	}
}

void Csc62015::Load_Internal(QFile *)
{
}

void Csc62015::Load_Internal(QXmlStreamReader *)
{
}

void Csc62015::save_internal(QFile *)
{
}

void Csc62015::save_internal(QXmlStreamWriter *)
{
}

/*!
 \brief Get data from memory

 \fn Csc::get_mem
 \param adr     address
 \param size    SIZE_08 or SIZE_16 or SIZE_20 or SIZE_24
 \return DWORD  value
*/
DWORD Csc62015::get_mem(DWORD adr,int size)
{
    switch(size)
    {
    case SIZE_8 :return(pPC->Get_PC(adr));
    case SIZE_16:return(pPC->Get_PC(adr)+(pPC->Get_PC(adr+1)<<8));
    case SIZE_20:return((pPC->Get_PC(adr)+(pPC->Get_PC(adr+1)<<8)+(pPC->Get_PC(adr+2)<<16))&MASK_20);
    case SIZE_24:return((pPC->Get_PC(adr)+(pPC->Get_PC(adr+1)<<8)+(pPC->Get_PC(adr+2)<<16))&MASK_24);
    }
    return(0);
}
/*****************************************************************************/
/* Set data to memory														 */
/*  ENTRY :DOWRD adr=address, int size=SIZE_xx, DWORD data=value			 */
/*  RETURN:none																 */
/*****************************************************************************/
void Csc62015::set_mem(DWORD adr,int size,DWORD data)
{
    switch(size)
    {
    case SIZE_8 :
        pPC->Set_8(adr , (BYTE) data);
        break;
    case SIZE_16:
        pPC->Set_8(adr , (BYTE) data);
        pPC->Set_8(adr+1 , (BYTE) (data>>8));
        break;
    case SIZE_20:
        pPC->Set_8(adr , (BYTE) data);
        pPC->Set_8(adr+1 , (BYTE) (data>>8));
        pPC->Set_8(adr+2 , (BYTE) ((data>>16)&MASK_4));
        break;
    case SIZE_24:
        pPC->Set_8(adr , (BYTE) data);
        pPC->Set_8(adr+1 , (BYTE) (data>>8));
        pPC->Set_8(adr+2 , (BYTE) (data>>16));
        break;
    }
}

void Csc62015::Regs_Info(UINT8 Type)
{
#if 0
    switch(Type)
    {
    case 0:			// Monitor Registers Dialog
    case 2:			// For Log File
        sprintf(Regs_String,	"I:%.2x J:%.2x A:%.2x B:%.2x \r\nX:%.2x%.2x  Y:%.2x%.2x \r\nK:%.2x L:%.2x M:%.2x N:%.2x \r\nP:%.2x Q:%.2x R:%.2x DP:%.4x \r\nC:%s Z:%s",
            I_REG_I,I_REG_J,I_REG_A,I_REG_B,
            I_REG_Xh,I_REG_Xl,I_REG_Yh,I_REG_Yl,
            I_REG_K,I_REG_L,I_REG_M,I_REG_N,
            reg.r.p,reg.r.q,reg.r.r,reg.d.dp,
            reg.r.c?"1":".",reg.r.z?"1":".");
        break;
    case 1:			// For Log File
        sprintf(Regs_String,	"I:%.2x J:%.2x A:%.2x B:%.2x X:%.2x%.2x  Y:%.2x%.2x K:%.2x L:%.2x M:%.2x N:%.2x P:%.2x Q:%.2x R:%.2x DP:%.4x C:%s Z:%s",
            I_REG_I,I_REG_J,I_REG_A,I_REG_B,
            I_REG_Xh,I_REG_Xl,I_REG_Yh,I_REG_Yl,
            I_REG_K,I_REG_L,I_REG_M,I_REG_N,
            reg.r.p,reg.r.q,reg.r.r,reg.d.dp,
            reg.r.c?"1":".",reg.r.z?"1":".");

        break;
    }
#endif
}

void Csc62015::Reset(void)
{
}


bool	Csc62015::Get_Xin(void)
{
//	return(Xin);
}

void Csc62015::Set_Xin(bool data)
{

//	Xin = data;
}

bool Csc62015::Get_Xout(void)
{
//	return(Xout);
}

void Csc62015::Set_Xout(bool data)
{
//	Xout = data;
}

DWORD Csc62015::get_PC(void)
{
    return(get_reg(REG_PC));
}
#endif
