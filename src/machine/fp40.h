#ifndef FP40_H
#define FP40_H

#include "printerctronics.h"

class DialogConsole;

class Cfp40:public CprinterCtronics{
Q_OBJECT
public:

    virtual bool run(void);
    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    //virtual void ExportByte(qint8);
    virtual void ComputeKey(void);
    virtual bool UpdateFinalImage(void);


    Cfp40(CPObject *parent = 0);
    virtual ~Cfp40();


    void Printer(quint8 data);
protected:

protected slots:


private:


};
#endif // FP40_H
