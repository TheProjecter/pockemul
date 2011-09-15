#include <QtGui>

#include "common.h"
#include "cx07.h"
#include "z80.h"
#include "Inter.h"
#include "init.h"
#include "ct6834.h"
#include "Log.h"
#include "Lcdc_x07.h"

#include "cx07char.h"
/*
           Mémoire du XO7
  0000    ----------------
         |  XXXX  |  RAM  |
  2000    ----------------
         | Carte RAM 8Ko  |
  4000    ----------------
         | Prise ROM 8Ko  |
  6000    ----------------
         | ROM Carte 8Ko  |
  8000    ----------------
         | V-RAM 6Ko      |
  9800    ----------------
         | ~~~~~~~~~~~~~~ |
  A000    ----------------
         | ROM TV 4Ko     |
  B000    ----------------
         | ROM 20Ko BASIC |
  FFFF    ----------------

*/

#define INIT_T6834     0xf0
#define LEC_T6834      0xf1
#define LEC_T6834_ACK  0xf2

Cx07::Cx07(CPObject *parent)	: CpcXXXX(parent)
{								//[constructor]
    setfrequency( (int) 576000/3);
    setcfgfname(QString("x07"));

    SessionHeader	= "X07PKM";
    SessionHeaderLen= 6;
    Initial_Session_Fname ="x07.pkm";

    BackGroundFname	= ":/x07/x07.png";
    LcdFname		= ":/x07/x07lcd.png";
    SymbFname		= ":/x07/x07symb.png";

    memsize		= 0x10000;
    InitMemValue	= 0xff;

    SlotList.clear();
    SlotList.append(CSlot(8 , 0x0000 ,	""                  , ""	, RAM , "RAM"));
    SlotList.append(CSlot(8 , 0x2000 ,	""					, ""	, RAM , "Carte RAM 8kb"));
    SlotList.append(CSlot(8 , 0x4000 ,	""					, ""	, ROM , "Prise ROM 8kb"));
    SlotList.append(CSlot(8 , 0x6000 ,	""					, ""	, ROM , "ROM Carte 8kb"));
    SlotList.append(CSlot(6 , 0x8000 ,	""					, ""	, RAM , "Carte RAM 8kb"));
    SlotList.append(CSlot(4 , 0xA000 ,	""                  , ""	, ROM , "ROM TV"));
    SlotList.append(CSlot(20, 0xB000 ,	":/x07/rom_xo7.bin" , "x07/rom_xo7.bin" 	, ROM , "BASIC ROM"));

    KeyMap		= KeyMap1250;
    KeyMapLenght= KeyMap1250Lenght;

    PowerSwitch	= 0;
    Pc_Offset_X = Pc_Offset_Y = 0;

    setDXmm(200);//Pc_DX_mm = 135;
    setDYmm(130);//Pc_DY_mm = 70;
    setDZmm(30);//Pc_DZ_mm = 10;

    setDX(715);//Pc_DX		= 483;//409;
    setDY(465);//Pc_DY		= 252;//213;

    Lcd_X		= 55;
    Lcd_Y		= 49;
    Lcd_DX		= 240;//168;//144 ;
    Lcd_DY		= 64;
    Lcd_ratio_X	= 2;// * 1.18;
    Lcd_ratio_Y	= 2;// * 1.18;

    Lcd_Symb_X	= 55;//(int) (45 * 1.18);
    Lcd_Symb_Y	= 41;//(int) (35 * 1.18);
    Lcd_Symb_DX	= 339;
    Lcd_Symb_DY	= 5;
    Lcd_Symb_ratio_X	= 1;//1.18;

    PowerSwitch = 0;

    pLCDC		= new Clcdc_x07(this);
    pCPU		= new CZ80(this);
    pT6834      = new CT6834(this);
    pTIMER		= new Ctimer(this);
    //pCONNECTOR	= new Cconnector(this,11,0,"Connector 11 pins",false,QPoint(1,87));		publish(pCONNECTOR);
    //pKEYB		= new Ckeyb(this,"x07.map",scandef_x07);

    First = 1;
    Cpt = 0;

}

