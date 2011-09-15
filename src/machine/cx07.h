#ifndef CX07_H
#define CX07_H

class CPObject;
#include "pcxxxx.h"
class CT6834;


typedef struct
 {
  qint8 F0;
  qint8 F1;
  qint8 F2;
  qint8 F3;
  qint8 F4;
  qint8 F5;
  qint8 F6;
  qint8 F7;
 } PORT_FX;

typedef struct
 {
  PORT_FX R;
  PORT_FX W;
 } PorT_FX;

typedef struct
 {
  int size_point_x;
  int size_point_y;
  qint8 Scroll_Min_Y;
  qint8 Scroll_Max_Y;
  qint8 Curs_X;
  qint8 Curs_Y;
  qint8 Curseur;
  qint8 Aff_Udk;
  qint8 Stick;
  qint8 Strig;
  qint8 Strig1;
  FILE *K7_File;
  qint8 Baud_Out_Mode;
  qint8 Rem_Canal;
  qint8 Break;
  char F_Key [12][80];

 } GENERAL_INFO;

typedef struct
 {
  qint8 Buff_Key[20];
  qint8 Nb_Key;
  qint8 Pt_Lec;
  qint8 Pt_Ecr;
 } CLAVIER;

#define NB_POINT_CAR_X 6
#define NB_POINT_CAR_Y 8
#define MAX_X          120
#define MAX_Y          32

class Cx07 : public CpcXXXX
{
public:
    Cx07(CPObject *parent = 0);

    virtual bool	Chk_Adr(DWORD *d,DWORD data);
    virtual bool	Chk_Adr_R(DWORD *d,DWORD data);
    UINT8 in(UINT8 address);
    UINT8 out(UINT8 address,UINT8 value);
    bool init();

    PorT_FX Port_FX;
    void ReceiveFromT6834(UINT8 Cmd, PorT_FX *Port);
    void SendToT6834(PorT_FX *Port);

    qint8 Mode_K7;
    int IT_T6834;
    qint8 Int_nsc800_BB;
    qint8 Send_Cmd_T6834 [255];
    CT6834 *pT6834;
    GENERAL_INFO General_Info;
    void ClrScr();
    qint8 Ram_Video[120][32];
    void RefreshVideo();
    void AffCurseur();
    void AffCar(qint8 x, qint8 y, qint8 Car);


    qint8 First;
    qint8 Loc_x,Loc_y;
    static qint8 Car_Def[256][8];
    void ScrollVideo();
    void LineClear(qint8 P_y);
    void Preset(qint8 x, qint8 y);
    void Pset(qint8 x, qint8 y);
    void Line(qint8 x1, qint8 y1, qint8 x2, qint8 y2);
    void Circle(int x, int y, int r);


    CLAVIER Clavier;
    bool run();
    void AddKey(qint8 Key);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void Reset();

    int Cpt;
    int Lng_Cmd;
    int Lng_Rsp;

    qint8 Rsp[80];
    int  pt;
    int  lng_rsp;
    qint8 Ordre;
};

#endif // CX07_H
