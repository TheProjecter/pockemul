#include <QPainter>
#include "common.h"
#include "pcxxxx.h"
#include "cpu.h"
#include "g850v.h"
#include "sed1560.h"
#include "Lcdc_g850.h"
#include "Lcdc_symb.h"

Clcdc_g850::Clcdc_g850(CPObject *parent )	: Clcdc(parent){						//[constructor]
    Color_Off.setRgb(
                        (int) (92*contrast),
                        (int) (120*contrast),
                        (int) (103*contrast));
}

static const struct {
    int x,y;
} g850_pos[15]={
    {7, 10},     // BUSY
    {60, 10},    // RUN
    {79, 10},    // PRO
    {35, 10},    // DBL
    {257 ,10},    // SMALL
    {238 ,10},    // KANA
    {202 ,0},    // HYP
    {200,10},    // CAPS
    {173,10},    // 2ndF
    {241,0},    // PRINT
    {300,10},    // E
    {181,0},    // RAD
    {176,0},    // G
    {168,0},    // DEG
    {314,9}    // BATT
};
/* busy  shift   small   de g rad   run  pro  reserve  def  i ii iii battery */
/* japanese? */


//      b7      b6      b5      b4      b3      b2      b1      b0
// S1   KBII                            S       x       CTRL    BATT    F3C7
// S2           RUN     PRO     RESERVE         RAD     G       DE      F64F
// S3   DEF     I       II      III     SML     x       SHIFT   BUSY    F64E

#define SYMB1_g850		(((Cg850v *)pPC)->pSED1560->info.imem[symbSL(0)])
#define SYMB2_g850		(((Cg850v *)pPC)->pHD61102_1->info.imem[symbSL(4)])
#define SYMB3_g850		(((Cg850v *)pPC)->pHD61102_1->info.imem[symbSL(5)])

void Clcdc_g850::disp_symb(void)
{
    /*
    disp_one_symb( BUSY,		COLOR(SYMB1_g850&1),	g850_pos[0].x,	g850_pos[0].y);
    disp_one_symb( RUN,         COLOR(SYMB1_g850&2),	g850_pos[1].x,	g850_pos[1].y);
    disp_one_symb( PRO,			COLOR(SYMB1_g850&4),	g850_pos[2].x,	g850_pos[2].y);
    disp_one_symb( DBL,         COLOR(SYMB1_g850&8),	g850_pos[3].x,	g850_pos[3].y);
    disp_one_symb( SMALL,		COLOR(SYMB2_g850&0x01),	g850_pos[4].x,	g850_pos[4].y);
    disp_one_symb( JAP, 		COLOR(SYMB2_g850&0x02),	g850_pos[5].x,	g850_pos[5].y);
    disp_one_symb( HYP,			COLOR(SYMB2_g850&0x04),	g850_pos[6].x,	g850_pos[6].y);
    disp_one_symb( CAPS,		COLOR(SYMB2_g850&0x08),	g850_pos[7].x,	g850_pos[7].y);
    disp_one_symb( SECF,		COLOR(SYMB2_g850&0x10),	g850_pos[8].x,	g850_pos[8].y);
    disp_one_symb( PRINTL,		COLOR(SYMB3_g850&0x01),	g850_pos[9].x,	g850_pos[9].y);
    disp_one_symb( E,			COLOR(SYMB3_g850&0x02),	g850_pos[10].x,	g850_pos[10].y);
    disp_one_symb( RAD,         COLOR(SYMB3_g850&0x04),	g850_pos[11].x,	g850_pos[11].y);
    disp_one_symb( G,       	COLOR(SYMB3_g850&0x08),	g850_pos[12].x,	g850_pos[12].y);
    disp_one_symb( DEG,         COLOR(SYMB3_g850&0x10),	g850_pos[13].x,	g850_pos[13].y);
    disp_one_symb( REV_BATT,    COLOR(SYMB3_g850&0x80),	g850_pos[14].x,	g850_pos[14].y);
*/
    Clcdc::disp_symb();
}
INLINE int Clcdc_g850::symbSL(int x)
{

    int l=(x+((Cg850v *)pPC)->pSED1560->info.displaySL/8);
    l = (l>=8) ? l-8 : l;
    return l*0x40+63;
}

INLINE int Clcdc_g850::computeSL(int ord)
{
    int y = ord;
    y -= ((Cg850v*)pPC)->pSED1560->info.displaySL;
    if (y < 0) y += 64;
    return y;
}

void Clcdc_g850::disp(void)
{

    Cg850v *g850 = (Cg850v*)pPC;

    BYTE b;

    Refresh = false;

    if (!ready) return;
    if (!g850->pSED1560) return;
    if (!g850->pSED1560->updated) return;

    g850->pSED1560->updated = false;

    Refresh = true;

//    disp_symb();

    QPainter painter(pPC->LcdImage);

    if (g850->pSED1560->info.on_off) {
        for (int i = 0 ; i < 166; i++)
        {
            for (int j = 0 ; j < 8 ; j++)
            {
            BYTE data = g850->pSED1560->info.imem[ (j * 0xa6) + i ];
                for (b=0; b<8;b++)
                {
                    //if (((data>>b)&0x01) && (pPC->pCPU->fp_log)) fprintf(pPC->pCPU->fp_log,"PSET [%i,%i]\n",i,j*8+b);
                    painter.setPen( ((data>>b)&0x01) ? Color_On : Color_Off );

                    int y = computeSL(j*8+b);
                    if ((y>=0)&&(y < 48)) painter.drawPoint( i, y );
                }
            }
        }
    }


    redraw = 0;
    painter.end();
}


