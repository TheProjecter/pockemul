#include <QTime>
#include <QPainter>

#include "ct6834.h"
#include "Lcdc.h"
#include "Log.h"

extern UINT8 X07_CarDef[][8];

CT6834::CT6834(CPObject *parent)	: CPObject(this)
{
    pPC = (Cx07*)parent;
    Locate_OnOff = 0;
    Loc_X=0;
    Loc_Y=0;
    R5 = 0;
    First = 1;
}

const CMD_T6834 CT6834::Cmd_T6834[] =
{{   1,   0,"Unknown"},          // 0x00
 {   1,   8,"TimeCall"},         // 0x01
 {   1,   1,"Stick"},            // 0x02
 {   1,   1,"Strig"},            // 0x03
 {   1,   1,"Strig1"},           // 0x04
 {   3,   1,"RamRead"},          // 0x05
 {   4,   0,"RamWrite"},         // 0x06
 {   3,   0,"ScrollSet"},        // 0x07
 {   1,   0,"ScrollExet"},       // 0x08
 {   2,   0,"LineClear"},        // 0x09
 {   9,   0,"TimeSet"},          // 0x0A
 {   1,   0,"CalcDay"},          // 0x0B
 {   9,   0,"AlarmSet"},         // 0x0C
 {   1,   0,"BuzzerOff"},        // 0x0D
 {   1,   0,"BuzzerOn"},         // 0x0E
 {   2, 120,"TrfLine"},          // 0x0F
 {   3,   1,"TestPoint"},        // 0x10
 {   3,   0,"Pset"},             // 0x11
 {   3,   0,"Preset"},           // 0x12
 {   3,   0,"Peor"},             // 0x13
 {   5,   0,"Line"},             // 0x14
 {   4,   0,"Circle"},           // 0x15
 {0x82,   0,"UDKWrite"},         // 0x16
 {   2,0x80,"UDKRead"},          // 0x17
 {   1,   0,"UDKOn"},            // 0x18
 {   1,   0,"UDKOff"},           // 0x19
 {  10,   0,"UDCWrite"},         // 0x1A
 {   2,   8,"UDCRead"},          // 0x1B
 {   1,   0,"UDCInt"},           // 0x1C
 {0x81,   0,"StartPgmWrite"},    // 0x1D
 {0x81,   0,"SPWriteCont"},      // 0x1E
 {   1,   0,"SPOn"},             // 0x1F
 {   1,   0,"SPOff"},            // 0x20
 {   1,0x80,"StartPgmRead"},     // 0x21
 {   1,   1,"OnStat"},           // 0x22
 {   1,   0,"OFFReq"},           // 0x23
 {   4,   0,"Locate"},           // 0x24
// {   38,   0,"Locate"},           // 0x24
 {   1,   0,"CursOn"},           // 0x25
 {   1,   0,"CursOff"},          // 0x26
 {   3,   1,"TestKey"},          // 0x27
 {   2,   1,"TestChr"},          // 0x28
 {   1,   0,"InitSec"},          // 0x29
 {   2,   0,"InitDate"},         // 0x2A
 {   1,   0,"ScrOff"},           // 0x2B
 {   1,   0,"ScrOn"},            // 0x2C
 {   1,   0,"KeyBufferClear"},   // 0x2D
 {   1,   0,"ClsScr"},           // 0x2E
 {   1,   0,"Home"},             // 0x2F
 {   1,   0,"AffUdkOn"},         // 0x30
 {   1,   0,"AffUDKOff"},        // 0x31
 {   1,   0,"RepateKeyOn"},      // 0x32
 {   1,   0,"RepateKeyOff"},     // 0x33
 {   1,   0,"UDK=KANA"},         // 0x34
 {0x82,   0,"UdkContWrite"},     // 0x35
 {   1,   8,"AlarmRead"},        // 0x36
 {   1,   1,"BuzzZero"},         // 0x37
 {   1,   0,"ClickOff"},         // 0x38
 {   1,   0,"ClickOn"},          // 0x39
 {   1,   0,"LocateClose"},      // 0x3A
 {   1,   0,"KeybOn"},           // 0x3B
 {   1,   0,"KeybOff"},          // 0x3C
 {   1,   0,"ExecStartPgm"},     // 0x3D
 {   1,   0,"UnexecStartPgm"},   // 0x3E
 {   1,   0,"Sleep"},            // 0x3F
 {   1,   0,"UDKInit"},          // 0x40
 {   9,   0,"AffUDC"},           // 0x41
 {   1,   8,"ReadCar"},          // 0x42
 {   3,   2,"ScanR"},            // 0x43
 {   3,   2,"ScanL"},            // 0x44
 {   1,   1,"TimeChk"},          // 0x45
 {   1,   1,"AlmChk"}};          // 0x46