bool Cx07::init(void)				// initialize
{
    memset((void*)&Port_FX,0,sizeof (Port_FX));
    memset((void*)&Clavier,0,sizeof (Clavier));
    //fp_CRVA = 0;
    // if DEBUG then log CPU
#ifndef QT_NO_DEBUG
    //pCPU->logsw = true;
#endif
    CpcXXXX::init();



    WatchPoint.remove(this);


    QMessageBox::about(this, tr("Attention"),"Canon X-07 Emulation is in alpha stage.");

    ((CZ80 *) pCPU)->z80.r16.pc = 0xC3C3;
    return true;
}

void Cx07::AddKey (qint8 Key)
{
    if (Clavier.Nb_Key < 20)
    {
        Clavier.Nb_Key ++;
        Clavier.Buff_Key[Clavier.Pt_Ecr] = Key & 0XFF;
        Clavier.Pt_Ecr ++;
        if (Clavier.Pt_Ecr >=20)
            Clavier.Pt_Ecr = 0;
    }
}

bool Cx07::run() {

    if (Clavier.Nb_Key)
     {
      Clavier.Nb_Key --;
      Port_FX.R.F1 = Clavier.Buff_Key[Clavier.Pt_Lec];
      Clavier.Pt_Lec ++;
      if (Clavier.Pt_Lec >=20)
       Clavier.Pt_Lec = 0;
      Port_FX.R.F0  = 0x00;
      Port_FX.R.F2 |= 0x01;
      IT_T6834      = 0;
      //return (IT_RST_A);
     }

    CpcXXXX::run();


}

bool Cx07::Chk_Adr(DWORD *d, DWORD data)
{
    if ( (*d>=0x0000) && (*d<=0x3FFF) )	return(true);		// RAM area()
    if ( (*d>=0x8000) && (*d<=0x97FF) )	return(true);		// RAM area()

    return false;
}

bool Cx07::Chk_Adr_R(DWORD *d, DWORD data)
{
    return true;
}

UINT8 Cx07::in(UINT8 Port)
{
    UINT8 Value=0;
     switch (Port)
      {
       case 0xF0 : /* Controle des interruptions */
                   Value = Port_FX.R.F0;
                   break;
       case 0xF1 : /* XBRR : Stockage des Informations venant du CCU */
                   Value = Port_FX.R.F1;
    /*               Port_FX.R.F2 &= 0xFE;*/
                   break;
       case 0xF2 : /* Flags sur l'etat du Canon XO7 */
    /*               if (Port_FX.W.F5 & 0x08) Port_FX.R.F2 |=0x01;
                   else                     Port_FX.R.F2 &=0xFE;*/
                   if (Port_FX.W.F5 & 0x04) Port_FX.R.F2 |=0x02;
                   else                     Port_FX.R.F2 &=0xFD;
                   Value = Port_FX.R.F2 | 2;
                   break;
       case 0xF3 : /* ~~~Vide~~~ */
                   Value = Port_FX.R.F3;
                   break;
       case 0xF4 : /* Modes */
                   Value = Port_FX.R.F4;
                   break;
       case 0xF5 : /* ~~~Vide~~~ */
                   Value = Port_FX.R.F5;
                   break;
       case 0xF6 : /* Status de l'UART */
                   if (Mode_K7) Port_FX.R.F6 |= 0x05;
                   Value = Port_FX.R.F6;
                   break;
       case 0xF7 : /* Données recu par l'UART */
                   Value = Port_FX.R.F7;
                   break;
      }

     pCPU->imem[Port] = Value;
     return (Value);
}

