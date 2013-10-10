// *********************************************************************
//    Copyright (c) 1989-2002  Warren Furlow
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// *********************************************************************

// *********************************************************************
// Chp41Keyboard.cpp : implementation file
// *********************************************************************

#include "hp41.h"

/****************************/
// These are VKs that are missing from system defines
/****************************/
#define VK_0 '0'
#define VK_1 '1'
#define VK_2 '2'
#define VK_3 '3'
#define VK_4 '4'
#define VK_5 '5'
#define VK_6 '6'
#define VK_7 '7'
#define VK_8 '8'
#define VK_9 '9'
#define VK_A 'A'
#define VK_Z 'Z'
#define VK_OEM_COLON  0xba   // ;: key
#define VK_OEM_PLUS   0xbb   // += key
#define VK_OEM_COMMA  0xbc   // ,< key
#define VK_OEM_MINUS  0xbd   // -_ key
#define VK_OEM_PERIOD 0xbe   // .> key
#define VK_OEM_2      0xbf   // /? key
#define VK_OEM_QUOTE  0xde   // '" key
#define VK_ENTER 13

/****************************/
// Table of Chp41 keycodes for unshifed alpha
// used for mapping PC alpha keys to Chp41 keys
/****************************/
byte UnshiftAlphaTable[26]=
  {
  0x10,0x30,0x70,0x80,0xc0,   /* A-E */
  0x11,0x31,0x71,0x81,0xc1,   /* F-J */
       0x32,0x72,0x82,        /* K-M */
  0x13,     0x73,0x83,        /* N-P */
  0x14,0x34,0x74,0x84,        /* Q-T */
  0x15,0x35,0x75,0x85,        /* U-X */
  0x16,0x36                   /* Y-Z */
  };

/****************************/
// keycode mappings
/****************************/
byte KeyCode[8][5]=
  {
  {0x10,0x30,0x70,0x80,0xc0},
  {0x11,0x31,0x71,0x81,0xc1},
  {0x12,0x32,0x72,0x82,0xc2},
  {0x13,0   ,0x73,0x83,0xc3},
  {0x14,0x34,0x74,0x84,0   },
  {0x15,0x35,0x75,0x85,0   },
  {0x16,0x36,0x76,0x86,0   },
  {0x17,0x37,0x77,0x87,0   }
  };

/****************************/
// Pixel locations of the keys for keyboard bitmap
// left,top,right,bottom
/****************************/
typedef int RECT[4];
typedef int POINT[2];

RECT RectKeyboardTiny[]=
  {
  /* on key row */
  {4,31,31,42},
  {39,31,70,42},
  {73,31,106,42},
  {115,31,144,42},

  /* row 1 */
  {3,47,27,62},
  {31,47,56,62},
  {62,47,86,62},
  {90,47,115,62},
  {119,47,144,62},

  /* row 2 */
  {3,65,27,80},
  {31,65,56,80},
  {62,65,86,80},
  {90,65,115,80},
  {119,65,144,80},

  /* row 3 */
  {4,84,28,99},
  {31,84,56,99},
  {62,84,86,99},
  {90,84,115,99},
  {119,84,144,99},

  /* row 4 */
  {3,103,58,118},
  {62,103,86,118},
  {90,103,115,118},
  {119,102,144,118},

  /* row 5 */
  {4,123,35,139},
  {40,123,72,139},
  {75,123,107,139},
  {110,123,144,139},

  /* row 6 */
  {4,143,35,159},
  {40,143,72,159},
  {75,142,107,159},
  {110,142,144,159},

  /* row 7 */
  {4,163,35,180},
  {40,163,72,180},
  {75,163,107,180},
  {110,162,144,179},

  /* row 8 */
  {4,184,35,200},
  {40,184,72,200},
  {75,184,107,200},
  {110,184,144,200},

  {0,0,0,0},        // keyboard overlay slide
  {0,0,0,0},         // control
  };

RECT RectKeyboardSmall[]=
  {
  /* on key row */
  {9,42,43,53},
  {50,42,83,53},
  {129,42,162,53},
  {169,42,202,53},

  /* row 1 */
  {14,64,41,82},
  {53,64,80,82},
  {92,64,119,82},
  {131,64,157,82},
  {169,64,197,82},

  /* row 2 */
  {14,93,41,111},
  {53,93,80,111},
  {92,93,119,111},
  {131,93,157,111},
  {169,93,197,111},

  /* row 3 */
  {14,122,41,139},
  {53,122,80,139},
  {92,122,119,139},
  {131,122,157,139},
  {169,122,197,139},

  /* row 4 */
  {14,152,81,169},
  {92,152,119,169},
  {131,152,157,169},
  {169,152,197,169},

  /* row 5 */
  {14,181,41,198},
  {62,181,92,198},
  {114,181,145,198},
  {166,181,198,198},

  /* row 6 */
  {14,209,41,227},
  {62,209,92,227},
  {114,209,145,227},
  {166,209,198,227},

  /* row 7 */
  {14,238,41,256},
  {62,238,92,256},
  {114,238,145,256},
  {166,238,198,256},

  /* row 8 */
  {14,267,41,285},
  {62,267,92,285},
  {114,267,145,285},
  {166,267,198,285},

  {95,39,116,53},        // keyboard overlay slide
  {0,0,0,0},        // control
  };

