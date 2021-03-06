#ifndef E500_H
#define E500_H

#include "pcxxxx.h"

class CHD61102;
//class CRP5C01;

class Ce500:public CpcXXXX{						//E500 emulator main class
Q_OBJECT
public:
    const char*	GetClassName(){ return("Ce500");}

    virtual void	TurnON(void);
    virtual void	TurnOFF(void);


    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);
    virtual BYTE	Get_PortA(void);
    virtual BYTE	Get_PortB(void);



//	bool	Mem_Mirror(DWORD *d);
    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d, DWORD *data);


    virtual bool	LoadExtra(QFile *);
    virtual bool	SaveExtra(QFile *);

    virtual UINT8 in(UINT8 address);
    virtual UINT8 out(UINT8 address,UINT8 value);

    Ce500(CPObject *parent = 0);

    virtual ~Ce500()
    {								//[constructor]
    }
    bool init();

    CHD61102        *pHD61102_1;
    CHD61102        *pHD61102_2;

//    CRP5C01         *pRP5C01;

    void disp(qint8 cmd, DWORD data);
    BYTE getKey();
    bool run();

    void computeSound();
    virtual void MemMirror(DWORD *d);
    virtual void initExtension();

protected:
//	BYTE previous_key;

private:
    bool Xin,Xout,start2khz,start4khz;
    qint64 tmp_state;


};

class Ce550:public Ce500{						//E500 emulator main class
Q_OBJECT
public:
    const char*	GetClassName(){ return("Ce550");}

    virtual void MemMirror(DWORD *d);
    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d, DWORD *data);

    Ce550(CPObject *parent = 0);

    virtual ~Ce550()
    {								//[constructor]
    }



};
#endif // E500_H
