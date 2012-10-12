#ifndef TINYBASIC_H
#define TINYBASIC_H

#include <QObject>
#include <QByteArray>
#include <QMap>

#include "cpu.h"

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
    enum Action{INITIAL,UP_ARROW,DOWN_ARROW,ENTER,BREAK,NO_ACTION};

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
};

#endif // TINYBASIC_H
