/*** PC-XXXX Emulator [CONNECT.H] ***********/
/* Connector emulation class				*/
/********************************************/


#ifndef _CONNECT_H
#define _CONNECT_H

#define GET_PIN(n)		(pCONNECTOR->Get_pin(n))
#define SET_PIN(n,v)	(pCONNECTOR->Set_pin(n,v))

#include "pobject.h"



class Cconnector:public QObject
{
    Q_OBJECT
public:
	CPObject *Parent;
	

	bool	refresh;

        int Id;
	
	QString	Desc;
	
    virtual bool init(void){return true;}
    virtual bool exit(void){return true;}
    Q_INVOKABLE qint64	Get_values(void);
    Q_INVOKABLE void	Set_values(qint64 val);
    Q_INVOKABLE bool	Get_pin(qint8);
    Q_INVOKABLE void	Set_pin(qint8 , bool);
	void	Dump_pin(void);
	char	dump[100];

    Q_INVOKABLE bool	getGender();
    Q_INVOKABLE void	setGender(bool);

	void	ConnectTo(Cconnector *);

    void    setSnap(QPoint p) {snap = p;}
    QPoint  getSnap(void) {return snap;}
    int    getNbpins(void) {return nbpins;}

    static bool arePluggable(Cconnector *a,Cconnector *b) {
        return ( (a->nbpins==b->nbpins) && (a->gender != b->gender) );
    }
	
    Cconnector(CPObject *parent , QString desc, bool newGender = false)
	{
		Parent	= parent;
		Desc = desc;
		gender = newGender;
		values = 0;
    }
    Cconnector(CPObject *parent , int nbpins, int Id, QString desc,bool newGender = false,QPoint snap=QPoint(0,0));

    virtual ~Cconnector(){}

private:
	bool	gender;		// Male = true   Female = false  :-)

    int		nbpins;
    qint64	values;
    QPoint  snap;
};

#endif		// _CONNECT_H