UINT8 Cx07::out(UINT8 Port, UINT8 Value)
{
 # if 1
#if AFF_OUT
 if ((Port!=0xf0) && (Value!=0x44))
 fprintf (stderr,"(%04X) Out %02X,%02X\n",Reg_Xo7.PC.W,Port,Value);
#endif

 switch (Port)
  {
   case 0xBB : /* Validation des interuptions du NSC800 */
               Int_nsc800_BB = Value;
               break;

   case 0xF0 : /* Controle des interruptions */
               Port_FX.W.F0 = Value;
               break;
   case 0xF1 : /* XBTR : Stockage des Informations pour CCU */
               Port_FX.W.F1 = Value;
               break;
   case 0xF2 : /* Controle de BAUDS (poids faible) */
               Port_FX.W.F2 = Value;
               break;
   case 0xF3 : /* Controle de BAUDS (poids fort) */
               Port_FX.W.F3 = Value;
               break;
   case 0xF4 : /* Modes */
               Port_FX.W.F4 = Value;
               Port_FX.R.F4 = Value;
               if ((Value & 0x0D) == 0x09) Mode_K7=1;
               else                        Mode_K7=0;
               if ((Mode_K7==1) && (Port_FX.W.F5& 0x04)) {
//                   Receive_from_K7 (&Port_FX);
               }
               break;
   case 0xF5 : /* Interruptions (RESET) */
               Port_FX.W.F5 = Value;
               /* Reception d'un octet venant du T6834 *
                *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
               if (Value & 0x01)
                ReceiveFromT6834 (LEC_T6834_ACK,&Port_FX);
               /* Envoie d'un octet a destination du T6834 *
                *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
               if (Value & 0x02)
                SendToT6834 (&Port_FX);
               /* Reception d'un octet EN PROVENANCE du lecteur de K7 *
                *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
               if (Value & 0x04) {
//                   Receive_from_K7 (&Port_FX);
               }
               /* Envoie d'un octet a destination du lecteur de K7 *
                *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
               if (Value & 0x08) {
//                   Send_to_K7 (&Port_FX);
               }
               /* Envoie d'un bit sur le port imprimante  *
                *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
               if (Value & 0x20) {
//                   Print (PRT_DATA,&Port_FX);
               }
               break;
   case 0xF6 : /* Configuration de L'UART */
               Port_FX.W.F6 = Value;
               break;
   case 0xF7 : /* Données émises par l'UART */
               Port_FX.W.F7 = Value;
               break;
  }
#endif
 return 0;
}

void Cx07::ReceiveFromT6834(UINT8 Cmd, PorT_FX *Port)
{

 switch (Cmd)
  {
   case INIT_T6834    : memset ((void*)Rsp,0,sizeof(Rsp));
                        pt=0;
                        lng_rsp = Get_8(0x020F); // RAM [0x026D];
                        Ordre = Send_Cmd_T6834[0];
#if AFF_DEBUG
                        fprintf (stderr,"INIT_T6834: Ordre=%02X Lng_Rsp=%02X (%02X)\n",Ordre,lng_rsp,Cmd_T6834[Ordre].lng_rsp);
#endif
                        lng_rsp = pT6834->InitReponseT6834(Ordre, Rsp, Port);
                        if (lng_rsp)
                         {
                          IT_T6834 = 1;
#if AFF_IT
                          fprintf (stderr,"Attente reponse...\n");
#endif
                         }
                        break;

   case LEC_T6834     : /*if (RAM[0x020F] != 0)*/
                         {
                          Port_FX.R.F0  = 0x40;
                          Port_FX.R.F1  = Rsp[pt];
                          Port_FX.R.F2 |= 0x01;
#if AFF_RCV_T6834
                          fprintf (stderr,"LEC_T6834: Data=%02X\n",Rsp[pt]);
#endif
                         }
                        break;

   case LEC_T6834_ACK : pt++;
                        fprintf (stderr,"LEC_T6834_ACK: IT_");
                        if (pt < lng_rsp)
                        {
                            IT_T6834 = 1;
                            fprintf (stderr,"ON\n");
                        }
                        else
                            fprintf (stderr,"OFF\n");

                        Port_FX.R.F2 &= 0xFE;

                        break;

   default : //fprintf (stderr,"Commande au T6834 inconnue[%02X]!!!\n",RAM [0x020F]);
             break;
  }
}