RECT RectKeyboardMedium[]=
  {
  /* on key row */
  {24,77,57,89},
  {64,77,98,89},
  {144,77,177,89},
  {184,77,218,89},

  /* row 1 */
  {37,122,62,142},
  {73,122,98,142},
  {109,122,134,142},
  {145,122,169,142},
  {180,122,205,142},

  /* row 2 */
  {37,157,62,178},
  {73,157,98,178},
  {109,157,134,178},
  {145,157,169,178},
  {180,157,205,178},

  /* row 3 */
  {37,193,62,214},
  {73,193,98,214},
  {109,193,134,214},
  {145,193,169,214},
  {180,193,205,214},

  /* row 4 */
  {37,229,98,250},
  {109,229,134,250},
  {145,229,169,250},
  {180,229,205,250},

  /* row 5 */
  {37,265,62,286},
  {81,265,110,286},
  {129,265,158,286},
  {177,265,205,286},

  /* row 6 */
  {37,301,62,322},
  {81,301,110,322},
  {129,301,158,322},
  {177,301,205,322},

  /* row 7 */
  {37,337,62,358},
  {81,337,110,358},
  {129,337,158,358},
  {177,337,205,358},

  /* row 8 */
  {37,373,62,394},
  {81,373,110,394},
  {129,373,158,394},
  {177,373,205,394},

  {110,84,132,99},        // keyboard overlay slide
  {0,0,0,0},        // control
  };

RECT RectKeyboardLarge[]=
  {
  /* on key row */
  {24,104,74,122},
  {74,104,125,122},
  {184,104,233,122},
  {233,104,284,122},

  /* row 1 */
  {43,165,76,194},
  {91,165,124,194},
  {139,165,172,194},
  {186,165,220,194},
  {234,165,268,194},

  /* row 2 */
  {43,212,76,242},
  {91,212,124,242},
  {139,212,172,242},
  {186,212,220,242},
  {234,212,268,242},

  /* row 3 */
  {43,259,76,290},
  {91,259,124,290},
  {139,259,172,290},
  {186,259,220,290},
  {235,259,268,290},

  /* row 4 */
  {43,308,124,338},
  {139,308,172,338},
  {186,308,220,338},
  {234,308,268,338},

  /* row 5 */
  {43,355,75,386},
  {100,355,139,386},
  {165,355,203,386},
  {228,355,267,386},

  /* row 6 */
  {43,403,75,434},
  {100,403,139,434},
  {165,403,203,434},
  {228,403,267,434},

  /* row 7 */
  {43,452,75,482},
  {100,452,139,482},
  {165,452,203,482},
  {228,452,267,482},

  /* row 8 */
  {43,499,75,530},
  {100,499,139,530},
  {165,499,203,530},
  {228,499,267,530},

  {140,114,169,135},        // keyboard overlay slide
  {0,0,0,0},        // control
  };

// rects for LCD and annunicator blt
// left,top,right,bottom
RECT RectLCDTiny={4,4,144,18};
RECT RectLCDTinyTT={4,4,144,18};
RECT RectAnnunTiny={4,19,144,24};

RECT RectLCDSmall={17,15,197,28};
RECT RectLCDSmallTT={15,11,197,28};
RECT RectAnnunSmall={17,29,197,34};

RECT RectLCDMedium={31,33,215,49};
RECT RectLCDMediumTT={30,31,214,48};
RECT RectAnnunMedium={34,50,214,55};

RECT RectLCDLarge={41,46,275,65};
RECT RectLCDLargeTT={35,43,274,67};
RECT RectAnnunLarge={44,68,280,78};


static const int RegionPointCount = 16;
//these coordinates are picked straight off the bitmap
static const POINT MediumShape[RegionPointCount] = {
  {211, 3},
  {211, 9},
  {229, 9},
  {233, 139},
  {235, 235},
  {231, 336},
  {228, 419},
  {225, 424},

  {18,  424},
  {15,  419},
  {12,  336},
  {8,   235},
  {10,  139},
  {14,  9},
  {32,  9},
  {33,  3}};

static const POINT LargeShape[RegionPointCount] = {
  {275, 7},
  {276, 14},
  {299, 14},
  {303, 199},
  {303, 359},
  {299, 499},
  {297, 562},
  {292, 570},

  {16,  570},
  {12,  562},
  {11,  499},
  {6,   359},
  {6,   199},
  {9,   14},
  {33,  14},
  {33,  7}};

static const int LargeMargin = 30;
static const int MediumMargin = 30;
static const int SmallMargin = 12;
static const int TinyMargin = 5;

//Input: Rect = ClientRect
//Output: Rect = ActiveRect
#if 0
static void GetActiveRect(RECT *Rect, int Margin)
  {
  InflateRect(Rect, -Margin, -Margin);
  }

