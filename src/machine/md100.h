#ifndef MD100_H
#define MD100_H


#include <QTime>
#include <QSound>
#include <QDir>


#include "pobject.h"



class Cmd100:public CPObject{
    Q_OBJECT
public:

    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    virtual	bool run(void);					//check access

    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);

    virtual bool UpdateFinalImage(void);


    Cconnector	*pCONNECTOR;		qint64 pCONNECTOR_value;
    Cconnector	*pCONNECTOR_Ext;	qint64 pCONNECTOR_Ext_value;




    Cmd100(CPObject *parent = 0);

    ~Cmd100()
    {
    }

public:


protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
    void definePath(void);

private:
    QDir    directory;
};



#endif // MD100_H
