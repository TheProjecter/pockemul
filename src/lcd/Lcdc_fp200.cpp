#include <QPainter>
#include "common.h"
#include "pcxxxx.h"

#include "Lcdc_fp200.h"
#include "Lcdc_symb.h"

Clcdc_fp200::Clcdc_fp200(CPObject *parent )	: Clcdc(parent){						//[constructor]
    Color_Off.setRgb(
                        (int) (92*contrast),
                        (int) (120*contrast),
                        (int) (103*contrast));
    updated = true;
    X = Y = 0;
    Status = 0;
    memset((char*)mem_video,0,sizeof(mem_video));
}

void Clcdc_fp200::Write(quint8 side, quint8 val) {
    quint8 offset = (side == 1 ? 0 : 80);
    mem_video[Y + offset][X] = val;
    //Y++;
    updated = true;
}

quint8 Clcdc_fp200::Read(quint8 side)
{
    quint8 offset = (side == 1 ? 0 : 80);
    quint8 val = mem_video[Y + offset][X];
    //Y++;
    return val;
}

void Clcdc_fp200::disp_symb(void)
{
    Clcdc::disp_symb();
}

void Clcdc_fp200::disp(void)
{

    BYTE b;

    Refresh = false;

    if (!ready) return;
    if (!updated) return;

    updated = false;

    Refresh = true;

    QPainter painter(pPC->LcdImage);

//    if (((Ce500 *)pPC)->pHD61102_2->info.on_off) {
        for (int i = 0 ; i < 160; i++)
        {
            for (int li = 0 ; li < 8 ; li++)
            {
            quint8 data = mem_video[i][ li ];
                for (b=0; b<8;b++)
                {
                    //if (((data>>b)&0x01) && (pPC->pCPU->fp_log)) fprintf(pPC->pCPU->fp_log,"PSET [%i,%i]\n",i,j*8+b);
                    painter.setPen( ((data>>b)&0x01) ? Color_On : Color_Off );
                    painter.drawPoint( i, li*8+b );
                }
            }
        }




    redraw = 0;
    painter.end();
}


