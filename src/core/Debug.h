#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "pobject.h"

typedef	struct{
	char	len;
    const char	*nim;
    const char	*descr;
}	DisAsmTbl;

extern 	DisAsmTbl	AsmTbl_sc61860[];
extern 	DisAsmTbl	AsmTbl_lh5801[];

class Cdebug:public CPObject
{
    Q_OBJECT
public:
	bool	isdebug, breakf,debugged;			//debug,break point flag
	DWORD	breakadr;					//break point address
	char	Buffer[255];
	DWORD	DasmAdr,NextDasmAdr;
	bool init(void);					//initialize
	bool exit(void);					//end
	DWORD DisAsm_File(DWORD adr,FILE *fp);	//disasm 1 line to File

    void header(DWORD adr,int l);
	virtual	DWORD	DisAsm_1(DWORD adr) = 0;			//disasm 1 line to Buffer

	DisAsmTbl	*AsmTbl;


	Cdebug(CPObject *parent)	: CPObject(parent)
	{							//[constructor]
		breakf		= 0;
		breakadr	= 0;
		debugged	= 0;	//break point(0:disable, 1:enable)
		isdebug		= 0;			//debug?(0:none, 1:debugging)
	};
	
	virtual ~Cdebug()
	{
	};
private:
};

class Cdebug_sc61860:public Cdebug{
    Q_OBJECT
public:
	DWORD DisAsm_1(DWORD adr);			//disasm 1 line to Buffer

		Cdebug_sc61860(CPObject *parent)	: Cdebug(parent)
		{
			AsmTbl = AsmTbl_sc61860;
		}
		virtual ~Cdebug_sc61860(){}

};


class Cdebug_lh5801:public Cdebug{
    Q_OBJECT
public:
	DWORD DisAsm_1(DWORD adr);			//disasm 1 line to Buffer

		Cdebug_lh5801(CPObject *parent)	: Cdebug(parent)
		{
			AsmTbl = AsmTbl_sc61860;
		}

};

class Cdebug_z80:public Cdebug{
    Q_OBJECT
public:
    DWORD DisAsm_1(DWORD adr);			//disasm 1 line to Buffer

        Cdebug_z80(CPObject *parent)	: Cdebug(parent)
        {
            AsmTbl = AsmTbl_sc61860;
        }

};
#endif
