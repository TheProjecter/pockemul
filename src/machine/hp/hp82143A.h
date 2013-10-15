#ifndef HP82143A_H
#define HP82143A_H


#include "printerctronics.h"

class DialogConsole;

class Chp82143A:public CprinterCtronics{
Q_OBJECT
public:

    virtual bool run(void);
    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    //virtual void ExportByte(qint8);
    virtual void ComputeKey(void);
    virtual bool UpdateFinalImage(void);


    Chp82143A(CPObject *parent = 0);
    virtual ~Chp82143A();


    virtual void Printer(quint8 data);

protected:

protected slots:


private:


};

#endif // HP82143A_H
