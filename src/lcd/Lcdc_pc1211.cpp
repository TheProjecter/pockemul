#include <QPainter>
#include <QDebug>

#include "Lcdc_pc1211.h"
#include "Lcdc_symb.h"
#include "pc1211.h"
#include "tinybasic/tinybasic.h"
#include "Inter.h"

quint8 pc1211_carDef[0x500] = {
    /*0000:*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0010:*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x79, 0x86, 0x1E, 0x79,
    /*0020:*/ 0x87, 0x3C, 0x79, 0x85, 0xCD, 0x79, 0x00, 0xD5, 0x79, 0x8B, 0x3A, 0x79, 0xE3, 0x01, 0x79, 0x89,
    /*0030:*/ 0x92, 0x79, 0x1C, 0xBE, 0x79, 0xD5, 0x9F, 0x79, 0xAA, 0x39, 0x79, 0xAC, 0x41, 0x79, 0x15, 0xEC,
    /*0040:*/ 0x79, 0x88, 0x91, 0x79, 0x88, 0x8D, 0x79, 0x1B, 0x8C, 0x79, 0x1B, 0x62, 0x79, 0xBE, 0x52, 0x79,
    /*0050:*/ 0x88, 0xA1, 0x79, 0xFA, 0xCB, 0x79, 0xA3, 0xCC, 0x79, 0xA3, 0xD0, 0x79, 0xA3, 0xD4, 0x79, 0xA3,
    /*0060:*/ 0xDD, 0x79, 0xA3, 0xE1, 0x79, 0x91, 0x49, 0x79, 0x90, 0xA4, 0x79, 0xA4, 0xC9, 0x79, 0x05, 0xC0,
    /*0070:*/ 0x79, 0x05, 0xC2, 0x79, 0x06, 0x22, 0x79, 0x06, 0x24, 0x79, 0x06, 0x5A, 0x79, 0x06, 0x5C, 0x79,
    /*0080:*/ 0xEE, 0x95, 0x79, 0xEE, 0xDF, 0x79, 0xEE, 0xD6, 0x79, 0x8E, 0x34, 0x79, 0x8E, 0xBA, 0x79, 0x8E,
    /*0090:*/ 0x68, 0x79, 0xDE, 0x98, 0x79, 0xD5, 0x34, 0x79, 0xDC, 0x28, 0x79, 0x8F, 0xD0, 0x79, 0x8E, 0xEF,
    /*00A0:*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x03, 0x04, 0x03, 0x00, 0x22,
    /*00B0:*/ 0x7F, 0x22, 0x7F, 0x22, 0x12, 0x2A, 0x7F, 0x2A, 0x24, 0x62, 0x64, 0x08, 0x13, 0x23, 0x50, 0x22,
    /*00C0:*/ 0x55, 0x49, 0x36, 0x00, 0x00, 0x07, 0x0B, 0x00, 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x1C, 0x22,
    /*00D0:*/ 0x41, 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, 0x00, 0x38, 0x58,
    /*00E0:*/ 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x60, 0x60, 0x00, 0x02, 0x04, 0x08, 0x10, 0x20,
    /*00F0:*/ 0x3E, 0x45, 0x49, 0x51, 0x3E, 0x00, 0x40, 0x7F, 0x42, 0x00, 0x46, 0x49, 0x51, 0x61, 0x42, 0x31,
    /*0100:*/ 0x4B, 0x4D, 0x49, 0x41, 0x10, 0x7F, 0x12, 0x14, 0x18, 0x39, 0x45, 0x45, 0x45, 0x27, 0x30, 0x49,
    /*0110:*/ 0x49, 0x4A, 0x3C, 0x03, 0x05, 0x79, 0x01, 0x01, 0x36, 0x49, 0x49, 0x49, 0x36, 0x1E, 0x29, 0x49,
    /*0120:*/ 0x49, 0x06, 0x00, 0x00, 0x36, 0x36, 0x00, 0x00, 0x00, 0x3B, 0x5B, 0x00, 0x00, 0x41, 0x22, 0x14,
    /*0130:*/ 0x08, 0x14, 0x14, 0x14, 0x14, 0x14, 0x08, 0x14, 0x22, 0x41, 0x00, 0x06, 0x09, 0x51, 0x01, 0x02,
    /*0140:*/ 0x3E, 0x41, 0x79, 0x49, 0x32, 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x36, 0x49, 0x49, 0x7F, 0x41, 0x22,
    /*0150:*/ 0x41, 0x41, 0x41, 0x3E, 0x3E, 0x41, 0x41, 0x7F, 0x41, 0x41, 0x49, 0x49, 0x49, 0x7F, 0x01, 0x09,
    /*0160:*/ 0x09, 0x09, 0x7F, 0x39, 0x49, 0x41, 0x41, 0x3E, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x41, 0x7F,
    /*0170:*/ 0x41, 0x00, 0x01, 0x3F, 0x41, 0x40, 0x20, 0x41, 0x22, 0x14, 0x08, 0x7F, 0x40, 0x40, 0x40, 0x40,
    /*0180:*/ 0x7F, 0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x7F, 0x10, 0x08, 0x04, 0x7F, 0x3E, 0x41, 0x41, 0x41, 0x3E,
    /*0190:*/ 0x06, 0x09, 0x09, 0x09, 0x7F, 0x5E, 0x21, 0x51, 0x41, 0x3E, 0x46, 0x29, 0x19, 0x09, 0x7F, 0x32,
    /*01A0:*/ 0x49, 0x49, 0x49, 0x26, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x07, 0x18,
    /*01B0:*/ 0x60, 0x18, 0x07, 0x7F, 0x20, 0x18, 0x20, 0x7F, 0x63, 0x14, 0x08, 0x14, 0x63, 0x03, 0x04, 0x78,
    /*01C0:*/ 0x04, 0x03, 0x43, 0x45, 0x49, 0x51, 0x61, 0x00, 0x41, 0x41, 0x7F, 0x00, 0x15, 0x16, 0x7C, 0x16,
    /*01D0:*/ 0x15, 0x00, 0x7F, 0x41, 0x41, 0x00, 0x18, 0x04, 0x02, 0x04, 0x18, 0x40, 0x40, 0x40, 0x40, 0x40,
    /*01E0:*/ 0x00, 0x0B, 0x07, 0x00, 0x00, 0x40, 0x3C, 0x44, 0x44, 0x38, 0x38, 0x44, 0x44, 0x48, 0x7F, 0x20,
    /*01F0:*/ 0x44, 0x44, 0x44, 0x38, 0x7F, 0x48, 0x44, 0x44, 0x38, 0x18, 0x54, 0x54, 0x54, 0x38, 0x02, 0x09,
    /*0200:*/ 0x7E, 0x08, 0x00, 0x00, 0x3C, 0x54, 0x54, 0x08, 0x00, 0x78, 0x04, 0x04, 0x7F, 0x00, 0x00, 0x7D,
    /*0210:*/ 0x00, 0x00, 0x00, 0x3D, 0x44, 0x40, 0x20, 0x00, 0x44, 0x28, 0x10, 0x7F, 0x00, 0x00, 0x7F, 0x00,
    /*0220:*/ 0x00, 0x78, 0x04, 0x78, 0x04, 0x78, 0x78, 0x04, 0x04, 0x08, 0x7C, 0x38, 0x44, 0x44, 0x44, 0x38,
    /*0230:*/ 0x08, 0x14, 0x14, 0x14, 0x7C, 0x40, 0x7C, 0x14, 0x24, 0x18, 0x00, 0x04, 0x04, 0x08, 0x7C, 0x20,
    /*0240:*/ 0x54, 0x54, 0x54, 0x48, 0x00, 0x20, 0x44, 0x3E, 0x04, 0x7C, 0x20, 0x40, 0x40, 0x3C, 0x1C, 0x20,
    /*0250:*/ 0x40, 0x20, 0x1C, 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x44, 0x48, 0x7C, 0x24, 0x44, 0x00, 0x3C, 0x50,
    /*0260:*/ 0x50, 0x0C, 0x44, 0x4C, 0x54, 0x64, 0x44, 0x00, 0x41, 0x77, 0x08, 0x00, 0x00, 0x00, 0x77, 0x00,
    /*0270:*/ 0x00, 0x00, 0x08, 0x77, 0x41, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*0280:*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x50, 0x70, 0x01, 0x01, 0x01, 0x07, 0x00, 0x00,
    /*0290:*/ 0x70, 0x40, 0x40, 0x40, 0x00, 0x00, 0x40, 0x20, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x1E, 0x2A,
    /*02A0:*/ 0x4A, 0x4A, 0x0A, 0x0C, 0x14, 0x3C, 0x44, 0x04, 0x00, 0x04, 0x78, 0x10, 0x20, 0x18, 0x28, 0x4C,
    /*02B0:*/ 0x08, 0x18, 0x48, 0x48, 0x78, 0x48, 0x48, 0x08, 0x7C, 0x18, 0x28, 0x48, 0x18, 0x28, 0x08, 0x7C,
    /*02C0:*/ 0x08, 0x40, 0x78, 0x48, 0x48, 0x40, 0x00, 0x7C, 0x54, 0x54, 0x54, 0x38, 0x40, 0x58, 0x00, 0x18,
    /*02D0:*/ 0x08, 0x08, 0x08, 0x08, 0x08, 0x07, 0x09, 0x3D, 0x41, 0x01, 0x01, 0x02, 0x7C, 0x08, 0x10, 0x1E,
    /*02E0:*/ 0x22, 0x43, 0x02, 0x06, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x02, 0x7F, 0x0A, 0x12, 0x22, 0x3E, 0x42,
    /*02F0:*/ 0x02, 0x3F, 0x42, 0x0A, 0x0A, 0x7F, 0x0A, 0x0A, 0x1E, 0x22, 0x42, 0x46, 0x08, 0x02, 0x3E, 0x42,
    /*0300:*/ 0x03, 0x04, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x02, 0x1F, 0x22, 0x4F, 0x02, 0x1C, 0x20, 0x40, 0x4A,
    /*0310:*/ 0x4A, 0x46, 0x2A, 0x12, 0x22, 0x42, 0x46, 0x4A, 0x42, 0x3F, 0x02, 0x1E, 0x20, 0x40, 0x48, 0x06,
    /*0320:*/ 0x1E, 0x32, 0x4A, 0x46, 0x08, 0x08, 0x09, 0x3E, 0x4A, 0x0A, 0x1E, 0x20, 0x4E, 0x00, 0x0E, 0x04,
    /*0330:*/ 0x05, 0x3D, 0x45, 0x04, 0x00, 0x10, 0x08, 0x7F, 0x00, 0x04, 0x04, 0x1F, 0x24, 0x44, 0x40, 0x42,
    /*0340:*/ 0x42, 0x42, 0x40, 0x06, 0x2A, 0x12, 0x2A, 0x42, 0x22, 0x16, 0x7B, 0x12, 0x22, 0x00, 0x1E, 0x20,
    /*0350:*/ 0x40, 0x00, 0x78, 0x04, 0x02, 0x00, 0x78, 0x44, 0x44, 0x44, 0x44, 0x3F, 0x1E, 0x22, 0x42, 0x42,
    /*0360:*/ 0x02, 0x30, 0x08, 0x04, 0x02, 0x04, 0x32, 0x02, 0x7F, 0x02, 0x32, 0x0E, 0x52, 0x22, 0x12, 0x02,
    /*0370:*/ 0x20, 0x2A, 0x2A, 0x2A, 0x02, 0x70, 0x20, 0x22, 0x24, 0x38, 0x06, 0x28, 0x10, 0x28, 0x40, 0x4A,
    /*0380:*/ 0x4A, 0x4A, 0x3E, 0x0A, 0x06, 0x0A, 0x02, 0x7F, 0x02, 0x40, 0x7E, 0x42, 0x42, 0x40, 0x7E, 0x4A,
    /*0390:*/ 0x4A, 0x4A, 0x4A, 0x1C, 0x25, 0x45, 0x05, 0x04, 0x00, 0x1F, 0x20, 0x40, 0x0F, 0x20, 0x40, 0x7E,
    /*03A0:*/ 0x00, 0x7C, 0x08, 0x10, 0x20, 0x40, 0x7E, 0x7E, 0x42, 0x42, 0x42, 0x7E, 0x1E, 0x22, 0x42, 0x02,
    /*03B0:*/ 0x06, 0x18, 0x20, 0x40, 0x42, 0x42, 0x00, 0x04, 0x02, 0x04, 0x02, 0x00, 0x00, 0x07, 0x05, 0x07,
    /*03C0:*/ 0x3E, 0x45, 0x49, 0x51, 0x3E, 0x2A, 0x7E, 0x2A, 0x3A, 0x27, 0x7F, 0x15, 0x15, 0x15, 0x7F, 0x7F,
    /*03D0:*/ 0x49, 0x49, 0x49, 0x7F, 0x7E, 0x12, 0x1E, 0x12, 0x7E, 0x1C, 0x5E, 0x7F, 0x5E, 0x1C, 0x1E, 0x3E,
    /*03E0:*/ 0x7C, 0x3E, 0x1E, 0x18, 0x3C, 0x7E, 0x3C, 0x18, 0x1C, 0x5B, 0x7F, 0x5B, 0x1C, 0x7F, 0x7F, 0x7F,
    /*03F0:*/ 0x7F, 0x7F, 0x63, 0x41, 0x41, 0x41, 0x63, 0x41, 0x3F, 0x01, 0x3F, 0x41, 0x01, 0x01, 0x7F, 0x20,
    /*0400:*/ 0x10, 0x08, 0x10, 0x08, 0x04, 0x08, 0x20, 0x10, 0x08, 0x04, 0x02, 0x42, 0x52, 0x45, 0x41, 0x4B,
    /*0410:*/ 0x20, 0x49, 0x4E, 0x20, 0x45, 0x52, 0x52, 0x4F, 0x52, 0x59, 0x48, 0x4E, 0x0C, 0x0B, 0x01, 0x55,
    /*0420:*/ 0x4A, 0x4D, 0x08, 0x01, 0x49, 0x4B, 0x20, 0x02, 0x4F, 0x4C, 0x0D, 0x50, 0x3D, 0x16, 0x10, 0x12,
    /*0430:*/ 0x15, 0x2C, 0x3B, 0x3A, 0x29, 0x51, 0x41, 0x5A, 0x2D, 0x2A, 0x2F, 0x28, 0x57, 0x53, 0x58, 0x2B,
    /*0440:*/ 0x33, 0x36, 0x39, 0x45, 0x44, 0x43, 0x2E, 0x32, 0x35, 0x38, 0x52, 0x46, 0x56, 0x30, 0x31, 0x34,
    /*0450:*/ 0x37, 0x54, 0x47, 0x42, 0x0E, 0x0F, 0x05, 0x04, 0x07, 0x01, 0xB6, 0xC9, 0xD9, 0x0C, 0x0B, 0x01,
    /*0460:*/ 0xB8, 0xCB, 0xDB, 0x08, 0x01, 0xBA, 0xCD, 0x20, 0x02, 0xBC, 0xCF, 0x0D, 0xBE, 0xD1, 0x16, 0x10,
    /*0470:*/ 0x12, 0x15, 0xB0, 0xA3, 0xA4, 0x29, 0xB1, 0xC0, 0xD3, 0xA1, 0x2A, 0x2F, 0x28, 0xB2, 0xC2, 0xD4,
    /*0480:*/ 0xDD, 0x33, 0x36, 0x39, 0xB3, 0xC3, 0xD5, 0x2E, 0x32, 0x35, 0x38, 0xB4, 0xC5, 0xD6, 0x30, 0x31,
    /*0490:*/ 0x34, 0x37, 0xB5, 0xC7, 0xD7, 0x0E, 0x0F, 0x05, 0x04, 0x07, 0x01, 0x26, 0x88, 0x8E, 0x0C, 0x0B,
    /*04A0:*/ 0x01, 0x3F, 0x8A, 0x8D, 0x08, 0x01, 0xFB, 0x8B, 0xF1, 0x03, 0xFC, 0x8C, 0x06, 0x40, 0xF4, 0x16,
    /*04B0:*/ 0x10, 0x12, 0x15, 0x2C, 0x3B, 0x3A, 0x3E, 0x21, 0x81, 0xFA, 0x5E, 0x5C, 0x2F, 0x3C, 0x22, 0xF3,
    /*04C0:*/ 0xF8, 0x2B, 0x33, 0x36, 0x39, 0x23, 0x84, 0x83, 0x2E, 0x32, 0x35, 0x38, 0x24, 0x86, 0xF6, 0x30,
    /*04D0:*/ 0x31, 0x34, 0x37, 0x25, 0x87, 0x82, 0x14, 0x13, 0x05, 0x04, 0x07, 0x01, 0xB7, 0xCA, 0xDA, 0x0C,
    /*04E0:*/ 0x0B, 0x01, 0xB9, 0xCC, 0xDC, 0x08, 0x01, 0xBB, 0xCE, 0x20, 0x03, 0xBD, 0xD0, 0x06, 0xBF, 0xD2,
    /*04F0:*/ 0x16, 0x10, 0x12, 0x15, 0xDE, 0xA2, 0xA5, 0x3E, 0xA7, 0xC1, 0xD3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};


