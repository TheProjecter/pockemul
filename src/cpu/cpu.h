#ifndef CPU_H
#define CPU_H

#include <QtCore/QFile>

#include "Debug.h"
class CpcXXXX;


#define		MASK_4		0xf				/*  4bit data mask */
#define		MASK_8		0xff			/*  8bit data mask */
#define		MASK_16		0xffff			/* 16bit data mask */
#define		MASK_20		0xfffff			/* 20bit data mask */
#define		MASK_24		0xffffff		/* 24bit data mask */
#define		SIZE_8		1				/*  8bit data size(by byte) */
#define		SIZE_16		2				/* 16bit data size(by byte) */
#define		SIZE_20		3				/* 20bit data size(by byte) */
#define		SIZE_24		SIZE_20+1		/* 24bit dummy data size */



class CCPU:public QObject{
Q_OBJECT
public:
	Cdebug	*pDEBUG;
	CpcXXXX *pPC;

	virtual	bool	init(void) = 0;						//initialize
	virtual	bool	exit(void);						//end
	virtual void	step(void) =0;						//step SC61860

	virtual	void	Load_Internal(QFile *file) =0;
	virtual	void	save_internal(QFile *file) =0;
			void	save(void);

	virtual	DWORD	get_mem(DWORD adr,int size) =0;		//get memory
	virtual	void	set_mem(DWORD adr,int size,DWORD data) =0;	//set memory
	
	virtual	bool	Get_Xin(void) =0;
	virtual	void	Set_Xin(bool) =0;
	virtual	bool	Get_Xout(void) =0;
	virtual	void	Set_Xout(bool) =0;

	virtual	DWORD	get_PC(void) =0;					//get Program Counter
	virtual void	Regs_Info(UINT8) =0;

    const char	*fn_status;					//status file name
	bool	usestatus;						//use status file(0:no,1:yes)
	bool	halt;					//halt,off flag
	bool	end,savesw, log,logsw;	//etc.flag
    char	Regs_String[1024];

	FILE	*fp_log;			// file pointer to log file
    const char    *fn_log;

    BYTE	imem[0x200];			// Internal memory

	int		CallSubLevel;

	void	Check_Log(void);

    virtual const char*	GetClassName(){ return("CCPU");};

	virtual void	Reset(void) = 0;

	CCPU(CPObject *parent)
	{				//[constructor]
		pPC = (CpcXXXX*) parent;
		
		halt=0;				//halt?(0:none, 1:halting)
		end=0;				//program end?(0:none, 1:end)
		savesw=1;			//end with memory save?(0:no, 1:yes)
		log=0;				//execute log?(0:off, 1:on)
		logsw=false;			//log mode?(0:off, 1:on)
		usestatus=0;
		fp_status=0;
		fp_log=0;
        fn_log="cpu.log";
		CallSubLevel=0;

        for (int i=0;i<0x200;i++) imem[i]=0;

	};
private:
	FILE *fp_status;		//pointer to status file
};


#endif
