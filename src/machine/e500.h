#ifndef E500_H
#define E500_H

#include "pcxxxx.h"

class Ce500:public CpcXXXX{						//E500 emulator main class
Q_OBJECT
public:
    const char*	GetClassName(){ return("Ce500");}

    void	TurnON(void);


    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);
    virtual BYTE	Get_PortA(void);
    virtual BYTE	Get_PortB(void);

    virtual bool	InitDisplay(void);
    //bool	CompleteDisplay(void);
    virtual void UpdateFinalImage(void);

//	bool	Mem_Mirror(DWORD *d);
    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);


    virtual bool	LoadExtra(QFile *);
    virtual bool	SaveExtra(QFile *);

    virtual UINT8 in(UINT8 address);
    virtual UINT8 out(UINT8 address,UINT8 value);

    Ce500(CPObject *parent = 0);

    virtual ~Ce500()
    {								//[constructor]
    }
protected:
//	BYTE previous_key;


};

#endif // E500_H
