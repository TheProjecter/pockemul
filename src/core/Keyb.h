#ifndef _KEYB_H_
#define _KEYB_H_

#include <QList>
#include <QXmlDefaultHandler>

#include "pobject.h"

enum KEY_NAME {
K_BRK,
K_UA,
K_DA,
K_LA,
K_RA,
K_MOD,
K_INS,
K_DEL,
K_CLR,
K_RET,
K_SML,
K_DEF,
K_SHT,
K_OB,
K_OF,
K_RCL,
K_RSV,
K_SIGN,
K_CAL,
K_BASIC,
K_SQR,
K_ROOT,
K_POT,
K_EXP,
K_XM,
K_STAT,
K_1X,
K_LOG,
K_LN,
K_DEG,
K_HEX,
K_MPLUS,
K_CCE,
K_FSE,
K_TAN,
K_COS,
K_SIN,
K_HYP,
K_RM,
K_CTRL,K_BS,K_KBII,
K_F1,K_F2,K_F3,K_F4,K_F5,K_F6,K_F7,
K_A,K_B,K_C,K_D,K_E,K_F,K_G,K_H,K_I,K_J,K_K,K_L,K_M,K_N,K_O,K_P,K_Q,K_R,K_S,K_T,K_U,K_V,K_W,K_X,K_Y,K_Z,
K_0,K_1,K_2,K_3,K_4,K_5,K_6,K_7,K_8,K_9
};


#define	MAX_KO		12					//KOn(0-11)
#define NUL			0xff
#define NM			15

#define K_BRK		0x03		/* ctrl-C */
#define K_DA		0x04 // ???
#define K_UA		0x05 // ???
#define K_LA		0x06 // ???
#define K_RA		0x07 // ???
#define K_MOD		0x08
#define K_INS		0x09
#define K_DEL		0x0a
#define K_CLR		0x0c
#define K_RET		0x0d
#define K_SML		0x14
#define K_DEF		0xde
#define K_SHT		0x11
#define K_OB		0x3f
#define K_OF		0x3e

#define K_0         0xa0
#define K_1         0xa0
#define K_2         0xa0
#define K_3         0xa0
#define K_4         0xa0
#define K_5         0xa0
#define K_6         0xa0
#define K_7         0xa0
#define K_8         0xa0
#define K_9         0xa0
#define K_MIN         0xa0
#define K_PT         0xa0
#define K_SLH         0xa0

#define K_PRINT		0xc0
#define K_PFEED		0xc1
#define K_PBFEED	0xc2

#define K_EJECT		0xd0
#define K_PLAY		0xd1
#define K_STOP		0xd2
#define K_LOAD		0xd3
#define K_RECORD	0xd4
#define K_CTRL      0xd5
#define K_BS        0xd6
#define K_KBII      0xd7

#define K_RCL		0xde
#define K_RSV		0xdf

#define K_SIGN		0xe0
#define K_CAL		0xe1
#define K_BASIC		0xe2
#define K_SQR		0xe3
#define K_ROOT		0xe4
#define K_POT		0xe5
#define K_EXP		0xe6
#define K_XM		0xe7
#define K_STAT		0xe8
#define K_1X		0xe9
#define K_LOG		0xea
#define K_LN		0xeb
#define K_DEG		0xec
#define K_HEX		0xed
#define K_MPLUS		0xee
#define K_CCE		0xef
#define K_FSE		0xf0
#define K_TAN		0xf1
#define K_COS		0xf2
#define K_SIN		0xf3
#define K_HYP		0xf4
#define K_RM		0xf5

#define K_F1		0xf6
#define K_F2		0xf7
#define K_F3		0xf8
#define K_F4		0xf9
#define K_F5		0xfa
#define K_F6		0xfb
#define K_F7		0xfc

#define K_A			0x41
#define K_B			0x42
#define K_C			0x43
#define K_D			0x44
#define K_E			0x45
#define K_F			0x46
#define K_G			0x47
#define K_H			0x48
#define K_I			0x49
#define K_J			0x50
#define K_K			0x51
#define K_L			0x52
#define K_M			0x53
#define K_N			0x54
#define K_O			0x55
#define K_P			0x56
#define K_Q			0x57
#define K_R			0x58
#define K_S			0x59
#define K_T			0x60
#define K_U			0x61
#define K_V			0x62
#define K_W			0x63
#define K_X			0x64
#define K_Y			0x65
#define K_Z			0x66

extern BYTE scandef_pc1250[];
extern BYTE scandef_pc1350[];
extern BYTE scandef_pc1360[];
extern BYTE scandef_pc2500[];
extern BYTE scandef_pc1401[];
extern BYTE scandef_pc1403[];
extern BYTE scandef_pc1450[];
extern BYTE scandef_pc1500[];
extern BYTE scandef_pc1600[];


class CKey{
public:
	CKey(int ScanCode, QString Description,QRect Rect = QRect());
    ~CKey(){}
	
	int		Icode;
    QString	Description;
	int		ScanCode;
	QRect	Rect;
protected:
};


class KEYBMAPParser : public QXmlDefaultHandler,CPObject
{
public:
  bool startDocument();
  bool endElement( const QString&, const QString&, const QString &name );
  bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs );

    KEYBMAPParser(CPObject *parent = 0)	: CPObject(parent){}
private:
  bool inKeyboard;
};

class Ckeyb : public CPObject
{
public:
	virtual bool init(void);				//initialize
	virtual bool exit(void);				//end
	virtual void read_config(void){};		//read key config
	void	Set_KS(BYTE data);
	BYTE	Get_KS(void);