static void SetWindowShape(CWnd* MainWindow, const POINT* RegionPoints) //MGL
  {
  POINT Origin;
  RECT WindowPos;
  Origin.x = 0;
  Origin.y = 0;
  MainWindow->ClientToScreen(&Origin);
  MainWindow->GetWindowRect(&WindowPos);

  Origin.x -= WindowPos.left;
  Origin.y -= WindowPos.top;

  POINT Points[RegionPointCount];
  for (int i = 0; i < RegionPointCount; ++i)
    {
    Points[i].x = RegionPoints[i].x + Origin.x;
    Points[i].y = RegionPoints[i].y + Origin.y;
    }
  MainWindow->SetWindowRgn(CreatePolygonRgn(Points, RegionPointCount, WINDING), true);
  }

static void NoWindowShape(CWnd* MainWindow)
  {
  MainWindow->SetWindowRgn(NULL, FALSE);
  }

#endif

/**************************************/
// setup Chp41 keyboard and other stuff
// set eKeyboardNone only once as the last thing before termination
/**************************************/
void Chp41::SetKeyboard(
  int eKbd,            // keyboard selection
  flag TrueType,       // use truetype font
  flag ShowTitle,      // setup window for title bar
  flag ShowMenu)       // setup window for menu
  {

#if 0
  static flag FirstTime=1;
  LOGFONT lf;

  if (FirstTime)
    {
    FirstTime=0;
    if (!AddFontResource("LCD.FON"))
      AfxMessageBox("Unable to load the font file: LCD.FON\nRebooting will usually fix this");
    if (!AddFontResource("LCD4.TTF"))
      AfxMessageBox("Unable to load the font file: LCD4.TTF\nRebooting will usually fix this");
    brushRed.CreateSolidBrush(RGB(255,0,0));
    brushGray.CreateSolidBrush(RGB(127,127,127));
    }

  // delete objects that might already exist
  if (CFontLCD.m_hObject)
    CFontLCD.DeleteObject();
  if (CFontAnnun.m_hObject)
    CFontAnnun.DeleteObject();
  if (CBMKeyboard.m_hObject)
    CBMKeyboard.DeleteObject();
  if (CPalKeyboard.m_hObject)
    CPalKeyboard.DeleteObject();
  FreeBMP(pBMKeyboard,pPalKeyboard,pPalIndexKeyboard);

  // setup font resources
  CDC *pCDC;
  pCDC=theApp.m_pMainWnd->GetDC();
  pCDC->SetMapMode(MM_TEXT);
  eKeyboard=eKbd;
  memset(&lf,0,sizeof(LOGFONT));
  lf.lfCharSet=SYMBOL_CHARSET;
  lf.lfWeight=FW_NORMAL;
  if (TrueType)
    {
    lf.lfQuality=ANTIALIASED_QUALITY;
    strcpy(lf.lfFaceName,"LCD4");
    eFont=eFontLCD4;
    if (eKeyboard==eKeyboardTiny)
      {
      pRectLCD=&RectLCDTinyTT;
      pRectAnnun=&RectAnnunTiny;
      }
    else if (eKeyboard==eKeyboardSmall)
      {
      pRectLCD=&RectLCDSmallTT;
      pRectAnnun=&RectAnnunSmall;
      }
    else if (eKeyboard==eKeyboardMedium)
      {
      pRectLCD=&RectLCDMediumTT;
      pRectAnnun=&RectAnnunMedium;
      }
    else if (eKeyboard==eKeyboardLarge)
      {
      pRectLCD=&RectLCDLargeTT;
      pRectAnnun=&RectAnnunLarge;
      }
    // find the largest font that will fit inside the LCD
    char szDisplay[50];     // test string
    CSize Size,SizeAlt;
    int HeightAlt;
    // alternate punc chars
    UseAltPunc=1;
    GetLCD(szDisplay);
    lf.lfHeight=28;
    do
      {
      lf.lfHeight--;
      CFontLCD.CreateFontIndirect(&lf);
      pCDC->SelectObject(CFontLCD);
      SizeAlt=pCDC->GetOutputTextExtent(szDisplay,24);
      CFontLCD.DeleteObject();
      }
    while ((SizeAlt.cx>pRectLCD->right-pRectLCD->left) || (SizeAlt.cy>pRectLCD->bottom-pRectLCD->top));
    HeightAlt=lf.lfHeight;
    // normal punc chars
    UseAltPunc=0;
    GetLCD(szDisplay);
    lf.lfHeight=28;
    do
      {
      lf.lfHeight--;
      CFontLCD.CreateFontIndirect(&lf);
      pCDC->SelectObject(CFontLCD);
      Size=pCDC->GetOutputTextExtent(szDisplay,24);
      CFontLCD.DeleteObject();
      }
    while ((Size.cx>pRectLCD->right-pRectLCD->left) || (Size.cy>pRectLCD->bottom-pRectLCD->top));
    // see if alt punc chars fit better (they are narrower)
    if (SizeAlt.cx>Size.cx)
      {
      lf.lfHeight=HeightAlt;
      UseAltPunc=1;
      }
    }

  else   // non truetype fonts
    {
    lf.lfPitchAndFamily=VARIABLE_PITCH|FF_DONTCARE;
    UseAltPunc=0;
    if (eKeyboard==eKeyboardTiny)
      {
      strcpy(lf.lfFaceName,"LCD1");
      lf.lfHeight=11;
      lf.lfWidth=11;
      eFont=eFontLCD1;
      pRectLCD=&RectLCDTiny;
      pRectAnnun=&RectAnnunTiny;
      }
    else if (eKeyboard==eKeyboardSmall)
      {
      strcpy(lf.lfFaceName,"LCD1");
      lf.lfHeight=11;
      lf.lfWidth=11;
      eFont=eFontLCD1;
      pRectLCD=&RectLCDSmall;
      pRectAnnun=&RectAnnunSmall;
      }
    else if (eKeyboard==eKeyboardMedium)
      {
      strcpy(lf.lfFaceName,"LCD2");
      lf.lfHeight=14;
      lf.lfWidth=11;
      eFont=eFontLCD2;
      pRectLCD=&RectLCDMedium;
      pRectAnnun=&RectAnnunMedium;
      }
    else if (eKeyboard==eKeyboardLarge)
      {
      strcpy(lf.lfFaceName,"LCD3");
      lf.lfHeight=18;
      lf.lfWidth=13;
      eFont=eFontLCD3;
      pRectLCD=&RectLCDLarge;
      pRectAnnun=&RectAnnunLarge;
      }
    }
  CFontLCD.CreateFontIndirect(&lf);

  // annunciator
  memset(&lf,0,sizeof(LOGFONT));
  lf.lfCharSet=SYMBOL_CHARSET;
  lf.lfPitchAndFamily=VARIABLE_PITCH|FF_DONTCARE;
  lf.lfWeight=FW_NORMAL;
  if ((eKeyboard==eKeyboardTiny)||(eKeyboard==eKeyboardSmall)||(eKeyboard==eKeyboardMedium))
    {
    strcpy(lf.lfFaceName,"ANNUN1");
    lf.lfHeight=5;
    lf.lfWidth=5;
    }
  else if (eKeyboard==eKeyboardLarge)
    {
    strcpy(lf.lfFaceName,"ANNUN2");
    lf.lfHeight=7;
    lf.lfWidth=7;
    }
  CFontAnnun.CreateFontIndirect(&lf);
  theApp.m_pMainWnd->ReleaseDC(pCDC);

  // setup keyboard bitmap resource
  RECT RectWindow;
  int NonClientX=2*GetSystemMetrics(SM_CXBORDER)+2*GetSystemMetrics(SM_CXFRAME);
  int NonClientY=2*GetSystemMetrics(SM_CYBORDER)+2*GetSystemMetrics(SM_CYFRAME);
  if (ShowTitle)
    NonClientY+=GetSystemMetrics(SM_CYCAPTION);
  if (ShowMenu)
    NonClientY+=GetSystemMetrics(SM_CYMENU);
  theApp.m_pMainWnd->GetWindowRect(&RectWindow);
  switch (eKeyboard)
    {
    case eKeyboardTiny:
      {
      LoadBMP("Tiny.bmp",pBMKeyboard,pPalKeyboard,pPalIndexKeyboard);
      pRectKeyboard=RectKeyboardTiny;
      NoWindowShape(theApp.m_pMainWnd);
      break;
      }
    default:      // out of bounds input
      eKeyboard=eKeyboardSmall;
    case eKeyboardSmall:
      {
      LoadBMP("Small.bmp",pBMKeyboard,pPalKeyboard,pPalIndexKeyboard);
      pRectKeyboard=RectKeyboardSmall;
      break;
      }
    case eKeyboardMedium:
      {
      LoadBMP("Medium.bmp",pBMKeyboard,pPalKeyboard,pPalIndexKeyboard);
      pRectKeyboard=RectKeyboardMedium;
      break;
      }
    case eKeyboardLarge:
      {
      LoadBMP("Large.bmp",pBMKeyboard,pPalKeyboard,pPalIndexKeyboard);
      pRectKeyboard=RectKeyboardLarge;
      break;
      }
    case eKeyboardNone:   // clean up - do this only once at app termination
      {
      RemoveFontResource("LCD.FON");
      RemoveFontResource("LCD4.TTF");
      break;
      }
    }
  if (pBMKeyboard)
    theApp.m_pMainWnd->MoveWindow(RectWindow.left,RectWindow.top,pBMKeyboard->biWidth+NonClientX,pBMKeyboard->biHeight+NonClientY,TRUE);
  else      // bitmap did not laod
    eKeyboard=eKeyboardNone;

  //set active window and region - MGL
  theApp.m_pMainWnd->GetClientRect(&ActiveRect);
  bool SetRegion = (!ShowTitle && !ShowMenu);
  switch (eKeyboard)
    {
    case eKeyboardTiny:
      {
      GetActiveRect(&ActiveRect, TinyMargin);
      NoWindowShape(theApp.m_pMainWnd);
      break;
      }
    case eKeyboardSmall:
      {
      GetActiveRect(&ActiveRect, SmallMargin);
      NoWindowShape(theApp.m_pMainWnd);
      break;
      }
    case eKeyboardMedium:
      {
      GetActiveRect(&ActiveRect, MediumMargin);
      if (SetRegion)  //MGL
        SetWindowShape(theApp.m_pMainWnd, MediumShape);
      else
        NoWindowShape(theApp.m_pMainWnd);
      break;
      }
     case eKeyboardLarge:
       {
       GetActiveRect(&ActiveRect, LargeMargin);
       if (SetRegion)
         SetWindowShape(theApp.m_pMainWnd, LargeShape);
       else
         NoWindowShape(theApp.m_pMainWnd);
       break;
       }
     default:
       {
       NoWindowShape(theApp.m_pMainWnd);
       }
     }

  theApp.pPalForeground=pPalKeyboard;
  theApp.m_pMainWnd->RedrawWindow(NULL, NULL, RDW_FRAME|RDW_INVALIDATE);
#endif
}


