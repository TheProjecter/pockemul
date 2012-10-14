#ifndef TINYBASIC_H
#define TINYBASIC_H

#include <QObject>
#include <QByteArray>
#include <QMap>

#include "cpu.h"

#ifndef boolean
#define boolean int
#define true 1
#define false 0
#endif

class CKeyword {
public:
    CKeyword(QString name = QString(),quint8 code = 0);

    QString Name;
    quint8  Code;
};

class CTinyBasic : public CCPU
{
    Q_OBJECT
public:

    enum Mode{RUN,COMMAND };
    enum Action{INITIAL,RUNNING,UP_ARROW,DOWN_ARROW,ENTER,BREAK,NO_ACTION};
    enum NEXT_STEP {
        WARMSTART,PROMPT,DIRECT,QHOW,EXECLINE,
        EXECNEXTLINE,INTERPERATEATTXTPOS,
        FILES,LIST,CHAIN,LOAD,MEM,QWHAT,QSORRY,SAVE,NEXT,ASSIGNMENT,
        UNIMPLEMENTED,GOSUB,GOSUB_RETURN,FORLOOP,INPUT,PRINT,POKE,BYE,GETLN,GETLN_END,
        RUN_NEXT_STATEMENT

    };

    NEXT_STEP nextStep;

       CTinyBasic(CPObject *parent = 0);

       virtual	bool	init(void);						//initialize
       virtual	bool	exit(void);						//end
       virtual void	step(void);						//step SC61860

       virtual	void	Load_Internal(QXmlStreamReader *);
       virtual	void	save_internal(QXmlStreamWriter *);
       virtual	bool	Get_Xin(void){return true;}
       virtual	void	Set_Xin(bool){}
       virtual	bool	Get_Xout(void){return true;}
       virtual	void	Set_Xout(bool){}

       virtual	DWORD	get_PC(void){return 0;}					//get Program Counter
       virtual void	Regs_Info(UINT8){}

       virtual const char*	GetClassName(){ return("CTINYBASIC");}

       virtual void	Reset(void);



       void Interpret(QByteArray, int pos = 0);
       void inputCommand(QByteArray);
       void Parse();

       QByteArray commandBuffer;
       QByteArray outputBuffer;

       QMap<QByteArray,CKeyword> CommandMap;

       QMap<int,QByteArray> basicLines;

       Mode mode;
       void saveBasicLine();
       void test();
       void executeCommand(QByteArray code, Action action = INITIAL);
       void go_LIST(QByteArray code,Action action = INITIAL);
       void go_RUN(QByteArray code,Action action = INITIAL);

       quint8 runningCmd;


       void inputChar(quint8 c);
       void outputChar(QByteArray ba);

       int curDispLine;
       void clearOutput();
       QByteArray convertToDisplay(QByteArray ba);
       QByteArray go_PI();
       void outchar(unsigned char c);
       void ignore_blanks();
       void scantable(unsigned char *table);
       int isValidFnChar(char c);
       unsigned char *filenameWord();
       void line_terminator();
       void setup();
       unsigned char breakcheck();
       int inchar();
       unsigned short testnum();
       void pushb(unsigned char b);
       unsigned char popb();
       void printnum(int num);
       void printUnum(unsigned int num);
       void printmsgNoNL(const unsigned char *msg);
       void loop();
       short expression();
       unsigned char print_quoted_string();
       unsigned char *findline();
       void toUppercaseBuffer();
       void printline();
       short expr4();
       short expr3();
       short expr2();
       void printmsg(const unsigned char *msg);
       void getln(char prompt);

       unsigned char *start;
       unsigned char *newEnd;
       unsigned char linelen;
           boolean isDigital;
           boolean alsoWait;
           int val;

           unsigned char var_for;
           short int initial_for, step_for, terminal_for;

           boolean breakFlag;
           bool waitForRTN;

           void go_MEM();
           void go_NEW();
           void go_RUN();
           void go_LIST();
           void go_PRINT();
           void go_GOTO();
           void go_SAVE();
           void go_RETURN();
           void go_NEXT();
           void go_GOSUB();
           void go_FORLOOP();
           void go_IF();
           void go_INPUT();
           void go_ASSIGNMENT();
};

#endif // TINYBASIC_H