Clcdc_pc1211::Clcdc_pc1211(CPObject *parent)	: Clcdc(parent){						//[constructor]
    Color_Off.setRgb(
                        (int) (102*contrast),
                        (int) (106*contrast),
                        (int) (29*contrast));

    pPC1211 = (Cpc1211*) parent;

    cursorPos=0;
    cursorBlink=false;
    blinkState =  0;
}

static const struct {
    int x,y;
} pc1211_pos[11]={
    {250,0},	// SHIFT
    {200, 0},	// DEF
    {280, 0},	// PRO
    {240, 0},	// RUN
    {320, 0},	// RESERVE
    {88,0},	// DE
    {96,0},	// G
    {101,0},	// RAD
    {290,0},	// E
    {30,0},		// P
    {0,0}		// BUSY
};


#define PRO_MODE    (pPC1211->pBASIC->runMode==CTinyBasic::PRO)
#define RUN_MODE    (pPC1211->pBASIC->runMode==CTinyBasic::RUN)
#define RSV_MODE    (pPC1211->pBASIC->runMode==CTinyBasic::RESERVE)
#define DEF_MODE    (pPC1211->pBASIC->runMode==CTinyBasic::DEF)

#define DEG_IND     (pPC1211->pBASIC->angleMode==CTinyBasic::DEGREE)
#define RAD_IND     (pPC1211->pBASIC->angleMode==CTinyBasic::RADIAN)
#define GRD_IND     (pPC1211->pBASIC->angleMode==CTinyBasic::GRAD)



