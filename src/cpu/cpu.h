#ifndef CPU_H
#define CPU_H

#include <QtCore/QFile>
#include "common.h"

class CPObject;
class CpcXXXX;
class Cdebug;
class QXmlStreamReader;
class QXmlStreamWriter;
class CregCPU;


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

    virtual	void	Load_Internal(QFile *) =0;
    virtual	void	Load_Internal(QXmlStreamReader *) =0;
    virtual	void	save_internal(QFile *) =0;
    virtual	void	save_internal(QXmlStreamWriter *) =0;
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
    bool    resetFlag;
    char	Regs_String[1024];

	FILE	*fp_log;			// file pointer to log file
    const char    *fn_log;

    BYTE	imem[0x200];			// Internal memory
    int   imemsize;

    int		CallSubLevel,prevCallSubLevel;

	void	Check_Log(void);

    virtual const char*	GetClassName(){ return("CCPU");}

	virtual void	Reset(void) = 0;

    CCPU(CPObject *parent);

    QByteArray getimem();

    CregCPU * regwidget;
    void setImemBit(WORD adr, int bit, BYTE data);
private:
	FILE *fp_status;		//pointer to status file
};


#endif
