
#include "Lcdc_pc1211.h"

Clcdc_pc1211::Clcdc_pc1211(CPObject *parent)	: Clcdc(parent){						//[constructor]
    Color_Off.setRgb(
                        (int) (95*contrast),
                        (int) (107*contrast),
                        (int) (97*contrast));
}

void Clcdc_pc1211::disp()
{

//    if (((Cx07 *)pPC)->pT6834)
//        ((Cx07 *)pPC)->pT6834->RefreshVideo();
//    redraw = false;
}

