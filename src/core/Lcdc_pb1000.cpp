#include <QPainter>
#include "common.h"
#include "pcxxxx.h"
#include "pb1000.h"
#include "hd44352.h"
#include "Lcdc_pb1000.h"
#include "Lcdc_symb.h"

#define BIT(x,n) (((x)>>(n))&1)

static const struct {
    int x,y;
} pb1000_pos[16]={
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

#define SYMB1_1600		((Cpb1000 *)pPC)->pHD61102_2->info.imem[symbSL(4)]
#define SYMB2_1600		((Cpb1000 *)pPC)->pHD61102_2->info.imem[symbSL(6)]
#define SYMB3_1600		((Cpb1000 *)pPC)->pHD61102_2->info.imem[symbSL(7)]

void Clcdc_pb1000::disp_symb(void)
{



    Refresh = true;

    Clcdc::disp_symb();
}



void Clcdc_pb1000::disp(void)
{
    if (!ready) return;
    if (!((Cpb1000 *)pPC)->pHD44352 ) return;
    Refresh = true;
    disp_symb();

    QPainter painter(pPC->LcdImage);

    UINT8 cw = info.m_char_width;

    //        bitmap_fill(&bitmap, &cliprect, 0);

    if (info.m_control_lines&0x80 && info.m_lcd_on)
    {
        for (int a=0; a<2; a++)
            for (int py=0; py<4; py++)
                for (int px=0; px<16; px++)
                    if (BIT(info.m_cursor_status, 4) && px == info.m_cursor_x && py == info.m_cursor_y && a == info.m_cursor_lcd)
                    {
                        //draw the cursor
                        for (int c=0; c<cw; c++)
                        {
                            UINT8 d = CHD44352::compute_newval((info.m_cursor_status>>5) & 0x07, info.m_video_ram[a][py*16*cw + px*cw + c + info.m_scroll * 48], info.m_cursor[c]);
                            for (int b=0; b<8; b++)
                            {
                                painter.setPen((BIT(d, 7-b)) ? Color_On : Color_Off );
                                painter.drawPoint( a*cw*16 + px*cw + c, py*8 + b );
                                //                                    *BITMAP_ADDR16(&bitmap, py*8 + b, a*cw*16 + px*cw + c) = BIT(d, 7-b);
                            }
                        }
                    }
                    else
                    {
                        for (int c=0; c<cw; c++)
                        {
                            UINT8 d = info.m_video_ram[a][py*16*cw + px*cw + c + info.m_scroll * 48];
                            for (int b=0; b<8; b++)
                            {
                                painter.setPen((BIT(d, 7-b)) ? Color_On : Color_Off );
                                painter.drawPoint( a*cw*16 + px*cw + c, py*8 + b );
                                //                                    *BITMAP_ADDR16(&bitmap, py*8 + b, a*cw*16 + px*cw + c) = BIT(d, 7-b);
                            }
                        }
                    }
    }

    redraw = 0;
    painter.end();
}

