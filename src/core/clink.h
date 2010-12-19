#ifndef _CLINK_H_
#define _CLINK_H_

#include "pobject.h"
#include "Connect.h"

class CDirectLink:public QObject
{
public:
	
    CDirectLink(void);

    QList<Cconnector*> AConnList;
    QList<Cconnector*> BConnList;
	
    QList<CPObject *> log_run;

	
    void outConnector(Cconnector* search);
    void Output(CPObject* pPC);
	
    CPObject * findObj(CPObject *);
    void findAllObj(CPObject *, QList<CPObject *> *);
    bool isLinked(CPObject *);
    bool isLinked(Cconnector *);
	
	
    void clearlog(void) {
        log_run.clear();
        //indx_log_object_run=0;
    }
    void insertlog(CPObject * pPC){
        log_run.append(pPC);
        //log_run[indx_log_object_run] = pPC;
        //indx_log_object_run++;
	}
    bool inlogrun(CPObject * pPC){
        return log_run.contains(pPC);
//		for (int i=0; i<indx_log_object_run;i++)
//		{
//			if (log_run[indx_log_object_run] == pPC) return true;
//		}
//		return false;
	}
};


class CLink:public CPObject
{
public:

	CLink()//(int insize,int outsize)
	{
		Visible = false;
		AConn = 0;
		BConn = 0;
	};

	bool	isVisible() { return Visible; };
	void	setVisible(bool val){ Visible = val; };

	virtual	bool run(void){ return true;};					// emulator main step

	Cconnector *AConn;
	Cconnector *BConn;	
	
private:

	
	bool	Visible;
	
};


class CLink11to11:public CLink
{
public:
	CLink11to11()
	{
        AConn = new Cconnector(this,11,"Connector 11 pins",false);//Cconnector11(this);
        BConn = new Cconnector(this,11,"Connector 11 pins",false);//Cconnector11(this);
	}

	bool run(void);

};




















#endif
