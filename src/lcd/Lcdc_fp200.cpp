#include <QPainter>
#include "common.h"
#include "pcxxxx.h"

#include "Lcdc_fp200.h"
#include "Lcdc_symb.h"
#include "Log.h"




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
    quint8 offset = (side == 1 ? 0 : 10);
    if (Status == 1) {

        AffCar(X+offset,Y,val);
    }
//    mem_video[Y + offset][X] = val;
    //Y++;
    updated = true;
}

quint8 Clcdc_fp200::Read(quint8 side)
{
    quint8 offset = (side == 1 ? 0 : 80);
    quint8 val = mem_video[X + offset][Y];
    //Y++;
    return val;
}

void Clcdc_fp200::disp_symb(void)
{
    Clcdc::disp_symb();
}

bool Clcdc_fp200::init()
{
    Clcdc::init();

    QFile file;
    file.setFileName(":/fp200/chr.bin");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    in.readRawData ((char *) &charset,0x800 );

    return true;
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
        for (int i = 0 ; i < 64; i++)
        {
            for (int li = 0 ; li < 20 ; li++)
            {
            quint8 data = mem_video[li][ i ];
                for (b=0; b<8;b++)
                {
                    //if (((data>>b)&0x01) && (pPC->pCPU->fp_log)) fprintf(pPC->pCPU->fp_log,"PSET [%i,%i]\n",i,j*8+b);
                    painter.setPen( ((data>>b)&0x01) ? Color_On : Color_Off );
                    painter.drawPoint( li*8+b,i );
                }
            }
        }




    redraw = 0;
    painter.end();
}


void Clcdc_fp200::AffCar(UINT8 x, UINT8 y, UINT8 Car)
{


    if (Car>0) AddLog (LOG_CONSOLE,tr("Draw char (%1) at %2,%3\n").arg(Car,2,16,QChar('0')).arg(x).arg(y));

    for (int P_y=0;P_y<8;P_y++)
    {
        UINT8 c = (Car>>4)| (Car<<4);
        quint8 b = FP200_CarDef[Car][P_y] ;
//        quint8 b = charset[c*8+P_y] ;
        b = (b * 0x0202020202ULL & 0x010884422010ULL) % 1023;       // reverse bits order

        mem_video[x][y+P_y] = b ;
    }

    updated = true;
}
