#ifndef CF79107PJ_H
#define CF79107PJ_H

#include <QObject>
#include "common.h"
class CpcXXXX;
class QXmlStreamReader;
class QXmlStreamWriter;



class CCF79107PJ:public QObject{

public:

    typedef struct
    {
        quint64 X;

    } REGS;

    const char*	GetClassName(){ return("CCF79107PJ");}
    CpcXXXX		*pPC;

    bool	init(void);						//initialize
    bool	exit(void);						//end
    void	Reset(void);
    bool	step(void);
    bool    instruction(BYTE cmd);

    void	Load_Internal(QXmlStreamReader *);
    void	save_internal(QXmlStreamWriter *);

    CCF79107PJ(CpcXXXX *parent,int clk);
    virtual ~CCF79107PJ();

private:
    REGS r;

};


#endif // CF79107PJ_H

#if 0

/***************************************/
/* ??:comet.h ????              */
/* ??:comet?????               */
/* ??:????                      */
/* ??:1.0 ?                        */
/* ??:???                        */
/* ??:chai2010@2002.cug.edu.cn      */
/* ??:2005-5-3                      */
/***************************************/

#ifndef COMET_H
#define COMET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ???????? */

#define		NELEMS(x)	(sizeof(x) / sizeof((x)[0]))

#define		MEMSIZE		0x10000		/* ???? */
#define		pc_max		0xFC00		/* ???? */
#define		sp_start	0xFC00		/* ???	*/

#define		IO_ADDR		0xFD10		/* IO??	*/
#define		IO_FLAG		0xFD11		/* IO??	*/

#define		IO_FIO		0x0100		/* ??		*/
#define		IO_TYPE		0x1C00		/* ??		*/
#define		IO_MAX		0x00FF		/* ????	*/
#define		IO_ERROR	0x0200		/* ??		*/

#define		IO_IN		0x0000		/* ??		*/
#define		IO_OUT		0x0100		/* ??		*/
#define		IO_CHR		0x0400		/* ??		*/
#define		IO_OCT		0x0800		/* ???	*/
#define		IO_DEC		0x0C00		/* ???	*/
#define		IO_HEX		0x1000		/* ????	*/

/* comet??????? */

#define		off_t		unsigned short

/* comet???? */

typedef enum {
    HALT, LD, ST, LEA,
    ADD, SUB, MUL, DIV, MOD,
    AND, OR, EOR,
    SLA, SRA, SLL, SRL,
    CPA, CPL,
    JMP, JPZ, JMI, JNZ, JZE,
    PUSH, POP, CALL, RET,
} OpType;

/* comet???? */

struct { OpType op; char *str; int len; } opTab[] = {
    {HALT, "HALT", 1},

    {LD, "LD", 2}, {ST, "ST", 2}, {LEA, "LEA", 2},

    {ADD, "ADD", 2}, {SUB, "SUB", 2},
    {MUL, "MUL", 2}, {DIV, "DIV", 2}, {MOD, "MOD", 2},
    {AND, "AND", 2}, {OR, "OR", 2}, {EOR, "EOR", 2},

    {SLA, "SLA", 2}, {SRA, "SRA", 2}, {SLL, "SLL", 2}, {SRL, "SRL", 2},

    {CPA, "CPA", 2}, {CPL, "CPL", 2},

    {JMP, "JMP", 2},
    {JPZ, "JPZ", 2}, {JMI, "JMI", 2}, {JNZ, "JNZ", 2}, {JZE, "JZE", 2},

    {PUSH, "PUSH", 2}, {POP, "POP", 1},
    {CALL, "CALL", 2}, {RET, "RET", 1}
};

/* ???? */

typedef enum {
    HELP, GO, STEP, JUMP, REGS, IMEM, DMEM,
    ALTER, TRACE, PRINT, CLEAR, QUIT } DbType;

/* ???? */

struct { DbType db; char *s1, *s2; } dbTab[] = {
    {HELP , "help" , "h"},
    {GO   , "go"   , "g"},
    {STEP , "step" , "s"},
    {JUMP , "jump" , "j"},
    {REGS , "regs" , "r"},
    {IMEM , "iMem" , "i"},
    {DMEM , "dMem" , "d"},
    {ALTER, "alter", "a"},
    {TRACE, "trace", "t"},
    {PRINT, "print", "p"},
    {CLEAR, "clear", "c"},
    {QUIT , "quit" , "q"}
};

/* comet????? */

struct comet {
    off_t pc;
    short fr;
    short gr[5];
    short mem[MEMSIZE];
} cmt;

/* ???? */

char pgmName[32];

/* ?????? */

FILE * source;

/* ???? */

int debug = 0;

#endif

#endif