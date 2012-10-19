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

// size of our program ram
#define kRamSize   1800

#define INPUTOUTPUT_IN  2
#define INPUTOUTPUT_UNKNOWN 6

#define STACK_SIZE (sizeof(struct stack_for_frame)*5)
#define VAR_TYPE double    //short int
#define VAR_SIZE sizeof(VAR_TYPE) // Size of variables in bytes

#define PI 3.141592653

class CKeyword {
public:
    CKeyword(QString name = QString(),quint8 code = 0);

    QString Name;
    quint8  Code;
};

typedef short unsigned LINENUM;

class CTinyBasic : public CCPU
{
    Q_OBJECT
public:

    enum Mode{RUN,PRO,RESERVE,DEF,STANDARD };
    enum Action{INITIAL,RUNNING,UP_ARROW,DOWN_ARROW,ENTER,BREAK,NO_ACTION};
    enum NEXT_STEP {
        WARMSTART,PROMPT,DIRECT,QHOW,EXECLINE,
        EXECNEXTLINE,INTERPERATEATTXTPOS,
        FILES,LIST,CHAIN,LOAD,MEM,QWHAT,QSORRY,SAVE,NEXT,ASSIGNMENT,
        UNIMPLEMENTED,GOSUB,GOSUB_RETURN,FORLOOP,INPUT,PRINT,POKE,BYE,GETLN,GETLN_END,
        RUN_NEXT_STATEMENT,LIST_NEXT,LIST_PREV

    };

    NEXT_STEP nextStep;
    Mode        runMode;

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

       QByteArray commandBuffer;
       QByteArray backupCommandBuffer;
       QByteArray outputBuffer;
       QByteArray inputBuffer;



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
       void printnum(VAR_TYPE num, int size=8);
       void printUnum(unsigned int num);
       void printmsgNoNL(const unsigned char *msg);
       void loop();
       VAR_TYPE expression();
       unsigned char print_quoted_string();
       unsigned char *findline();
       void toUppercaseBuffer();
       void printline();
       VAR_TYPE expr4();
       VAR_TYPE expr3();
       VAR_TYPE expr2();
       void printmsg(const unsigned char *msg);
       void getln(char prompt);

       unsigned char *start;
       unsigned char *newEnd;
       unsigned char linelen;
           boolean isDigital;
           boolean alsoWait;
           int val;

           unsigned char var_for;
           VAR_TYPE initial_for, step_for, terminal_for;

           boolean breakFlag;
           bool waitForRTN;

           void go_MEM();
           void go_NEW();
           void go_RUN();
           void go_LIST(LINENUM lineNb = 0);
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

           unsigned char program[kRamSize];
           unsigned char *txtpos,*list_line;
           unsigned char expression_error;
           unsigned char *tempsp;

           enum ANGLE_MODE { DEGREE,RADIAN,GRAD};
           ANGLE_MODE angleMode;
           double convertToRad(double angle);
           void convertLine();

           int cursorPos;
           void switchMode();
           bool inputMode;
           qint64 waitState;
           bool pauseFlag;
           void go_PAUSE();
           void go_LIST_NEXT();
           void go_LIST_PREV();
           QMap<LINENUM,unsigned char*> lineMap;
           void scanLines();
           bool inLIST;
};

#endif // TINYBASIC_H
