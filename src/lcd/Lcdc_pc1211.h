#ifndef LCDC_PC1211_H
#define LCDC_PC1211_H

#include "Lcdc.h"

class Clcdc_pc1211:public Clcdc{
public:
    void disp(void);				//display LCDC data to screen
    void disp_symb(void){}
    const char*	GetClassName(){ return("Clcdc_pc1211");}

    Clcdc_pc1211(CPObject *parent = 0);
    virtual ~Clcdc_pc1211()
    {						//[constructor]
    }


};

#endif // LCDC_PC1211_H
