/*** PC-XXXX Emulator [CONNECT.H] ***********/
/* Connector emulation class				*/
/********************************************/


#ifndef _CONNECT_H
#define _CONNECT_H

#include <QObject>
#include <QPoint>

#define GET_PIN(n)		(pCONNECTOR->Get_pin(n))
#define SET_PIN(n,v)	(pCONNECTOR->Set_pin(n,v))

class CPObject;



class Cconnector:public QObject
{
    Q_OBJECT
public:
	CPObject *Parent;
	
    enum ConnectorType
    {
        Sharp_15,
        Sharp_11,
        Sharp_60,
        Jack,
        Optical,
        Canon_15,
        Canon_9,
        Canon_40,
        Canon_Din,
        Casio_30,
        Centronics_36,
        DB_25,
        DIN_8
    };

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

    Q_INVOKABLE ConnectorType	getType();
    Q_INVOKABLE void	setType(ConnectorType);

	void	ConnectTo(Cconnector *);

    void    setSnap(QPoint p) {snap = p;}
    QPoint  getSnap(void) {return snap;}
    int    getNbpins(void) {return nbpins;}
    void   setNbpins(int nb) { nbpins = nb;}

    static bool arePluggable(Cconnector *a,Cconnector *b);
	
    Cconnector(CPObject *parent , QString desc, bool newGender = false);
    Cconnector(CPObject *parent , int nbpins, int Id, ConnectorType type, QString desc,bool newGender = false,QPoint snap=QPoint(0,0));

    virtual ~Cconnector(){}

private:
	bool	gender;		// Male = true   Female = false  :-)
    ConnectorType Type;
    int		nbpins;
    qint64	values;
    QPoint  snap;
};

#endif		// _CONNECT_H