	BYTE	Read(BYTE);						// Scan keyboard
	bool	CheckKon();
	int		CheckOff();

	QString KeyString(QPoint pts);
	BYTE	KeyClick(QPoint pts);
	void	keyscan(void);
	BYTE	*scandef;

	BYTE	KStrobe;							// Last K strobe signal sent
	BYTE	keym[200];
	bool	access;							//KO access flag(0:none,1:access)
	QString fn_KeyMap;
    bool	Kon;
	BYTE	LastKey;

	QList<CKey>	Keys;
	bool	modified;
	int		baseWidth;
	int		baseHeight;

        bool    isShift;


	KEYBMAPParser *handler;
		
	Ckeyb(CPObject *parent = 0,QString map = "")	: CPObject(parent)								//[constructor]
	{
		for(int i=0;i<MAX_KO;i++) pc1350KeyStatus[i]=0;
		for(int j=0;j<200;j++) keym[j]=0;
		access		= 0;							//ko port access?(0:none, 1:access)
		KStrobe		= 0;
		IA_PORT		= 0;
        Kon			= false;
		fn_KeyMap	= map;
		modified = false;
		handler = new KEYBMAPParser(this);
	};
	
	virtual ~Ckeyb(){};
					
protected:
	BYTE pc1350KeyStatus[MAX_KO];			//Ko0-11
	BYTE IA_PORT;

};




///////////////////////////////////////////////////////////////////
// Keyboard emulation Windows ////////////////////////////////////
////////////////////////////////////////////////////////////////
class Ckeyb_pc1350:public Ckeyb{
public:

	Ckeyb_pc1350(CPObject *parent = 0)	: Ckeyb(parent)
	{
		scandef = scandef_pc1350;
		fn_KeyMap	= "pc1350.map";
	}
	virtual ~Ckeyb_pc1350(){};

private:
};


class Ckeyb_pc1360:public Ckeyb{
public:

	Ckeyb_pc1360(CPObject *parent = 0)	: Ckeyb(parent)
	{
		scandef = scandef_pc1360;
		fn_KeyMap	= "pc1360.map";
	}
	virtual ~Ckeyb_pc1360(){};

private:
};

class Ckeyb_pc2500:public Ckeyb{
public:

    Ckeyb_pc2500(CPObject *parent = 0)	: Ckeyb(parent)
    {
        scandef = scandef_pc2500;
        fn_KeyMap	= "pc2500.map";
    }
    virtual ~Ckeyb_pc2500(){};

private:
};

class Ckeyb_pc1401:public Ckeyb{
public:

	Ckeyb_pc1401(CPObject *parent = 0)	: Ckeyb(parent)
	{
		scandef = scandef_pc1401;
		fn_KeyMap	= "pc1401.map";
	}
	virtual ~Ckeyb_pc1401(){};

private:
};


class Ckeyb_pc1403:public Ckeyb{
public:

	Ckeyb_pc1403(CPObject *parent = 0)	: Ckeyb(parent)
	{
		scandef = scandef_pc1403;
		fn_KeyMap	= "pc1403.map";
	}
	virtual ~Ckeyb_pc1403(){};

private:
};

class Ckeyb_pc1450:public Ckeyb{
public:

	Ckeyb_pc1450(CPObject *parent = 0)	: Ckeyb(parent)
	{
		scandef = scandef_pc1450;
		fn_KeyMap	= "pc1450.map";
	}
	virtual ~Ckeyb_pc1450(){};

private:
};

class Ckeyb_pc1250:public Ckeyb{
public:

	Ckeyb_pc1250(CPObject *parent = 0)	: Ckeyb(parent)
	{
		scandef = scandef_pc1250;
		fn_KeyMap	= "pc1250.map";
	}
	virtual ~Ckeyb_pc1250(){};

private:
};

class Ckeyb_pc1500:public Ckeyb{
public:

	Ckeyb_pc1500(CPObject *parent = 0)	: Ckeyb(parent)
	{
		scandef = scandef_pc1500;
		fn_KeyMap	= "pc1500.map";
	}
	virtual ~Ckeyb_pc1500(){};

private:
};

class Ckeyb_trspc2:public Ckeyb{
public:

	Ckeyb_trspc2(CPObject *parent = 0)	: Ckeyb(parent)
	{
		scandef = scandef_pc1500;
		fn_KeyMap	= "trspc2.map";
	}
	virtual ~Ckeyb_trspc2(){};

private:
};

class Ckeyb_ce150:public Ckeyb{
public:

	Ckeyb_ce150(CPObject *parent = 0)	: Ckeyb(parent)
	{
		scandef = 0;
		fn_KeyMap	= "ce150.map";
	}
	virtual ~Ckeyb_ce150(){};

private:
};

class Ckeyb_pc1600:public Ckeyb{
public:

    Ckeyb_pc1600(CPObject *parent = 0)	: Ckeyb(parent)
    {
        scandef = scandef_pc1600;
        fn_KeyMap	= "pc1600.map";
    }
    virtual ~Ckeyb_pc1600(){};

private:
};

class Ckeyb_ce1600p:public Ckeyb{
public:

    Ckeyb_ce1600p(CPObject *parent = 0)	: Ckeyb(parent)
    {
        scandef = 0;
        fn_KeyMap	= "ce1600p.map";
    }
    virtual ~Ckeyb_ce1600p(){};

private:
};

#endif
