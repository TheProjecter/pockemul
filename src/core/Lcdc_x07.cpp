#include "Lcdc_x07.h"
#include "cx07.h"


Clcdc_x07::Clcdc_x07(CPObject *parent)	: Clcdc(parent){						//[constructor]
    Color_Off.setRgb(
                        (int) (95*contrast),
                        (int) (119*contrast),
                (int) (193*contrast));
}

void Clcdc_x07::disp()
{
    Refresh = true;
    ((Cx07 *)pPC)->RefreshVideo();
}

