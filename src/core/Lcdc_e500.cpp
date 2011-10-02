#include <QPainter>
#include "common.h"
#include "pcxxxx.h"
#include "cpu.h"
#include "e500.h"
#include "Lcdc_e500.h"
#include "Lcdc_symb.h"

Clcdc_e500::Clcdc_e500(CPObject *parent )	: Clcdc(parent){						//[constructor]
    Color_Off.setRgb(
                        (int) (126*contrast),
                        (int) (151*contrast),
                        (int) (124*contrast));
}

static const struct {
    int x,y;
} e500_pos[16]={
    {0, 0},     // BUSY
    {18, 0},    // SHIFT
    {40, 0},    // JAP
    {50, 0},    // SMALL
    {75 ,0},    // DE
    {83 ,0},    // G
    {88 ,0},    // RAD
    {160,0},    // RUN
    {175,0},    // PRO
    {190,0},    // RESERVE
    {220,0},    // DEF
    {240,0},    // ROMEAN_I
    {245,0},    // ROMEAN_II
    {255,0},    // ROMEAN_III
    {265,0},    // CTRL
    {290,0}     // REV_BAT
};
/* busy  shift   small   de g rad   run  pro  reserve  def  i ii iii battery */
/* japanese? */


//      b7      b6      b5      b4      b3      b2      b1      b0
// S1   KBII                            S       x       CTRL    BATT    F3C7
// S2           RUN     PRO     RESERVE         RAD     G       DE      F64F
// S3   DEF     I       II      III     SML     x       SHIFT   BUSY    F64E

#define SYMB1_1600		((Ce500 *)pPC)->pHD61102_2->info.imem[symbSL(4)]
#define SYMB2_1600		((Ce500 *)pPC)->pHD61102_2->info.imem[symbSL(6)]
#define SYMB3_1600		((Ce500 *)pPC)->pHD61102_2->info.imem[symbSL(7)]

void Clcdc_e500::disp_symb(void)
{
    disp_one_symb( BUSY,		COLOR(SYMB3_1600&1),	e500_pos[0].x,	e500_pos[0].y);
    disp_one_symb( SHIFT,		COLOR(SYMB3_1600&2),	e500_pos[1].x,	e500_pos[1].y);
    disp_one_symb( JAP,			COLOR(SYMB3_1600&4),	e500_pos[2].x,	e500_pos[2].y);
    disp_one_symb( SMALL,		COLOR(SYMB3_1600&8),	e500_pos[3].x,	e500_pos[3].y);
    disp_one_symb( DE,			COLOR(SYMB2_1600&0x01),	e500_pos[4].x,	e500_pos[4].y);
    disp_one_symb( G,			COLOR(SYMB2_1600&0x02),	e500_pos[5].x,	e500_pos[5].y);
    disp_one_symb( RAD,			COLOR(SYMB2_1600&0x04),	e500_pos[6].x,	e500_pos[6].y);
    disp_one_symb( RUN,			COLOR(SYMB2_1600&0x40),	e500_pos[7].x,	e500_pos[7].y);
    disp_one_symb( PRO,			COLOR(SYMB2_1600&0x20),	e500_pos[8].x,	e500_pos[8].y);
    disp_one_symb( RESERVE,		COLOR(SYMB2_1600&0x10),	e500_pos[9].x,	e500_pos[9].y);
    disp_one_symb( DEF,			COLOR(SYMB3_1600&0x80),	e500_pos[10].x,	e500_pos[10].y);
    disp_one_symb( ROMEAN_I,	COLOR(SYMB3_1600&0x40),	e500_pos[11].x,	e500_pos[11].y);
    disp_one_symb( ROMEAN_II,	COLOR(SYMB3_1600&0x20),	e500_pos[12].x,	e500_pos[12].y);
    disp_one_symb( ROMEAN_III,	COLOR(SYMB3_1600&0x10),	e500_pos[13].x,	e500_pos[13].y);
    disp_one_symb( CTRL,        COLOR(SYMB1_1600&0x02),	e500_pos[14].x,	e500_pos[14].y);
    disp_one_symb( REV_BATT,    COLOR(0),               e500_pos[15].x,	e500_pos[15].y);

    Refresh = true;

    Clcdc::disp_symb();
}
INLINE int Clcdc_e500::symbSL(int x)
{
    Ce500 *p = (Ce500 *)pPC;
    int l=(x+((Ce500 *)pPC)->pHD61102_2->info.displaySL/8);
    l = (l>=8) ? l-8 : l;
    return l*0x40+63;
}

