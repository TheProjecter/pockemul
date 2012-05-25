#ifndef G850_H
#define G850_H

#include <QMenu>

#include "pcxxxx.h"



class CZ80;


class Cg850v:public CpcXXXX{

Q_OBJECT

public:
    const char*	GetClassName(){ return("Cg850");}

    bool	LoadConfig(QXmlStreamReader *);
    bool	SaveConfig(QXmlStreamWriter *);
    bool	InitDisplay(void);
    bool	CompleteDisplay(void);

    virtual bool	run(void);				// emulator main
    void	Set_Port(PORTS Port,BYTE data);
    BYTE	Get_Port(PORTS Port);

    virtual bool	Mem_Mirror(DWORD *d);
    void	TurnON(void);
    void    Reset(void);


    virtual bool Chk_Adr(DWORD *d,DWORD data);
    virtual bool Chk_Adr_R(DWORD *d,DWORD data);
    UINT8		in(UINT8 address);
    UINT8       out(UINT8 address,UINT8 value);
    virtual bool Set_Connector(void);
    virtual bool Get_Connector(void);
    BYTE        getKey(void);

    virtual bool	init(void);				// initialize

    Cg850v(CPObject *parent = 0);
    virtual ~Cg850v();

};





#endif // G850_H