/**********************************/
int Chp41::GetKeyboard(void)
  {
  return(eKeyboard);
  }


/**********************************/
// Simulates a key press
/**********************************/



#if 0
/**************************************/
// Maps pixels to keyboard for mouse clicks
// this returns 0 if no key
// or it returns the keycode of key
/**************************************/
byte Chp41::MapPointToKey(CPoint pt)
  {
  int row,col;

  if (eKeyboard==eKeyboardNone)
    return(0);
  if (!CBMKeyboard.m_hObject)
    return(0);

  // LCD
  if ( (pt.y<pRectLCD->bottom) && (pt.y>pRectLCD->top) &&
    (pt.x>=pRectLCD->left) && (pt.x<pRectLCD->right) )
    return(1);

  // keyboard overlay slide
  if ( (pt.y<pRectKeyboard[39].bottom) && (pt.y>pRectKeyboard[39].top) &&
    (pt.x>=pRectKeyboard[39].left) && (pt.x<pRectKeyboard[39].right) )
    return(2);

  // ON key row
  if (pt.y<(pRectKeyboard+0)->top)
    return(0);
  if (pt.y<(pRectKeyboard+0)->bottom)
    {
    if ( (pt.x>=(pRectKeyboard+0)->left) && (pt.x<(pRectKeyboard+0)->right) )
      return(0x18);
    if ( (pt.x>=(pRectKeyboard+1)->left) && (pt.x<(pRectKeyboard+1)->right) )
      return(0xC6);
    if ( (pt.x>=(pRectKeyboard+2)->left) && (pt.x<(pRectKeyboard+2)->right) )
      return(0xC5);
    if ( (pt.x>=(pRectKeyboard+3)->left) && (pt.x<(pRectKeyboard+3)->right) )
      return(0xC4);
    return(0);
    }

  // first 4 rows
  if (pt.y<(pRectKeyboard+19)->bottom)
    {
    if ( (pt.y>=(pRectKeyboard+4)->top) && (pt.y<(pRectKeyboard+4)->bottom) )
      row=0;
    else if ( (pt.y>=(pRectKeyboard+9)->top) && (pt.y<(pRectKeyboard+9)->bottom) )
      row=1;
    else if ( (pt.y>=(pRectKeyboard+14)->top) && (pt.y<(pRectKeyboard+14)->bottom) )
      row=2;
    else if ( (pt.y>=(pRectKeyboard+19)->top) && (pt.y<(pRectKeyboard+19)->bottom) )
      {
      row=3;
      if ( (pt.x>=(pRectKeyboard+19)->left) && (pt.x<(pRectKeyboard+19)->right) )
        return(KeyCode[row][0]);   // enter key is special case
      }
    else
      return(0);
    if ( (pt.x>=(pRectKeyboard+4)->left) && (pt.x<(pRectKeyboard+4)->right) )
      col=0;
    else if ( (pt.x>=(pRectKeyboard+5)->left) && (pt.x<(pRectKeyboard+5)->right) )
      col=1;
    else if ( (pt.x>=(pRectKeyboard+6)->left) && (pt.x<(pRectKeyboard+6)->right) )
      col=2;
    else if ( (pt.x>=(pRectKeyboard+7)->left) && (pt.x<(pRectKeyboard+7)->right) )
      col=3;
    else if ( (pt.x>=(pRectKeyboard+8)->left) && (pt.x<(pRectKeyboard+8)->right) )
      col=4;
    else
      return(0);
    return(KeyCode[row][col]);
    }

  // last 4 rows
  if (pt.y<(pRectKeyboard+35)->bottom)
    {
    if ( (pt.y>=(pRectKeyboard+23)->top) && (pt.y<(pRectKeyboard+23)->bottom) )
      row=4;
    else if ( (pt.y>=(pRectKeyboard+27)->top) && (pt.y<(pRectKeyboard+27)->bottom) )
      row=5;
    else if ( (pt.y>=(pRectKeyboard+31)->top) && (pt.y<(pRectKeyboard+31)->bottom) )
      row=6;
    else if ( (pt.y>=(pRectKeyboard+35)->top) && (pt.y<(pRectKeyboard+35)->bottom) )
      row=7;
    else
      return(0);
    if ( (pt.x>=(pRectKeyboard+23)->left) && (pt.x<(pRectKeyboard+23)->right) )
      col=0;
    else if ( (pt.x>=(pRectKeyboard+24)->left) && (pt.x<(pRectKeyboard+24)->right) )
      col=1;
    else if ( (pt.x>=(pRectKeyboard+25)->left) && (pt.x<(pRectKeyboard+25)->right) )
      col=2;
    else if ( (pt.x>=(pRectKeyboard+26)->left) && (pt.x<(pRectKeyboard+26)->right) )
      col=3;
    else
      return(0);

    return(KeyCode[row][col]);
    }

  return(0);
  }
