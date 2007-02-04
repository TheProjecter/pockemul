#ifndef _CE125_H_
#define _CE125_H_

#include "Ce126.h"
#include "ce152.h"

extern TransMap KeyMapce125tape[];
extern int KeyMapce125tapeLenght;

class Cce125tape:public Cce152
{
public:
	virtual void ComputeKey(void);
	
	Cce125tape(CPObject *parent = 0)	: Cce152(parent)
	{									//[constructor]
		BackGroundFname	= ":/EXT/ext/ce-125tape.jpg";
		Pc_DX	= 249;
		Pc_DY	= 299;
		KeyMap		= KeyMapce125tape;
		KeyMapLenght= KeyMapce125tapeLenght;
		pKEYB		= new Ckeyb(this,"ce125tape.map");
	};

};

class Cce125:public Cce126
{
public:
	virtual bool init(void);				//initialize
	virtual bool exit(void);				//end
	virtual	bool run(void);					//check access

	Cce125()
	{								//[constructor]
		BackGroundFname	= ":/EXT/ext/ce-125.jpg";
		Pc_DX	= 731;
		Pc_DY	= 532;
		SnapPts = QPoint(247,280);
		setPaperPos(QPoint(377,0));

		stackBehind = true;
	};
	
private:
	Cce125tape* pTAPE;
};

class C263591:public Cce126
{
public:

	C263591()
	{								//[constructor]
		BackGroundFname	= ":/EXT/ext/26-3591.jpg";
		Pc_DX	= 854;
		Pc_DY	= 349;
		SnapPts = QPoint(373,0);
		setPaperPos(QPoint(78,0));

		stackBehind = true;
	};
	
private:
};

#endif