void Cx07::SendToT6834 (PorT_FX *Port)
{

// int i;

 if (!Cpt)
  {
   if (pT6834->Locate_OnOff &&
      ((Port->W.F1 & 0x7F) != 0x24) &&
      ((Port->W.F1) >= 0x20) &&
      ((Port->W.F1) <  0x80))
    {
     General_Info.Curs_X ++;
     //General_Info.Curs_Y ++;
//     fputc (Port->W.F1,stderr);
     fprintf (stderr,"(%02X) %c",Port->W.F1,Port->W.F1);
     AffCar (General_Info.Curs_X,General_Info.Curs_Y,Port->W.F1);
    }
   else
    {
     pT6834->Locate_OnOff = 0;
//     Loc_X = Loc_Y = 0xff;
     if (((Port->W.F1) & 0x7F) < 0x47)
      {
       Send_Cmd_T6834 [Cpt++] = Port->W.F1 & 0x7F;
       Lng_Cmd = CT6834::Cmd_T6834[Port->W.F1 & 0x7F].lng_send;
       Lng_Rsp = CT6834::Cmd_T6834[Port->W.F1 & 0x7F].lng_rsp;
       fprintf (stderr,"0x%02X: Send:%02X Rsp:%d (%d)\n",Port->W.F1,Lng_Cmd,Lng_Rsp,Cpt);
      }
     else
         AddLog(LOG_TEMP,tr("Cmd T6834 inconnu (%1)[%2]").arg(Port->W.F1,2,16,QChar('0')).arg(Get_8(0x020F),2,16,QChar('0')));
    }
  }
 else
  {
   Send_Cmd_T6834 [Cpt++] = Port->W.F1;
   if ((Lng_Cmd & 0x80) && (Cpt>(Lng_Cmd&0x7F)) && (!Port->W.F1))
    {
     fprintf (stderr,"0x%02X: Send:%02X Rsp:%d (%d)[%d]\n",Send_Cmd_T6834 [0],Lng_Cmd,Lng_Rsp,Cpt,Port->W.F1);
     Lng_Cmd = Cpt;
    }
   if (Cpt == 2)
    switch (Send_Cmd_T6834 [0])
     {
      case 0x0C : if (Send_Cmd_T6834 [1] == 0xB0)
                   {
                    ClrScr ();
                    Send_Cmd_T6834 [0] = Send_Cmd_T6834 [1]&0x7f;
                    Lng_Cmd = CT6834::Cmd_T6834[Send_Cmd_T6834 [0] & 0x7F].lng_send;
                    Lng_Rsp = CT6834::Cmd_T6834[Send_Cmd_T6834 [0] & 0x7F].lng_rsp;
                    Cpt --;
                   }
                  break;
      case 0x07 : if (Send_Cmd_T6834 [1] > 4)
                   {
                    fputc (pT6834->Send_Cmd_T6834[0],stderr);
                    Send_Cmd_T6834 [0] = Send_Cmd_T6834 [1]&0x7f;
                    Lng_Cmd = CT6834::Cmd_T6834[Send_Cmd_T6834 [0] & 0x7F].lng_send;
                    Lng_Rsp = CT6834::Cmd_T6834[Send_Cmd_T6834 [0] & 0x7F].lng_rsp;
                    Cpt --;
                   }
                  break;
     }

  }

 if (Cpt && (Cpt == Lng_Cmd))
  {
#if AFF_SND_T6834
   for (i=0; i < Cpt; i++)
    fprintf (stderr,"%02X ",Send_Cmd_T6834[i]);
#endif
#if AFF_STR_SND_T6834
   fprintf (stderr,"(%s) \n",Cmd_T6834[Send_Cmd_T6834[0]].Str_Cmd);
#endif
   ReceiveFromT6834 (INIT_T6834,Port);
    Cpt = 0;
  }
}

void Cx07::ClrScr (void)
{
 qint8 x,y;

 for (x=0 ; x<120 ; x++)
  for (y=0 ; y<32 ; y++)
   Ram_Video [x][y] = 0;
 RefreshVideo ();
}

void Cx07::RefreshVideo (void)
{
 int x;
 int y;
 int ColorIndex;
 for (x=0;x<120;x++)
  for (y=0;y<32;y++)
   {
    QColor col = (Ram_Video[x][y])?pLCDC->Color_On:pLCDC->Color_Off;
    QPainter painter(LcdImage);
    painter.setPen(  col  );
    painter.drawPoint(x,y);
    painter.end();
//    XPutImage (display,win,mygc[ColorIndex],ima,0,0,x*General_Info.size_point_x,
//                                                    y*General_Info.size_point_y,
//                                                      General_Info.size_point_x,
//                                                      General_Info.size_point_y);
   }
 AffCurseur ();
 //XFlush(display);
 Refresh_Display = true;
}

