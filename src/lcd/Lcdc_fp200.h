#ifndef LCDC_FP200_H
#define LCDC_FP200_H

#include <QtCore>
#include "Lcdc.h"

class Clcdc_fp200:public Clcdc{
public:
    void disp(void);				//display LCDC data to screen
    void disp_symb(void);
    const char*	GetClassName(){ return("Clcdc_fp200");}

    Clcdc_fp200(CPObject *parent = 0);
    virtual ~Clcdc_fp200()
    {						//[constructor]
    }

    quint8 mem_video[160][8];
    quint8 X,Y;
    bool updated;
};

#endif // LCDC_FP200_H
