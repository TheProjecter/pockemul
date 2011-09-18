#include <QTime>

#include "ct6834.h"
#include "Log.h"

extern UINT8 X07_CarDef[][8];

CT6834::CT6834(CPObject *parent)	: CPObject(this)
{
    pPC = (Cx07*)parent;
    Locate_OnOff = 0;
    Loc_X=0;
    Loc_Y=0;
    R5 = 0;
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
              pPC->ScrollVideo ();
              break;

   case 0x09:
              pPC->LineClear (Send_Cmd_T6834[1]);
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
              pPC->Pset (Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
              break;

   case 0x12:
#if AFF_CMD_T6834
              fprintf (stderr,"Preset %d,%d ",Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
#endif
              pPC->Preset (Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
              break;
  case 0x13:	// Peor
              if(Send_Cmd_T6834[1] < 120 && Send_Cmd_T6834[2] < 32) {
                  pPC->Ram_Video[Send_Cmd_T6834[2]][Send_Cmd_T6834[1]] = ~(pPC->Ram_Video[Send_Cmd_T6834[2]][Send_Cmd_T6834[1]]);
              }

   case 0x14: // Line (x1,y1)-(x2,y2)
#if AFF_CMD_T6834
              fprintf (stderr,"Line(%d,%d)-(%d,%d)\n", Send_Cmd_T6834[1],
                                                       Send_Cmd_T6834[2],
                                                       Send_Cmd_T6834[3],
                                                       Send_Cmd_T6834[4]);

#endif
              pPC->Line (Send_Cmd_T6834[1],Send_Cmd_T6834[2],
                    Send_Cmd_T6834[3],Send_Cmd_T6834[4]);
              break;
  case 0x15:
#if AFF_CMD_T6834
             fprintf (stderr,"Circle(%d,%d)-(%d,%d)\n", Send_Cmd_T6834[1],
                                                      Send_Cmd_T6834[2],
                                                      Send_Cmd_T6834[3],
                                                      Send_Cmd_T6834[4]);

#endif
             pPC->Circle(Send_Cmd_T6834[1],Send_Cmd_T6834[2],Send_Cmd_T6834[3]);
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
                pPC->AffCar (Send_Cmd_T6834[1],Send_Cmd_T6834[2],Send_Cmd_T6834[3]);
               }
              else
               {
                pPC->AffCurseur();
               }
#if AFF_CMD_T6834
              fputc ('\n',stderr);
#endif
              break;

   case 0x25: // CursOn

                AddLog (LOG_TEMP,"Curseur ON\n");

              pPC->General_Info.Curseur = 1;
              pPC->AffCurseur ();
              break;

   case 0x26: // CursOff

              AddLog (LOG_TEMP,"Curseur OFF\n");

              pPC->General_Info.Curseur = 0;
              break;

   case 0x2D: // KeyBufferClear
              break;

   case 0x30: // UDKOn
              pPC->General_Info.Aff_Udk = 1;
              AffUdkON (0);
              break;
   case 0x31: // UDKOff
              pPC->General_Info.Aff_Udk = 0;
              pPC->LineClear (3);
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
        pPC->AffCar (x++,3,131);
        for (j=0;j<3;j++)
            pPC->AffCar (x++,3,pPC->General_Info.F_Key[i+(6*Offset)][j]);
    }
}