const qint8 CT6834::Date[]={128,192,224,240,248,252,254,255};

int CT6834::InitReponseT6834 (UINT8 Ordre, UINT8 *Rsp, PorT_FX *Port)
{

 int    Lng_rsp;
 qint16   Adresse;
 UINT8  i;

 Lng_rsp = Cmd_T6834[Ordre].lng_rsp;

 switch (Ordre & 0x7F)
  {
   case 0x00: //lng_rsp = Cmd_T6834[Ordre].lng_rsp;
              break;

   case 0x01:
              {
                QDateTime my_t = QDateTime::currentDateTime();


               Rsp[0] = ((my_t.date().year()-100)>>8) & 0xFF;
               Rsp[1] = my_t.date().year() & 0xFF;
               Rsp[2] = my_t.date().month()+1;
               Rsp[3] = my_t.date().day();
               Rsp[4] = my_t.date().dayOfWeek();
               Rsp[5] = my_t.time().hour();
               Rsp[6] = my_t.time().minute();
               Rsp[7] = my_t.time().second();
              fprintf (stderr,"TimeCall:%d/%d/%d (%d) %d:%d:%d\n",(Rsp [0]<<8)+
                                                                   Rsp [1],
                                                                   Rsp [2],
                                                                   Rsp [3],
                                                                   Rsp [4],
                                                                   Rsp [5],
                                                                   Rsp [6],
                                                                   Rsp [7]);
              }
              break;

   case 0x02:

            AddLog (LOG_TEMP,tr("Stick = %1").arg(pPC->General_Info.Stick,2,16,QChar('0')));

              Rsp[0] = pPC->General_Info.Stick;
              break;
   case 0x03:

              AddLog (LOG_TEMP,tr("Strig = %1").arg(pPC->General_Info.Strig,2,16,QChar('0')));

              Rsp[0] = pPC->General_Info.Strig;
              break;
   case 0x04:

              AddLog(LOG_TEMP,tr("Strig1 = %1").arg(pPC->General_Info.Strig1,2,16,QChar('0')));

              Rsp[0] = pPC->General_Info.Strig1;
              break;

   case 0x05: Adresse = Send_Cmd_T6834[1] + ( Send_Cmd_T6834[2] << 8);
#if AFF_CMD_T6834
              fprintf (stderr,"Adresse = %04X\n",Adresse);
#endif
              if (!R5) {Rsp[0]=0x0A;R5++;}
              else     {Rsp[0]='0';}
              break;

   case 0x06:
              Adresse = Send_Cmd_T6834[2] + ( Send_Cmd_T6834[3] << 8);
#if AFF_CMD_T6834
              fprintf (stderr,"Adresse = %04X Data:%02X\n",Adresse,Send_Cmd_T6834[1]);
#endif
              break;

   case 0x07: // Scroll Set
#if AFF_CMD_T6834
                fprintf (stderr,"Scroll_Set[%d,%d]\n",Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
#endif
                pPC->General_Info.Scroll_Min_Y = Send_Cmd_T6834[1];
                pPC->General_Info.Scroll_Max_Y = Send_Cmd_T6834[2]+1;
              break;

   case 0x08:
              fputc ('\n',stderr);
              ScrollVideo ();
              break;

   case 0x09:
              LineClear (Send_Cmd_T6834[1]);
              break;

   case 0x0A:
              fprintf (stderr,"Time set:%d/%d/%d (%d) %d:%d:%d\n",(Send_Cmd_T6834[1]<<8)+
                                                                   Send_Cmd_T6834[2],
                                                                   Send_Cmd_T6834[3],
                                                                   Send_Cmd_T6834[4],
                                                                   Send_Cmd_T6834[5],
                                                                   Send_Cmd_T6834[6],
                                                                   Send_Cmd_T6834[7],
                                                                   Send_Cmd_T6834[8]);
   case 0x0B: // Calc Day (not used)
              break;

   case 0x0C: // AlarmSet (not used)
              fprintf (stderr,"AlarmSet:[%d %d]%d/%d/%d (%d) %d:%d:%d\n",
                                                                   Send_Cmd_T6834[1],
                                                                   Send_Cmd_T6834[2],
                                                                  (Send_Cmd_T6834[1]<<8)+
                                                                   Send_Cmd_T6834[2],
                                                                   Send_Cmd_T6834[3],
                                                                   Send_Cmd_T6834[4],
                                                                   Send_Cmd_T6834[5],
                                                                   Send_Cmd_T6834[6],
                                                                   Send_Cmd_T6834[7],
                                                                   Send_Cmd_T6834[8]);
              break;

   case 0x11:
#if AFF_CMD_T6834
              fprintf (stderr,"Pset %d,%d ",Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
#endif
              Pset (Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
              break;

   case 0x12:
#if AFF_CMD_T6834
              fprintf (stderr,"Preset %d,%d ",Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
#endif
              Preset (Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
              break;
  case 0x13:	// Peor
              if(Send_Cmd_T6834[1] < 120 && Send_Cmd_T6834[2] < 32) {
                  Ram_Video[Send_Cmd_T6834[2]][Send_Cmd_T6834[1]] = ~(Ram_Video[Send_Cmd_T6834[2]][Send_Cmd_T6834[1]]);
              }

   case 0x14: // Line (x1,y1)-(x2,y2)
#if AFF_CMD_T6834
              fprintf (stderr,"Line(%d,%d)-(%d,%d)\n", Send_Cmd_T6834[1],
                                                       Send_Cmd_T6834[2],
                                                       Send_Cmd_T6834[3],
                                                       Send_Cmd_T6834[4]);

#endif
              Line (Send_Cmd_T6834[1],Send_Cmd_T6834[2],
                    Send_Cmd_T6834[3],Send_Cmd_T6834[4]);
              break;
  case 0x15:
#if AFF_CMD_T6834
             fprintf (stderr,"Circle(%d,%d)-(%d,%d)\n", Send_Cmd_T6834[1],
                                                      Send_Cmd_T6834[2],
                                                      Send_Cmd_T6834[3],
                                                      Send_Cmd_T6834[4]);

#endif
             Circle(Send_Cmd_T6834[1],Send_Cmd_T6834[2],Send_Cmd_T6834[3]);
             break;
   case 0x16: // UDKWrite
//#if AFF_CMD_T6834
                 fprintf (stderr,"UDKWrite[%d] ", Send_Cmd_T6834[1]);
                 for (i=2;(i<50) && (Send_Cmd_T6834[i]);i++)
                  if (Send_Cmd_T6834[i]>=' ') fputc (Send_Cmd_T6834[i],stderr);
                  else fprintf (stderr," $%02X",Send_Cmd_T6834[i]);
                 fputc ('\n',stderr);
//#endif
                 strcpy (pPC->General_Info.F_Key [Send_Cmd_T6834[1]-1],(char*)&Send_Cmd_T6834[2]);
                 break;
  case 0x17:	// UDKRead
                  //val = Send_Cmd_T6834[1];
                  for(i = 0; i < 42; i++) {
                      UINT8 code = pPC->General_Info.F_Key [Send_Cmd_T6834[1]-1][i];
                      Rsp[i] = code;
                      if(!code) {
                          return (i+1);
                      }
                  }
                  return (i+1);
                  break;
   case 0x18: // UDKOn
   case 0x19: // UDKOff
                 break;

   case 0x1A: // UDCWrite
#if AFF_CMD_T6834
              fprintf (stderr,"UDCWrite[%02X]\n",Send_Cmd_T6834[1]);
#endif
              for (i=0;i<8;i++)
               X07_CarDef [Send_Cmd_T6834[1]][i] = Send_Cmd_T6834[2+i];
              break;

   case 0x1B: // UDCRead
#if AFF_CMD_T6834
              fprintf (stderr,"UDCRead[%02X]\n",Send_Cmd_T6834[1]);
#endif
              for (i=0;i<8;i++)
               Rsp[i] = X07_CarDef [Send_Cmd_T6834[1]][i];
              break;

   case 0x1F: // SPOn
   case 0x20: // SPOff
                 break;

   case 0x22: Rsp[0]=0x04; // 0x41
              break;
   case 0x23: // Turn OFF
              mainwindow->saveAll =NO;
              pPC->TurnOFF();
              break;

   case 0x24:

              AddLog (LOG_TEMP,tr("Locate %1,%2 ").arg(Send_Cmd_T6834[1],2,16,QChar('0')).arg(Send_Cmd_T6834[2],2,16,QChar('0')));

              if ((Send_Cmd_T6834[1] == 0) && (Send_Cmd_T6834[2]==pPC->General_Info.Curs_Y+1))
               fputc ('\n',stderr);
              if ((Loc_X == Send_Cmd_T6834[1]) && (Loc_Y == Send_Cmd_T6834[2]))
                {
                AddLog (LOG_TEMP,"Locate_OnOff = 0\n");
                Locate_OnOff = 0;
                }
               else
                {
                AddLog (LOG_TEMP,"Locate_OnOff = 1\n");
                Locate_OnOff = 1;
                }
              Loc_X = pPC->General_Info.Curs_X = Send_Cmd_T6834[1];
              Loc_Y = pPC->General_Info.Curs_Y = Send_Cmd_T6834[2];
              if (Send_Cmd_T6834[3])
               {
                fputc (Send_Cmd_T6834[3],stderr);
                AffCar (Send_Cmd_T6834[1],Send_Cmd_T6834[2],Send_Cmd_T6834[3]);
               }
              else
               {
                //pPC->AffCurseur();
               }
#if AFF_CMD_T6834
              fputc ('\n',stderr);
#endif
              break;

   case 0x25: // CursOn

                AddLog (LOG_TEMP,"Curseur ON\n");

              pPC->General_Info.Curseur = true;
              //pPC->AffCurseur ();
              break;

   case 0x26: // CursOff

              AddLog (LOG_TEMP,"Curseur OFF\n");

              pPC->General_Info.Curseur = false;
              break;

   case 0x2D: // KeyBufferClear
              break;

   case 0x30: // UDKOn
              pPC->General_Info.Aff_Udk = 1;
              AffUdkON (0);
              break;
   case 0x31: // UDKOff
              pPC->General_Info.Aff_Udk = 0;
              LineClear (3);
              break;

   case 0x3b: // KeybOn
   case 0x3c: // KeybOff
                 break;

   case 0x3F: // Sleep
#if AFF_CMD_T6834
              AddLog (LOG_TMP,"Sleep\n");
#endif
                 //pPC->pCPU->Reg_Xo7.Trace=1;
                 break;

   default:    fprintf (stderr,"(%s) \n",Cmd_T6834[Send_Cmd_T6834[0]].Str_Cmd);
               break;

  }
 return (Lng_rsp);
}

void CT6834::AffUdkON (bool shift)
{
    qint8 Offset,i,j;
    qint8 x=0;

    Offset = (shift)?1:0;
    for (i=0;i<5;i++)
    {
        AffCar (x++,3,131);
        for (j=0;j<3;j++)
            AffCar (x++,3,pPC->General_Info.F_Key[i+(6*Offset)][j]);
    }
}

void CT6834::ClrScr (void)
{
    memset(&Ram_Video,0,sizeof(Ram_Video));
    RefreshVideo ();
}

void CT6834::RefreshVideo (void)
{
    int x;
    int y;
    int ColorIndex;

    //AffCurseur ();
    QPainter painter(pPC->LcdImage);
    for (x=0;x<120;x++)
        for (y=0;y<32;y++)
        {
            if (pPC->General_Info.Curseur &&
                cursorTimer.elapsed()>500 &&
                pPC->General_Info.Curs_X == (x/6) &&
                pPC->General_Info.Curs_Y == (y/8)) {

                painter.setPen( ((y == pPC->General_Info.Curs_Y * 8 + 6) &&(x!=pPC->General_Info.Curs_X*6+5))? pPC->pLCDC->Color_On : pPC->pLCDC->Color_Off );
                painter.drawPoint(x,y);
            }
            else {
                QColor col = (Ram_Video[x][y])?pPC->pLCDC->Color_On : pPC->pLCDC->Color_Off;

                painter.setPen(  col  );
                painter.drawPoint(x,y);
            }
        }
    if (pPC->General_Info.Curseur && cursorTimer.elapsed()>1000) cursorTimer.restart();
    painter.end();

    Refresh_Display = true;
}

void CT6834::AffCurseur (void)
{
    if (!First)
    {
        if (pPC->General_Info.Curseur)
        {
            UINT8 y = ((Loc_Y+1) * pPC->General_Info.size_point_y * NB_POINT_CAR_Y) - pPC->General_Info.size_point_y;
            UINT8 x =   Loc_X    * pPC->General_Info.size_point_x * NB_POINT_CAR_X;

            for (int i=0;i<=NB_POINT_CAR_X*pPC->General_Info.size_point_x;i++)
                Ram_Video[x+i][y-1] = 1;

        }
    }
    else First = 0;

    Loc_X = pPC->General_Info.Curs_X;
    Loc_Y = pPC->General_Info.Curs_Y;
    if (pPC->General_Info.Curseur)
    {
        UINT8 y = ((Loc_Y+1) * pPC->General_Info.size_point_y * NB_POINT_CAR_Y) - pPC->General_Info.size_point_y;
        UINT8 x =   Loc_X    * pPC->General_Info.size_point_x * NB_POINT_CAR_X;
        for (int i=0;i<=NB_POINT_CAR_X*pPC->General_Info.size_point_x;i++)
            Ram_Video[x+i][y-1] = 1;
    }
}

void CT6834::AffCar(UINT8 x, UINT8 y, UINT8 Car)
{
    int P_x,P_y;
    UINT8 Mask;
    int offsetX = x*NB_POINT_CAR_X;
    int offsetY = y*NB_POINT_CAR_Y;
    /* Dessin du caractere point par point */
    /*-------------------------------------*/

    AddLog (LOG_TEMP,tr("Draw char (%1) at %2,%3").arg(Car,2,16,QChar('0')).arg(x).arg(y));
    for (P_y=0;P_y<8;P_y++)
    {
        Mask=0x80;


        for (P_x=0;P_x<6;P_x++)
        {
            int color = ( (X07_CarDef[Car][P_y] & Mask) ? 1 : 0);

            /* Positionnement de la mémoire video */
            /*------------------------------------*/
            Ram_Video[offsetX+P_x][offsetY+P_y] = color;

            Mask >>=1;
        }
    }


}

void CT6834::ScrollVideo (void)
{
    UINT8 x,y;

    for (x=0 ; x<MAX_X ; x++)
        for (y = (pPC->General_Info.Scroll_Min_Y * NB_POINT_CAR_Y);
             y < (pPC->General_Info.Scroll_Max_Y * NB_POINT_CAR_Y);
             y++)
            if (y<((pPC->General_Info.Scroll_Max_Y - 1)*NB_POINT_CAR_Y))
                Ram_Video [x][y] = Ram_Video[x][y+8];
            else
                Ram_Video [x][y] = 0;
    RefreshVideo ();
}

void CT6834::LineClear (UINT8 P_y)
{
 UINT8 x,y;

 /* Effacement de la mémoire video */
 /*--------------------------------*/
 for (x=0;x<MAX_X;x++)
  for (y=P_y*NB_POINT_CAR_Y;y<(P_y+1)*NB_POINT_CAR_Y;y++)
   Ram_Video[x][y]=0;
}

/*---------------------------------------------------------------------------*/
void CT6834::Pset (UINT8 x, UINT8 y)
{
#if AFF_CMD_T6834
    fprintf (stderr,"Pset %d,%d ",x,y);
#endif
    Ram_Video[x][y]=1;

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void CT6834::Preset (UINT8 x, UINT8 y)
{
#if AFF_CMD_T6834
    fprintf (stderr,"Preset %d,%d ",x,y);
#endif
    Ram_Video[x][y]=0;
}

void CT6834::Line (UINT8 x1, UINT8 y1, UINT8 x2, UINT8 y2)
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

void CT6834::Circle(int x, int y, int r)
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
