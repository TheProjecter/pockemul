/*** PC-E500 Emulator [SC62015.H] *************************/
/* SC62015 CPU emulation class                            */
/*                       Copyright (C) Matsumo 1998,1999  */
/**********************************************************/

#if 0

#ifndef SC62015_H
#define SC62015_H

#include "cpu.h"

//#include "e500.h"


#define		MAX_MEM		0x190000		/* main + EMS memory size */
#define		MAX_IMEM	0x100			/* internal memory size */
#define		MASK_4		0xf				/*  4bit data mask */
#define		MASK_8		0xff			/*  8bit data mask */
#define		MASK_16		0xffff			/* 16bit data mask */
#define		MASK_20		0xfffff			/* 20bit data mask */
#define		MASK_24		0xffffff		/* 24bit data mask */
#define		SIZE_8		1				/*  8bit data size(by byte) */
#define		SIZE_16		2				/* 16bit data size(by byte) */
#define		SIZE_20		3				/* 20bit data size(by byte) */
#define		SIZE_24		SIZE_20+1		/* 24bit dummy data size */

#define		IMEM_KOL	0xf0			// Key Output Port(L)
#define		IMEM_KOH	0xf1			// Key Output Port(H)
#define		IMEM_KI		0xf2			// Key Input Port
#define		IMEM_EOL	0xf3			// E-port Output Buffer(L)
#define		IMEM_EOH	0xf4			// E-port Output Buffer(H)
#define		IMEM_EIL	0xf5			// E-port Input Buffer(L)
#define		IMEM_EIH	0xf6			// E-port Input Buffer(H)

#define		IMEM_UCR	0xf7			// UART Control Register
#define		IMEM_USR	0xf8			// UART Status Register
#define		IMEM_RxD	0xf9			// UART Receive Buffer
#define		IMEM_TxD	0xfa			// UART Send Buffer

#define		IMEM_IISR	0xeb			/* Interrupt In Service Register address*/
#define		IMEM_IMR	0xfb			/* Interrupt Mask Register address*/
#define		IMEM_ISR	0xfc			/* Interrupt Status Register address*/
#define		IMEM_SCR	0xfd			/* System Control Register address*/
#define		IMEM_SSR	0xff			/* System Status Register address*/

#define		VECT_IR		0xffffa			/* Interrupt Vector address */
#define		VECT_RESET	0xffffd			/* Reset Vector address */

#define HEAD_STR		"E500STA"		//status file
#define HEAD_BGN		0
#define HEAD_LEN		7
#define REG_BGN			HEAD_LEN
#define REG_LEN			25
#define IMEM_BGN		(HEAD_LEN+REG_LEN)
#define IMEM_LEN		256

/*--------------------------------------------------------------------------*/
enum REGNAME{					//register name
	REG_A , REG_IL, REG_BA, REG_I, REG_X ,
	REG_Y , REG_U, REG_S , REG_P , REG_F
};
enum OPRMODE{					//operation mode
	OPR_AND, OPR_OR , OPR_XOR, OPR_ADD, OPR_SUB
};
enum MEMUNIT{					//memory unit
	UNIT_B, UNIT_KB, UNIT_MB
};

class Csc62015:public Csc{
public:
	short init(void);						//initialize
	void exit(void);						//end
	short step(void);						//step SC62015
	DWORD get_reg(REGNAME regname);			//get register
	void set_reg(REGNAME regname,DWORD data);	//set register
	DWORD get_mem(DWORD adr,int size);		//get memory
	void set_mem(DWORD adr,int size,DWORD data);	//set memory
	void opr_mem(DWORD adr,OPRMODE opr,BYTE data);	//operation memory
	DWORD get_imem(BYTE adr);				//get i-mem
	void set_imem(BYTE adr,BYTE data);		//set i-mem
	void opr_imem(BYTE adr,OPRMODE opr,BYTE data);	//operation i-mem
	bool check_filesize(FILE *fp,int size,MEMUNIT unit);	//check file size
	char *fn_status;					//status file name
	bool usestatus;						//use status file(0:no,1:yes)
	bool *SlotName[4];					//slot filename
	BOOL	halt,off;					//halt,off flag
	BOOL	end,save, e6, log,logsw;	//etc.flag
	BYTE	emsmode;					//ems memory size
	Csc62015(){									//[constructor]
		halt=0;				//halt?(0:none, 1:halting)
		off=0;				//off?(0:none, 1:off)
		end=0;				//program end?(0:none, 1:end)
		save=0;				//end with memory save?(0:no, 1:yes)
		SlotName[0]="s1.ram"; SlotName[1]="s2.ram";
		SlotName[2]="s3.rom"; SlotName[3]="ems.ram";
		e6=0;				//E650 mode?(0:no, 1:yes)
		emsmode=0;			//EMS mode(0:none, 1-5:Delta 1-4 or Super)
		log=0;				//execute log?(0:off, 1:on)
		logsw=0;				//log mode?(0:off, 1:on)
		usestatus=0;
		fp_status=0;
		fn_status="e500e.sta";
	};
private:
	bool Mem_Load(BYTE s);
	bool EMS_Load(void);
	void Mem_Save(BYTE s);
	void EMS_Save(void);
    //FILE *fp_log;			// file pointer to log file
	FILE *fp_status;		//pointer to status file
	

	
};

#endif