INLINE int Clcdc_e500::computeSL(CHD61102* pCtrl,int ord)
{
    int y = ord;
    y -= pCtrl->info.displaySL;
    if (y < 0) y += 64;
    return y;
}

void Clcdc_e500::disp(void)
{

    BYTE b,data,x;
    int ind;
    WORD adr;


    if (!ready) return;
    if (!((Ce500 *)pPC)->pHD61102_1 || !((Ce500 *)pPC)->pHD61102_2) return;
    Refresh = true;

    disp_symb();

    QPainter painter(pPC->LcdImage);

    if (((Ce500 *)pPC)->pHD61102_2->info.on_off) {
        for (int i = 0 ; i < 64; i++)
        {
            for (int j = 0 ; j < 8 ; j++)
            {
            BYTE data = ((Ce500 *)pPC)->pHD61102_2->info.imem[ (j * 0x40) + i ];
                for (b=0; b<8;b++)
                {
                    if (((data>>b)&0x01) && (pPC->pCPU->fp_log)) fprintf(pPC->pCPU->fp_log,"PSET [%i,%i]\n",i,j*8+b);
                    painter.setPen( ((data>>b)&0x01) ? Color_On : Color_Off );

                    int y = computeSL(((Ce500 *)pPC)->pHD61102_2,j*8+b);
                    if ((y>=0)&&(y < 32)) painter.drawPoint( i, y );
                }
            }
        }
    }

    if (((Ce500 *)pPC)->pHD61102_1->info.on_off) {
        for (int i = 0 ; i < 56; i++)
        {
            for (int j = 0 ; j < 8 ; j++)
            {
                BYTE data = ((Ce500 *)pPC)->pHD61102_1->info.imem[ (j * 0x40) + i ];
                for (b=0; b<8;b++)
                {
                    if (((data>>b)&0x01) && (pPC->pCPU->fp_log)) fprintf(pPC->pCPU->fp_log,"PSET [%i,%i]\n",64+i,j*8+b);
                    painter.setPen( ((data>>b)&0x01) ? Color_On : Color_Off );
    //                painter.drawPoint( 64+i, computeSL(j*8+b));
                    int y = computeSL(((Ce500 *)pPC)->pHD61102_1,j*8+b);
                     if ((y>=0)&&(y < 32)) painter.drawPoint( 64+i, y );
                }
            }
        }
    }

    if (((Ce500 *)pPC)->pHD61102_1->info.on_off) {

        for (int i = 0 ; i < 56; i++)
        {
            for (int j = 0 ; j < 8 ; j++)
            {
                BYTE data = ((Ce500 *)pPC)->pHD61102_1->info.imem[ (j * 0x40) + i ];
                for (b=0; b<8;b++)
                {
    //                if (((data>>b)&0x01) && (pPC->pCPU->fp_log)) fprintf(pPC->pCPU->fp_log,"PSET [%i,%i]\n",128+i,(j-4)*8+b);
                    painter.setPen( ((data>>b)&0x01) ? Color_On : Color_Off );
                    int y = computeSL(((Ce500 *)pPC)->pHD61102_1,j*8+b-32);
                    if ((y>=0)&&(y < 32)) painter.drawPoint( 175-i, y );

                }
            }
        }
    }


    if (((Ce500 *)pPC)->pHD61102_2->info.on_off) {

        for (int i = 0 ; i < 64; i++)
        {
            for (int j = 0 ; j < 8 ; j++)
            {
                BYTE data = ((Ce500 *)pPC)->pHD61102_2->info.imem[ (j * 0x40) + i ];
                for (b=0; b<8;b++)
                {
    //                if (((data>>b)&0x01) && (pPC->pCPU->fp_log)) fprintf(pPC->pCPU->fp_log,"PSET [%i,%i]\n",128+i,(j-4)*8+b);
                    painter.setPen( ((data>>b)&0x01) ? Color_On : Color_Off );
                    int y = computeSL(((Ce500 *)pPC)->pHD61102_2,j*8+b-32);
                    if ((y>=0)&&(y < 32)) painter.drawPoint( 239-i, y );

                }
            }
        }
    }

    redraw = 0;
    painter.end();
}