void Clcdc_pc1211::disp_symb(void)
{

//        disp_one_symb(S_SHIFT,	COLOR(SYMB2_1250&0x02),	pc1211_pos[0].x,	pc1211_pos[0].y);
        disp_one_symb(S_DEF,		COLOR(DEF_MODE),	pc1211_pos[1].x,	pc1211_pos[1].y);
        disp_one_symb(S_PRO,		COLOR(PRO_MODE),	pc1211_pos[2].x,	pc1211_pos[2].y);
        disp_one_symb(S_RUN,		COLOR(RUN_MODE),	pc1211_pos[3].x,	pc1211_pos[3].y);
        disp_one_symb(S_RESERVE,	COLOR(RSV_MODE),    pc1211_pos[4].x,	pc1211_pos[4].y);
        disp_one_symb(S_DE,         COLOR(DEG_IND),     pc1211_pos[5].x,	pc1211_pos[5].y);
        disp_one_symb(S_G,          COLOR(DEG_IND||GRD_IND),	pc1211_pos[6].x,	pc1211_pos[6].y);
        disp_one_symb(S_RAD,		COLOR(RAD_IND||GRD_IND),	pc1211_pos[7].x,	pc1211_pos[7].y);
//        disp_one_symb(S_E,          COLOR(SYMB2_1250&0x08),	pc1211_pos[8].x,	pc1211_pos[8].y);
//        disp_one_symb(S_PRINT,      COLOR(SYMB1_1250&0x02),	pc1211_pos[9].x,	pc1211_pos[9].y);
//        disp_one_symb(S_BUSY,       COLOR(SYMB2_1250&0x01),	pc1211_pos[10].x,	pc1211_pos[10].y);



        Refresh = TRUE;


    Clcdc::disp_symb();

}

