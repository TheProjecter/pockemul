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
	

	bool	refresh;


	
	QString	Desc;
	
	virtual bool init(void){return true;};
	virtual bool exit(void){return true;};
	qint64	Get_values(void);
    void	Set_values(qint64 val);
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
		values = 0;
	};
	Cconnector(CPObject *parent , int nbpins, QString desc,bool newGender);
	virtual ~Cconnector(){};

private:
	bool	gender;		// Male = true   Female = false  :-)

    int		nbpins;
    qint64	values;
};

#endif		// _CONNECT_H
