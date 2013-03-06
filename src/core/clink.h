#ifndef _CLINK_H_
#define _CLINK_H_

#include "pobject.h"
class Cconnector;

class CDirectLink:public QObject
{
public:
	
    CDirectLink(void);

    QList<Cconnector*> AConnList;
    QList<Cconnector*> BConnList;
    QList<bool>        closeList;
	
    QList<CPObject *> log_run;

    void addLink(Cconnector *A,Cconnector *B, bool close);
    void removeLink(int ind);

    void outConnector(Cconnector* search);
    void Output(CPObject* pPC);
    void updateConnectors(CPObject* pPC);
	
    CPObject * findObj(CPObject *);
    void findAllObj(CPObject *, QList<CPObject *> *, bool onlyclosed = true);
    bool isLinked(CPObject *);
    bool isLinked(Cconnector *);
    Cconnector * Linked(Cconnector * search);
	
	
    void clearlog(void) {
        log_run.clear();
    }
    void insertlog(CPObject * pPC){
        log_run.append(pPC);
	}
    bool inlogrun(CPObject * pPC){
        return log_run.contains(pPC);
    }

};


class CLink:public CPObject
{
public:

	CLink()//(int insize,int outsize)
	{
		Visible = false;
        direct = true;
		AConn = 0;
		BConn = 0;
    }

    bool	isVisible() { return Visible; }
    void	setVisible(bool val){ Visible = val; }

    virtual	bool run(void){ return true;}				// emulator main step

	Cconnector *AConn;
	Cconnector *BConn;	
    bool direct;
	
private:

	
	bool	Visible;
	
};






















#endif
