/*** PC-XXXX Emulator [CONNECT.H] ***********/
/* Connector emulation class				*/
/********************************************/


#ifndef _CONNECT_H
#define _CONNECT_H
#include "pobject.h"

class Cconnector:public QObject
{
public:
	CPObject *Parent;
	
	int		nbpins;
	bool	refresh;
	bool	*pins;
	
	QString	Desc;
	
	virtual bool init(void){return true;};
	virtual bool exit(void){return true;};
	qint64	Get_values(void);
	bool	Get_pin(qint8);
	void	Set_pin(qint8 , bool);
	void	Dump_pin(void);
	char	dump[100];

	bool	getGender();
	void	setGender(bool);

	void	ConnectTo(Cconnector *);
	
	Cconnector(CPObject *parent , QString desc, bool newGender = false)
	{
		Parent	= parent;
		Desc = desc;
		gender = newGender;
	};
	Cconnector(CPObject *parent , int nbpins, QString desc,bool newGender);
	virtual ~Cconnector(){};

private:
	bool	gender;		// Male = true   Female = false  :-)
};

class Cconnector11:public Cconnector{
public:
	bool	init(void);
	bool	exit(void);


	Cconnector11(CPObject *parent , bool newGender = false , QString desc = "Connector 11 pins"):Cconnector(parent,desc,newGender){
		nbpins = 11;
		init();
	}

private:

};

class Cconnector15:public Cconnector{
public:
	bool	init(void);
	bool	exit(void);


	Cconnector15(CPObject *parent , bool newGender = false,QString desc = "Connector 15 pins"):Cconnector(parent,desc,newGender){
		nbpins = 15;
		init();

	}

private:

};

class Cconnector60:public Cconnector{
public:
	bool	init(void);
	bool	exit(void);


	Cconnector60(CPObject *parent , bool newGender = false,QString desc = "Connector 60 pins"):Cconnector(parent,desc,newGender){
		nbpins = 60;
		init();

	}

private:

};
#endif		// _CONNECT_H
