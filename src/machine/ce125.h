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
        BackGroundFname	= ":/EXT/ext/ce-125tape.png";
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
    virtual void resizeEvent ( QResizeEvent * );

    Cce125(CPObject *parent = 0);
	
private:
	Cce125tape* pTAPE;
};

class Cmp2200:public Cce125
{
public:

    Cmp2200(CPObject *parent = 0);

};



#endif
