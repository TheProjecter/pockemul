#ifndef _COMMON_H
#define _COMMON_H

#include <QMessageBox>

#define TRACE(c)             QMessageBox::about(mainwindow, mainwindow->tr("Trace"),mainwindow->tr(c));
#define MSG_ERROR(c)         QMessageBox::critical(mainwindow, mainwindow->tr("ERROR"),c);

#if 1
#include <stdio.h>
#include <string.h>
#endif 



#if 1
#define INLINE inline
#else
#define INLINE
#endif

#define ADDLOG if (mainwindow->dialoganalog) Addlog



#define	POCKEMUL_VERSION	"0.9"

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

#define READ_BIT(b,p)	( ((b)>>(p)) & 0x01 ? 1 :0 )
#define SET_BIT(b,p)	((b) |= (0x01<<(p)))
#define UNSET_BIT(b,p)	((b) &= ~(0x01<<(p)))
#define PUT_BIT(b,p,v)	if (v) SET_BIT(b,p); else UNSET_BIT(b,p);

#define B0	0x01
#define B1	0x02
#define B2	0x04
#define B3	0x08
#define B4	0x10
#define B5	0x20
#define B6	0x40
#define B7	0x80

#define PIN_MT_OUT2	1
#define PIN_GND		2
#define PIN_VGG		3
#define PIN_BUSY	4
#define PIN_D_OUT	5
#define PIN_MT_IN	6
#define PIN_MT_OUT1	7
#define PIN_D_IN	8
#define PIN_ACK		9
#define PIN_SEL2	10
#define PIN_SEL1	11

#define RGB(r,g,b)	((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g))<<8)) | (((DWORD)(BYTE)(b))<<16)))

#include "fmod.h"
//#include "fmod_errors.h"

#include "global.h"

typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef unsigned char	BYTE;
typedef unsigned int    UINT;
typedef 		 DWORD	COLORREF;	


typedef struct tagPOINT
{
	long x;
	long y;
} POINT,*PPOINT;

typedef struct tagRECT
{
	long left;
	long top;
	long right;
	long bottom;
} RECT;

typedef struct TransMap
{
	int		Icode;
    char	KeyDescription[256];
	int		ScanCode;
	int		x1,y1,Type;
}; 



typedef int		Bool;
typedef long	ErrCode;
typedef unsigned char uchar;
typedef unsigned long ulong;

#endif // _COMMON_H