void Clcdc_pc1211::disp()
{

    disp_symb();

//    qWarning()<< "DISP:"<<pPC1211->pBASIC->outputBuffer<<"**";
    line.fill(0,24);
    QByteArray *buf;
    if (pPC1211->pBASIC->inputMode) buf= &pPC1211->inputBuffer;
    else buf = &pPC1211->pBASIC->outputBuffer;
    if (!buf->isEmpty()) {
        line.prepend(buf->mid(0,buf->indexOf('\n')));
        pPC1211->DisplayWaitForRTN = buf->contains('\n');
    }


    for (int i=0;i<24;i++) {
        quint8 c =0;
        c=line.at(i);
        DrawChar(c,i);
    }


    if (pPC->pTIMER->msElapsed(blinkState)>500) {
        DrawChar(0xff,cursorPos);
    }
    if (pPC->pTIMER->msElapsed(blinkState)>1000) {
        blinkState = pPC->pTIMER->state;
    }

    Refresh = true;
}

void Clcdc_pc1211::DrawChar(quint8 c, int x)
{
//    Refresh = FALSE;
    //	disp_symb();
    QPainter painter(pPC->LcdImage);

    for (int ind=0; ind<5; ind++)
    {
        int adr = c*5;
        quint8 data = pc1211_carDef[adr+ind];

        int off =x*6+4-ind;			// +1 every 5 cols

        for (int b=0; b<7;b++)
        {
            Refresh = TRUE;
            painter.setPen( ((data>>b)&0x01) ? Color_On : Color_Off );
            painter.drawPoint( off, b);
        }
    }

    redraw = 0;
    painter.end();

}
