#ifndef CT6834_H
#define CT6834_H

#include <Qt>
#include "pobject.h"
#include "cx07.h"

typedef struct
 {
  UINT8 lng_send;
  UINT8 lng_rsp;
  char *Str_Cmd;
 } CMD_T6834;

class CT6834 : public CPObject
{
public:
    CT6834(CPObject *parent = 0);

    static const qint8 Date[8];

    UINT8 Send_Cmd_T6834 [255];
    UINT8 Locate_OnOff;
    UINT8 Loc_X;
    UINT8 Loc_Y;

    UINT8 Loc_x,Loc_y;

    static const CMD_T6834 Cmd_T6834[0x48];
    int InitReponseT6834(UINT8 Ordre, UINT8 *Rsp, PorT_FX *Port);
    UINT8 R5;
    Cx07 *pPC;
    UINT8 Ram_Video[120][32];
    QTime cursorTimer;
    UINT8 First;

    void AffUdkON(bool State);
    void ClrScr();
    void RefreshVideo();
    void AffCurseur();
    void AffCar(UINT8 x, UINT8 y, UINT8 Car);
    void ScrollVideo();
    void LineClear(UINT8 P_y);
    void Pset(UINT8 x, UINT8 y);
    void Preset(UINT8 x, UINT8 y);
    void Line(UINT8 x1, UINT8 y1, UINT8 x2, UINT8 y2);
    void Circle(int x, int y, int r);
};

#endif // CT6834_H