void Cx07::AffCurseur (void)
{

 qint8 x,y;


 if (!First)
  {
   if (General_Info.Curseur)
    {
     y = ((Loc_y+1) * General_Info.size_point_y * NB_POINT_CAR_Y) -General_Info.size_point_y;
     x =   Loc_x    * General_Info.size_point_x * NB_POINT_CAR_X;

     QPainter painter(LcdImage);
     painter.setPen(  pLCDC->Color_Off  );
     painter.drawLine(x,y,x+NB_POINT_CAR_X*General_Info.size_point_x,y+1);
     painter.end();
//     XPutImage (display,win,mygc[7],ima,0,0,x,y,NB_POINT_CAR_X*General_Info.size_point_x,1);
    }
  }
 else First = 0;

 Loc_x = General_Info.Curs_X;
 Loc_y = General_Info.Curs_Y;
 if (General_Info.Curseur)
  {
   y = ((Loc_y+1) * General_Info.size_point_y * NB_POINT_CAR_Y) -General_Info.size_point_y;
   x =   Loc_x    * General_Info.size_point_x * NB_POINT_CAR_X;
   QPainter painter(LcdImage);
   painter.setPen(  pLCDC->Color_On  );
   painter.drawLine(x,y,x+NB_POINT_CAR_X*General_Info.size_point_x,y+1);
   painter.end();
   //XPutImage (display,win,mygc[0],ima,0,0,x,y,NB_POINT_CAR_X*General_Info.size_point_x,1);
   //XFlush(display);
  }
}

void Cx07::AffCar(qint8 x, qint8 y, qint8 Car)
{
 int P_x,P_y;
 qint8 Mask;

 /* On efface le caractere precedent */
 /*----------------------------------*/
// XPutImage (display,win,mygc[7],ima,0,0,x*NB_POINT_CAR_X*General_Info.size_point_x,
//                                        y*NB_POINT_CAR_Y*General_Info.size_point_y,
//                                        NB_POINT_CAR_X*General_Info.size_point_x,
//                                        NB_POINT_CAR_Y*General_Info.size_point_y);

 /* Dessin du caractere point par point */
 /*-------------------------------------*/
 for (P_y=0;P_y<8;P_y++)
  {
   Mask=0x80;
   for (P_x=0;P_x<6;P_x++)
    {
     if (Car_Def[Car][P_y] & Mask)
      {
       /* Positionnement de la mémoire video */
       /*------------------------------------*/
       Ram_Video[(x*NB_POINT_CAR_X)+P_x][(y*NB_POINT_CAR_Y)+P_y]=1;

       /* Affichage d'un point noir si necessaire */
       /*-----------------------------------------*/
       //Pset ((x*MUL_X)+P_x,(y*MUL_Y)+P_y);
//       XPutImage (display,win,mygc[0],ima,0,0,(x*NB_POINT_CAR_X*General_Info.size_point_x)+(P_x*General_Info.size_point_x),
//                                              (y*NB_POINT_CAR_Y*General_Info.size_point_y)+(P_y*General_Info.size_point_y),
//                                              General_Info.size_point_x,
//                                              General_Info.size_point_y);
      }
     else
      {
       Ram_Video[(x*NB_POINT_CAR_X)+P_x][(y*NB_POINT_CAR_Y)+P_y]=0;
      }
     Mask >>=1;
    }
  }

 /* Envoi de l'image au serveur X11 */
 /*---------------------------------*/
// XFlush(display);
}

void Cx07::ScrollVideo (void)
{
 qint8 x,y;

 for (x=0 ; x<MAX_X ; x++)
  for (y = (General_Info.Scroll_Min_Y * NB_POINT_CAR_Y);
       y < (General_Info.Scroll_Max_Y * NB_POINT_CAR_Y);
       y++)
  if (y<((General_Info.Scroll_Max_Y - 1)*NB_POINT_CAR_Y))
   Ram_Video [x][y] = Ram_Video[x][y+8];
  else
   Ram_Video [x][y] = 0;
 RefreshVideo ();
}

void Cx07::LineClear (qint8 P_y)
{
 qint8 x,y;

 /* Effacement de la mémoire video */
 /*--------------------------------*/
 for (x=0;x<MAX_X;x++)
  for (y=P_y*NB_POINT_CAR_Y;y<(P_y+1)*NB_POINT_CAR_Y;y++)
   Ram_Video[x][y]=0;

 /* Effacement dans la fenetre video */
 /*----------------------------------*/
// XPutImage (display,win,mygc[7],ima,0,0,0,
//                                        P_y*(NB_POINT_CAR_Y*General_Info.size_point_y),
//                                        (MAX_X*NB_POINT_CAR_X*General_Info.size_point_x),
//                                        NB_POINT_CAR_Y*General_Info.size_point_y);

 /* Envoi de l'image au serveur X11 */
 /*---------------------------------*/
// XFlush(display);
}

