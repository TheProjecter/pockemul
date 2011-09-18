#ifndef CX07_H
#define CX07_H

class CPObject;
#include "pcxxxx.h"
class CT6834;


typedef struct
 {
  UINT8 F0;
  UINT8 F1;
  UINT8 F2;
  UINT8 F3;
  UINT8 F4;
  UINT8 F5;
  UINT8 F6;
  UINT8 F7;
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
  UINT8 Scroll_Min_Y;
  UINT8 Scroll_Max_Y;
  UINT8 Curs_X;
  UINT8 Curs_Y;
  UINT8 Curseur;
  UINT8 Aff_Udk;
  UINT8 Stick;
  UINT8 Strig;
  UINT8 Strig1;
  FILE *K7_File;
  UINT8 Baud_Out_Mode;
  UINT8 Rem_Canal;
  UINT8 Break;
  char F_Key [12][80];

 } GENERAL_INFO;

typedef struct
 {
  UINT8 Buff_Key[20];
  UINT8 Nb_Key;
  UINT8 Pt_Lec;
  UINT8 Pt_Ecr;
 } CLAVIER;

#define NB_POINT_CAR_X 6
#define NB_POINT_CAR_Y 8
#define MAX_X          120
#define MAX_Y          32

#define IT_RST_A       0x3C  // Interuptions T6834
#define IT_RST_B       0x34  // Interuptions Liaisons séries et Timer
#define IT_RST_C       0x2C  // Interuptions non utilisées
#define IT_NMI         0x66  // Non Maskable Interrupt



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

    UINT8 Mode_K7;
    int IT_T6834;
    UINT8 Int_nsc800_BB;

    CT6834 *pT6834;
    GENERAL_INFO General_Info;
    void ClrScr();
    UINT8 Ram_Video[120][32];
    void RefreshVideo();
    void AffCurseur();
    void AffCar(UINT8 x, UINT8 y, UINT8 Car);


    UINT8 First;
    UINT8 Loc_x,Loc_y;

    void ScrollVideo();
    void LineClear(UINT8 P_y);
    void Preset(UINT8 x, UINT8 y);
    void Pset(UINT8 x, UINT8 y);
    void Line(UINT8 x1, UINT8 y1, UINT8 x2, UINT8 y2);
    void Circle(int x, int y, int r);


    //CLAVIER Clavier;
    QByteArray Clavier;

    bool run();
    void AddKey(UINT8 Key);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void Reset();

    int Cpt;
    int Lng_Cmd;
    int Lng_Rsp;

    UINT8 Rsp[80];
    int  pt;
    int  lng_rsp;
    UINT8 Ordre;

    int Nb;
    int Lec_K7;
    void AddFKey(UINT8 F_Key);


    static const UINT8 key_tbl[256];
    static const UINT8 key_tbl_c[256];
    static const UINT8 key_tbl_s[256];
    static const UINT8 key_tbl_k[256];
    static const UINT8 key_tbl_ks[256];
    static const UINT8 key_tbl_g[256];


};

#endif // CX07_H