#endif

/**********************************/
// Maps PC keyboard codes to HP-41 keycode
// returns 0 if key is not used
/**********************************/
#if 0
byte Chp41::MapKeyToKey(
  flag fAlt,                  // true if this is an alt key
  flag fExtended,             // true if this is an extended PC keycode
  int PCKey)                  // PC keycode
  {
  if (fAlt)
    return(0);

  if (fExtended)
    {
    switch (PCKey)
      {
      case VK_ENTER:
        return(0x13);
      case VK_DIVIDE:
        return(0x17);
      }
    return(0);
    }

  if ( (PCKey>=VK_A) && (PCKey<=VK_Z) )
    return(UnshiftAlphaTable[PCKey-VK_A]);

  switch (PCKey)
    {
    case VK_F1:
      return(0);        // HELP
    case VK_F2:
      return(0x18);     // ON
    case VK_F3:
      return(0xc6);     // USER
    case VK_F4:
      return(0xc5);     // PRGM
    case VK_F5:
      return(0xc4);     // ALPHA
    case VK_F6:
      return(0xc2);     // SST
    case VK_F7:
      return(0x87);     // R/S
    case VK_F8:
    case VK_F9:
    case VK_F10:
    case VK_F11:
    case VK_F12:
      return(0);
    case VK_SHIFT:
      return(0x12);     // SHIFT
    case VK_CONTROL:
      return(1);        // COPY LCD
    case VK_TAB:
      return(3);        // TURBO
    case VK_BACK:
    case VK_ESCAPE:
      return(0xc3);     // BACKARROW
    case VK_OEM_MINUS:  // -_ KEY
    case VK_SUBTRACT:
      return(0x14);
    case VK_OEM_PLUS:   // += KEY
    case VK_ADD:
      return(0x15);
    case VK_MULTIPLY:
    case VK_OEM_QUOTE:  // '"
      return(0x16);
    case VK_OEM_2:      // /? KEY
    case VK_DIVIDE:
      return(0x17);
    case VK_0:
    case VK_NUMPAD0:
    case VK_INSERT:
      return(0x37);
    case VK_1:
    case VK_NUMPAD1:
    case VK_END:
      return(0x36);
    case VK_2:
    case VK_NUMPAD2:
    case VK_DOWN:
      return(0x76);
    case VK_3:
    case VK_NUMPAD3:
    case VK_NEXT:
      return(0x86);
    case VK_4:
    case VK_NUMPAD4:
    case VK_LEFT:
      return(0x35);
    case VK_5:
    case VK_NUMPAD5:
    case VK_CLEAR:
      return(0x75);
    case VK_6:
    case VK_NUMPAD6:
    case VK_RIGHT:
      return(0x85);
    case VK_7:
    case VK_NUMPAD7:
    case VK_HOME:
      return(0x34);
    case VK_8:
    case VK_NUMPAD8:
    case VK_UP:
      return(0x74);
    case VK_9:
    case VK_NUMPAD9:
    case VK_PRIOR:
      return(0x84);
    case VK_DELETE:
    case VK_OEM_PERIOD:
    case VK_DECIMAL:
    case VK_OEM_COMMA:
      return(0x77);
    case VK_SPACE:
      return(0x37);
    case VK_ENTER:
      return(0x13);
    }
  return(0);
  }