/*---------------------------------------------------------------------------*/
void Cx07::Pset (qint8 x, qint8 y)
{
#if AFF_CMD_T6834
 fprintf (stderr,"Pset %d,%d ",x,y);
#endif
 Ram_Video[x][y]=1;
    QPainter painter(LcdImage);
    painter.setPen(  pLCDC->Color_On  );
    painter.drawPoint(x,y);
    painter.end();
// XPutImage (display,win,mygc[0],ima,0,0,x*General_Info.size_point_x,
//                                        y*General_Info.size_point_y,
//                                          General_Info.size_point_x,
//                                          General_Info.size_point_y);
// XFlush(display);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void Cx07::Preset (qint8 x, qint8 y)
{
#if AFF_CMD_T6834
 fprintf (stderr,"Preset %d,%d ",x,y);
#endif
 Ram_Video[x][y]=0;
 QPainter painter(pPC->LcdImage);
 painter.setPen(  pLCDC->Color_Off  );
 painter.drawPoint(x,y);
 painter.end();
// XPutImage (display,win,mygc[7],ima,0,0,x*General_Info.size_point_x,
//                                        y*General_Info.size_point_y,
//                                          General_Info.size_point_x,
//                                          General_Info.size_point_y);
// XFlush(display);
}

void Cx07::Line (qint8 x1, qint8 y1, qint8 x2, qint8 y2)
{
    int next_x = x1, next_y = y1;
    int delta_x = abs(x2 - x1) * 2;
    int delta_y = abs(y2 - y1) * 2;
    int step_x = (x2 < x1) ? -1 : 1;
    int step_y = (y2 < y1) ? -1 : 1;

    if(delta_x > delta_y) {
        int frac = delta_y - delta_x / 2;
        while(next_x != x2) {
            if(frac >= 0) {
                next_y += step_y;
                frac -= delta_x;
            }
            next_x += step_x;
            frac += delta_y;
            Pset(next_x, next_y);
        }
    }
    else {
        int frac = delta_x - delta_y / 2;
        while(next_y != y2) {
            if(frac >= 0) {
                next_x += step_x;
                frac -= delta_y;
            }
            next_y += step_y;
            frac += delta_x;
            Pset(next_x, next_y);
        }
    }
    Pset(x1, y1);
    Pset(x2, y2);
}

void Cx07::Circle(int x, int y, int r)
{
#if 0
// high accuracy
double xlim = sqrt((double)(r * r) / 2);

for(int cx = 0, cy = r; cx <= xlim ; cx++) {
    double d1 = (cx * cx + cy * cy) - r * r;
    double d2 = (cx * cx + (cy - 1) * (cy - 1)) - r * r;
    if(abs(d1) > abs(d2)) {
        cy--;
    }
    draw_point(cx + x, cy + y, 0xff);
    draw_point(cx + x, -cy + y, 0xff);
    draw_point(-cx + x, cy + y, 0xff);
    draw_point(-cx + x, -cy + y, 0xff);
    draw_point(cy + x, cx + y, 0xff);
    draw_point(cy + x, -cx + y, 0xff);
    draw_point(-cy + x, cx + y, 0xff);
    draw_point(-cy + x, -cx + y, 0xff);
}
#else
// high speed
    int cx = 0, cy = r;
    int d = 2 - 2 * r;

    Pset(cx + x, cy + y);
    Pset(cx + x, -cy + y);
    Pset(cy + x, cx + y);
    Pset(-cy + x, cx + y);
    while(1) {
        if(d > -cy) {
            cy--;
            d += 1 - 2 * cy;
        }
        if(d <= cx) {
            cx++;
            d += 1 + 2 * cx;
        }
        if(!cy) {
            return;
        }
        Pset(cx + x, cy + y);
        Pset(-cx + x, cy + y);
        Pset(-cx + x, -cy + y);
        Pset(cx + x, -cy + y);
    }
#endif
}

void Cx07::keyReleaseEvent(QKeyEvent *event)
{
}

void Cx07::keyPressEvent(QKeyEvent *event)
{
    AddKey(event->key());
}

void Cx07::Reset()
{
    CpcXXXX::Reset();
    ((CZ80 *) pCPU)->z80.r16.pc = 0xC3C3;
}