#endif

/**********************************/
// simulates key press
/**********************************/
void Chp41::MoveKey(
  byte Key,
  int State)
  {

#if 0
  int KeyNumber;

  if (eKeyboard==eKeyboardNone)
    return;
  if (!CBMKeyboard.m_hObject)
    return;
  if (!Key)
    return;

  if (Key<=3)
    {
    RECT *pRectHotSpot;
    switch(Key)
      {
      case 1:       // LCD
        pRectHotSpot=pRectLCD;
        break;
      case 2:       // slide
        pRectHotSpot=&pRectKeyboard[39];
        break;
      case 3:       // CONTROL
        pRectHotSpot=&pRectKeyboard[40];
        break;
      }
    if (State==MOVE)
      {
      CPalette *pCPalOld;
      CDC *pCDC=theApp.m_pMainWnd->GetDC();
      if (fUsePal)
        {
        pCPalOld=pCDC->SelectPalette(&CPalKeyboard,FALSE);
        pCDC->RealizePalette();
        }
      pCDC->InvertRect(pRectHotSpot);
      if (fUsePal)
        pCDC->SelectPalette(pCPalOld,FALSE);
      theApp.m_pMainWnd->ReleaseDC(pCDC);
      }
    else
      theApp.m_pMainWnd->InvalidateRect(pRectHotSpot,FALSE);          // restore original bitmap
    return;
    }

  switch(Key)
    {
    case 0x18:  KeyNumber=0;   break;
    case 0xc6:  KeyNumber=1;   break;
    case 0xc5:  KeyNumber=2;   break;
    case 0xc4:  KeyNumber=3;   break;
    case 0x10:  KeyNumber=4;   break;
    case 0x30:  KeyNumber=5;   break;
    case 0x70:  KeyNumber=6;   break;
    case 0x80:  KeyNumber=7;   break;
    case 0xc0:  KeyNumber=8;   break;
    case 0x11:  KeyNumber=9;   break;
    case 0x31:  KeyNumber=10;  break;
    case 0x71:  KeyNumber=11;  break;
    case 0x81:  KeyNumber=12;  break;
    case 0xc1:  KeyNumber=13;  break;
    case 0x12:  KeyNumber=14;  break;
    case 0x32:  KeyNumber=15;  break;
    case 0x72:  KeyNumber=16;  break;
    case 0x82:  KeyNumber=17;  break;
    case 0xc2:  KeyNumber=18;  break;
    case 0x13:  KeyNumber=19;  break;
    case 0x73:  KeyNumber=20;  break;
    case 0x83:  KeyNumber=21;  break;
    case 0xc3:  KeyNumber=22;  break;
    case 0x14:  KeyNumber=23;  break;
    case 0x34:  KeyNumber=24;  break;
    case 0x74:  KeyNumber=25;  break;
    case 0x84:  KeyNumber=26;  break;
    case 0x15:  KeyNumber=27;  break;
    case 0x35:  KeyNumber=28;  break;
    case 0x75:  KeyNumber=29;  break;
    case 0x85:  KeyNumber=30;  break;
    case 0x16:  KeyNumber=31;  break;
    case 0x36:  KeyNumber=32;  break;
    case 0x76:  KeyNumber=33;  break;
    case 0x86:  KeyNumber=34;  break;
    case 0x17:  KeyNumber=35;  break;
    case 0x37:  KeyNumber=36;  break;
    case 0x77:  KeyNumber=37;  break;
    case 0x87:  KeyNumber=38;  break;
    }

  // simulate a keypress
  if (State==MOVE)
    {
    CPalette *pCPalOld;
    CDC *pCDC=theApp.m_pMainWnd->GetDC();
    if (fUsePal)
      {
      pCPalOld=pCDC->SelectPalette(&CPalKeyboard,FALSE);
      pCDC->RealizePalette();
      }
    if (eKeyboard==eKeyboardTiny)  // just invert bitmap
      pCDC->InvertRect(pRectKeyboard+KeyNumber);
    else                                      // simulate key movement
      {
      CBitmap *pbmOld;
      CDC dcMem;
      int FillFactor,MoveFactor;
      if (eKeyboard==eKeyboardTiny)
        {
        FillFactor=1;
        MoveFactor=1;
        }
      if (eKeyboard==eKeyboardSmall)
        {
        FillFactor=2;
        MoveFactor=2;
        }
      else if (eKeyboard==eKeyboardMedium)
        {
        FillFactor=3;
        MoveFactor=3;
        }
      else           // Large
        {
        FillFactor=5;
        MoveFactor=3;
        }
      RECT *pRect=pRectKeyboard+KeyNumber;    // the rect for this key

      dcMem.CreateCompatibleDC(pCDC);
      pbmOld=dcMem.SelectObject(&CBMKeyboard);
      switch(KeyNumber)      // move button
        {
        case 0:     // on
          pCDC->BitBlt(pRectKeyboard[0].left,pRectKeyboard[0].top,
            pRectKeyboard[1].right-pRectKeyboard[0].left-MoveFactor,pRectKeyboard[0].bottom-pRectKeyboard[0].top,
            &dcMem,pRectKeyboard[0].left+MoveFactor,pRectKeyboard[0].top,SRCCOPY);    // draw it offset slightly left
          break;
        case 1:     // user
          pCDC->BitBlt(pRectKeyboard[0].left+MoveFactor,pRectKeyboard[0].top,
            pRectKeyboard[1].right-pRectKeyboard[0].left-MoveFactor,pRectKeyboard[0].bottom-pRectKeyboard[0].top,
            &dcMem,pRectKeyboard[0].left,pRectKeyboard[0].top,SRCCOPY);    // draw it offset slightly right
          break;
        case 2:     // prgm
          pCDC->BitBlt(pRectKeyboard[2].left,pRectKeyboard[2].top,
            pRectKeyboard[3].right-pRectKeyboard[2].left-MoveFactor,pRectKeyboard[2].bottom-pRectKeyboard[2].top,
            &dcMem,pRectKeyboard[2].left+MoveFactor,pRectKeyboard[2].top,SRCCOPY);    // draw it offset slightly left
          break;
        case 3:     // alpha
          pCDC->BitBlt(pRectKeyboard[2].left+MoveFactor,pRectKeyboard[2].top,
            pRectKeyboard[3].right-pRectKeyboard[2].left-MoveFactor,pRectKeyboard[2].bottom-pRectKeyboard[2].top,
            &dcMem,pRectKeyboard[2].left,pRectKeyboard[2].top,SRCCOPY);    // draw it offset slightly right
          break;
        default:    // all other keys
          pCDC->BitBlt(pRect->left,pRect->top,
            pRect->right-pRect->left,pRect->bottom-pRect->top-MoveFactor,
            &dcMem,pRect->left,pRect->top+MoveFactor,SRCCOPY);    // draw it offset slightly up
          break;
        }
      switch(KeyNumber)     // backfill where key was by filling in a clean part of another key
        {
        case 0:     // on
          pCDC->BitBlt(pRectKeyboard[1].right-4,pRectKeyboard[1].top,
            2,pRectKeyboard[1].bottom-pRectKeyboard[1].top,
            &dcMem,pRectKeyboard[1].right-2,pRectKeyboard[1].top,SRCCOPY);
          break;
        case 1:     // user
          pCDC->BitBlt(pRectKeyboard[0].left+2,pRectKeyboard[0].top,
            2,pRectKeyboard[0].bottom-pRectKeyboard[0].top,
            &dcMem,pRectKeyboard[1].right-2,pRectKeyboard[1].top,SRCCOPY);
          break;
        case 2:     // prgm
          pCDC->BitBlt(pRectKeyboard[3].right-4,pRectKeyboard[3].top,
            2,pRectKeyboard[3].bottom-pRectKeyboard[3].top,
            &dcMem,pRectKeyboard[3].right-2,pRectKeyboard[3].top,SRCCOPY);
          break;
        case 3:     // alpha
          pCDC->BitBlt(pRectKeyboard[2].left+2,pRectKeyboard[2].top,
            2,pRectKeyboard[2].bottom-pRectKeyboard[2].top,
            &dcMem,pRectKeyboard[3].right-2,pRectKeyboard[3].top,SRCCOPY);
          break;
        case 14:    // shift
          pCDC->BitBlt(pRect->left,pRect->bottom-FillFactor,
            pRect->right-pRect->left,FillFactor,
            &dcMem,pRectKeyboard[14].left,pRectKeyboard[14].bottom-FillFactor,SRCCOPY);  // use blank part of shift key
          break;
        case 19:    // enter
          pCDC->BitBlt(pRect->left,pRect->bottom-FillFactor,
            pRect->right-pRect->left,FillFactor,
            &dcMem,pRectKeyboard[19].left,pRectKeyboard[19].bottom-FillFactor,SRCCOPY);  // use enter key
          pCDC->BitBlt(pRect->left+25,pRect->bottom-FillFactor,
            25,FillFactor,
            &dcMem,pRectKeyboard[19].left,pRectKeyboard[19].bottom-FillFactor,SRCCOPY);
          break;
        default:    // all other keys
          if ((KeyNumber<=22)||(KeyNumber==23)||(KeyNumber==27)||(KeyNumber==31)||(KeyNumber==35)) // smaller keys
            pCDC->BitBlt(pRect->left,pRect->bottom-FillFactor,
              pRect->right-pRect->left,FillFactor,
              &dcMem,pRectKeyboard[18].left,pRectKeyboard[18].bottom-FillFactor,SRCCOPY);  // use blank part of SST key
          else                                                                                     // larger keys
            pCDC->BitBlt(pRect->left,pRect->bottom-FillFactor,
              pRect->right-pRect->left,FillFactor,
              &dcMem,pRectKeyboard[38].left,pRectKeyboard[38].bottom-FillFactor,SRCCOPY);  // use blank part of R/S part
        }
      dcMem.SelectObject(pbmOld);
      dcMem.DeleteDC();
      }
    if (fUsePal)
      pCDC->SelectPalette(pCPalOld,FALSE);
    theApp.m_pMainWnd->ReleaseDC(pCDC);
    }

  // restore key to normal
  else
    {
    if (eKeyboard==eKeyboardTiny)
      theApp.m_pMainWnd->InvalidateRect(pRectKeyboard+KeyNumber,FALSE);          // restore original bitmap
    else
      {
      RECT Rect;
      switch(KeyNumber)      // restore all of on/user or prgm/alpha button
        {
        case 0:     // on
        case 1:     // user
          Rect=pRectKeyboard[0];
          Rect.right=pRectKeyboard[1].right;
          break;
        case 2:     // prgm
        case 3:     // alpha
          Rect=pRectKeyboard[2];
          Rect.right=pRectKeyboard[3].right;
          break;
        default:    // all other keys
          Rect=pRectKeyboard[KeyNumber];
          break;
        }
      theApp.m_pMainWnd->InvalidateRect(&Rect,FALSE);
      }
    }
#endif
  }

