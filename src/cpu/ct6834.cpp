
#include <math.h>

#include <QTime>
#include <QPainter>
#include <QKeyEvent>

#include "ct6834.h"
#include "Lcdc.h"
#include "Log.h"
#include "cpu.h"
#include "cx07.h"
#include "cx07char.h"
#include "Keyb.h"


CT6834::CT6834(CPObject *parent)	: CPObject(this)
{
    pPC = (Cx07*)parent;
    Locate_OnOff = 0;
    Loc_X=0;
    Loc_Y=0;
    R5 = 0;
    First = 1;
    curOnOff = false;

    kana=graph=shift=ctrl = false;

}

CT6834::~CT6834(){
    free(mem);
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


const int CT6834::udk_ofs[12] = {
    0, 42, 84, 126, 168, 210, 256, 298, 340, 382, 424, 466
};

const int CT6834::udk_size[12] = {
    42, 42, 42, 42, 42, 46, 42, 42, 42, 42, 42, 46
};
//extern FILE */*fp_tm*/p3;
int CT6834::InitReponseT6834 (UINT8 Ordre, UINT8 *Rsp, PorT_FX *Port)
{
    int    Lng_rsp;
    DWORD  Adresse;
    UINT8  i;

    Lng_rsp = Cmd_T6834[Ordre].lng_rsp;

//    if (fp_tmp3)  fprintf(fp_tmp3,"ORDER %02X\n",Ordre);
 switch (Ordre & 0x7F)
  {
   case 0x00: //lng_rsp = Cmd_T6834[Ordre].lng_rsp;
              break;

   case 0x01: {
                  QDateTime my_t = QDateTime::currentDateTime();
                  Rsp[0] = ((my_t.date().year()-100)>>8) & 0xFF;
                  Rsp[1] = my_t.date().year() & 0xFF;
                  Rsp[2] = my_t.date().month();
                  Rsp[3] = my_t.date().day();
                  Rsp[4] = ~(((0x01 << (7 - my_t.date().dayOfWeek())) - 1) & 0xff);
                  Rsp[5] = my_t.time().hour();
                  Rsp[6] = my_t.time().minute();
                  Rsp[7] = my_t.time().second();
              }
              break;

   case 0x02: AddLog (LOG_TEMP,tr("Stick = %1").arg(General_Info.Stick,2,16,QChar('0')));
     switch (General_Info.Stick) {
         case 0x00: Rsp[0] = 0x30; break;
         case 0x01: Rsp[0] = 0x31; break;
         case 0x03: Rsp[0] = 0x32; break;
         case 0x02: Rsp[0] = 0x33; break;
         case 0x06: Rsp[0] = 0x34; break;
         case 0x04: Rsp[0] = 0x35; break;
         case 0x0C: Rsp[0] = 0x36; break;
         case 0x08: Rsp[0] = 0x37; break;
         case 0x09: Rsp[0] = 0x38; break;
     default: Rsp[0] = 0x30; break;
     }
              break;

   case 0x03: AddLog (LOG_TEMP,tr("Strig = %1").arg(General_Info.Strig,2,16,QChar('0')));
              Rsp[0] = General_Info.Strig;
              break;

   case 0x04: AddLog(LOG_TEMP,tr("Strig1 = %1").arg(General_Info.Strig1,2,16,QChar('0')));
              Rsp[0] = General_Info.Strig1;
              break;

   case 0x05: // RamRead
              Adresse = Send_Cmd_T6834[1] + ( Send_Cmd_T6834[2] << 8);
              Adresse |= 0xC000;
              if (pPC->pCPU->fp_log) fprintf(pPC->pCPU->fp_log,"Lecture adr %04X",Adresse);
              AddLog(LOG_CANON,tr("Lecture adr %1").arg(Adresse,4,16,QChar('0')));
              if(Adresse == 0xc00e) {
                  Rsp[0] = 0x0A;
              }
              else
              if(Adresse == 0xd000) {
                  Rsp[0] = 0x30;
              }
              else
              {

                  Rsp[0] = mem[Adresse - 0xC000];
              }
              break;

   case 0x06: // RamWrite
     Adresse = Send_Cmd_T6834[2] + ( Send_Cmd_T6834[3] << 8);
//     if (fp_tmp3) fprintf(fp_tmp3,"Ecriture adr %04X : %02X\n",Adresse,Send_Cmd_T6834[1]);
              if (pPC->pCPU->fp_log) fprintf(pPC->pCPU->fp_log,"Ecriture adr %04X : %02X",Adresse,Send_Cmd_T6834[1]);
              AddLog(LOG_CANON,tr("Ecriture adr [%1]=%2").arg(Adresse,4,16,QChar('0')).arg(Send_Cmd_T6834[1],2,16,QChar('0')));
              Adresse |= 0xC000;
              mem[Adresse - 0xC000] = Send_Cmd_T6834[1];
              if(Adresse == 0xc00e) {
                  AddLog(LOG_TEMP,tr("RAM WRITE C00E %1").arg(Rsp[0],2,16,QChar('0')));
              }
              break;

   case 0x07: // Scroll Set
                General_Info.Scroll_Min_Y = Send_Cmd_T6834[1];
                General_Info.Scroll_Max_Y = Send_Cmd_T6834[2]+1;
              break;

   case 0x08:
              pPC->pLCDC->redraw = true;
              ScrollVideo ();
              break;

   case 0x09:
              pPC->pLCDC->redraw = true;
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
  case 0x0d:
  case 0x0e:
  case 0x0f:/*
  {
      UINT8 line = m_in.data[m_in.read++];
      for(int i = 0; i < 120; i++)
          m_out.data[m_out.write++] = (line < 32) ? m_lcd_map[line][i] : 0;
  } */
      break;

  case 0x10:    //POINT
            return (Ram_Video[Send_Cmd_T6834[1]][Send_Cmd_T6834[2]] ? 0: 0xff);
            break;

   case 0x11: pPC->pLCDC->redraw = true;
              Pset (Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
              break;

   case 0x12: pPC->pLCDC->redraw = true;
              Preset (Send_Cmd_T6834[1],Send_Cmd_T6834[2]);
              break;
  case 0x13:	// Peor
              pPC->pLCDC->redraw = true;
              if(Send_Cmd_T6834[1] < 120 && Send_Cmd_T6834[2] < 32) {
                  Ram_Video[Send_Cmd_T6834[1]][Send_Cmd_T6834[2]] = ~(Ram_Video[Send_Cmd_T6834[1]][Send_Cmd_T6834[2]]);
              }
              break;

   case 0x14: // Line (x1,y1)-(x2,y2)
              pPC->pLCDC->redraw = true;
              Line (Send_Cmd_T6834[1],Send_Cmd_T6834[2],
                    Send_Cmd_T6834[3],Send_Cmd_T6834[4]);
              break;
  case 0x15: pPC->pLCDC->redraw = true;
             Circle(Send_Cmd_T6834[1],Send_Cmd_T6834[2],Send_Cmd_T6834[3]);
             break;

  case 0x16: // UDKWrite
      for(i = 0; i < udk_size[Send_Cmd_T6834[1]]; i++) {
          mem[0x800+udk_ofs[Send_Cmd_T6834[1]] + i] = Send_Cmd_T6834[2+i];
      }
      break;

  case 0x17:	// UDKRead
                  //val = Send_Cmd_T6834[1];
                  for(i = 0; i < 42; i++) {
                      UINT8 code = mem[0x800+udk_ofs[Send_Cmd_T6834[1]] + i];//General_Info.F_Key [Send_Cmd_T6834[1]-1][i];
                      Rsp[i] = code;
                      if(!code) {
                          return (i+1);
                      }
                  }
                  return (i+1);
                  break;
   case 0x18: // UDKOn
   case 0x19: // UDKOff
                 pPC->pLCDC->redraw = true;
                 break;

   case 0x1A: // UDCWrite
              for (i=0;i<8;i++)
               X07_CarDef [Send_Cmd_T6834[1]][i] = Send_Cmd_T6834[2+i];
              break;

   case 0x1B: // UDCRead
                for (i=0;i<8;i++)
                    Rsp[i] = X07_CarDef [Send_Cmd_T6834[1]][i];
              break;

  case 0x1d:
  case 0x1e:
      for(i = 0; (i < 0x200 && Send_Cmd_T6834[i+1]); i++) {
          mem[0xE00+i] = Send_Cmd_T6834[12+i];
      }

      break;
   case 0x1F: // SPOn
   case 0x20: // SPOff
                 break;
   case 0x21: // SP Read  0xCE00 -> 0xCFFF
      for(i = 0; i < 0x200; i++) {
          UINT8 code = mem[0xE00+i];
          Rsp[i] = code;
          if(!code) {
              return (i+1);
          }
      }
      return (i+1);
      break;

   case 0x22: Rsp[0]=mem[0x0006];//0x04; //  | (m_sleep<<6) | m_warm_start; // MEM 0xC006
                // teste le bit 6 de A. tenter de le mettre à 1

              break;
   case 0x23: // Turn OFF
      pPC->pLCDC->redraw = true;
      TurnOFF();

              break;

   case 0x24:
              pPC->pLCDC->redraw = true;
              AddLog (LOG_TEMP,tr("Locate %1,%2 ").arg(Send_Cmd_T6834[1],2,16,QChar('0')).arg(Send_Cmd_T6834[2],2,16,QChar('0')));

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
              Loc_X = General_Info.Curs_X = Send_Cmd_T6834[1];
              Loc_Y = General_Info.Curs_Y = Send_Cmd_T6834[2];
              if (Send_Cmd_T6834[3])
              {
                  AffCar (Send_Cmd_T6834[1],Send_Cmd_T6834[2],Send_Cmd_T6834[3]);
              }
              else
              {
                  //pPC->AffCurseur();
              }

              break;

   case 0x25: // CursOn
                pPC->pLCDC->redraw = true;
                AddLog (LOG_TEMP,"Curseur ON\n");

              General_Info.Curseur = true;
              break;

   case 0x26: // CursOff
            pPC->pLCDC->redraw = true;
              AddLog (LOG_TEMP,"Curseur OFF\n");

              General_Info.Curseur = false;
              break;
  case 0x27 :
      /*
  {
      static const char *const lines[] = {"S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8", "BZ", "A1"};
      UINT16 matrix;
      UINT8 data = 0;
      matrix = m_in.data[m_in.read++];
      matrix |= (m_in.data[m_in.read++] << 8);

      for (int i=0 ;i<10; i++)
          if (matrix & (1<<i))
              data |= input_port_read(machine, lines[i]);

      m_out.data[m_out.write++] = data;
  }
  */
      break;
  case 0x28:	//test chr
//      {
//          UINT8 idx = kb_get_index(m_in.data[m_in.read++]);
//          m_out.data[m_out.write++] = (input_port_read(machine, x07_keycodes[idx].tag) & x07_keycodes[idx].mask) ? 0x00 : 0xff;
//      }
      break;

  case 0x29:	//init sec
  case 0x2a:	//init date
      break;

   case 0x2b: // LCD OFF
                pPC->pLCDC->redraw = true;
                General_Info.LcdOn = false;
                break;
   case 0x2c: // LCD ON
                pPC->pLCDC->redraw = true;
                General_Info.LcdOn = true;
                break;
   case 0x2D: // KeyBufferClear
      //memset(m_t6834_ram + 0x400, 0, 0x100);
              Clavier.clear();
              break;

   case 0x30: // UDKOn
                pPC->pLCDC->redraw = true;
              General_Info.Aff_Udk = 1;
              AffUdkON (0);
              break;
   case 0x31: // UDKOff
                pPC->pLCDC->redraw = true;
              General_Info.Aff_Udk = 0;
              LineClear (3);
              break;
    case 0x37: Rsp[0]=0;
              break;
   case 0x3b: // KeybOn
                General_Info.EnableKeyEntry = true;
                break;
   case 0x3c: // KeybOff
                General_Info.EnableKeyEntry = false;
                 break;

   case 0x3F: // Sleep
                pPC->pLCDC->redraw = true;
              AddLog (LOG_TEMP,"Sleep\n");
              mem[0x0006] |= 0x41;      // bit 0 and 6 to 1 for OFF and SLEEP
              General_Info.LcdOn = false;
                pPC->TurnOFF();
                 break;
  case 0x40:
        initUdk();
      break;
  case 0x41:	//char write
      {
//          for(int cy = 0; cy < 8; cy++)
//          {
//              UINT8 cl = m_in.data[m_in.read++];

//              for(int cx = 0; cx < 6; cx++)
//                  m_lcd_map[m_cursor.y * 8 + cy][m_cursor.x * 6 + cx] = (cl & (0x80>>cx)) ? 1 : 0;
//          }
      }
      break;

  case 0x42: //char read
      {
//          for(int cy = 0; cy < 8; cy++)
//          {
//              UINT8 cl = 0x00;

//              for(int cx = 0; cx < 6; cx++)
//                  cl |= (m_lcd_map[m_cursor.y * 8 + cy][m_cursor.x * 6 + cx] != 0) ? (1<<(7-cx)) : 0;

//              m_out.data[m_out.write++] = cl;
//          }
      }
      break;
  case 0x43:	// ScanR
  case 0x44:	// ScanL
      Rsp[0]=0;
      Rsp[1]=0;
      break;
  case 0x45:	// TimeChk
  case 0x46:	// AlmChk
      Rsp[0]=0;
      break;

   default:    fprintf (stderr,"(%s) \n",Cmd_T6834[Send_Cmd_T6834[0]].Str_Cmd);
               break;

  }
 return (Lng_rsp);
}

void CT6834::initUdk() {
    strcpy((char*)&mem[0x800+udk_ofs[0]],"tim?TIME$\r");
    strcpy((char*)&mem[0x800+udk_ofs[1]],"cldCLOAD");
    strcpy((char*)&mem[0x800+udk_ofs[2]],"locLOCATE");
    strcpy((char*)&mem[0x800+udk_ofs[3]],"lstLIST");
    strcpy((char*)&mem[0x800+udk_ofs[4]],"runRUN\r");
    strcpy((char*)&mem[0x800+udk_ofs[5]],"nul");
    strcpy((char*)&mem[0x800+udk_ofs[6]],"dat?DATE$\r");
    strcpy((char*)&mem[0x800+udk_ofs[7]],"csaCSAVE");
    strcpy((char*)&mem[0x800+udk_ofs[8]],"prtPRINT");
    strcpy((char*)&mem[0x800+udk_ofs[9]],"slpSLEEP");
    strcpy((char*)&mem[0x800+udk_ofs[10]],"cntCONT\r");
    strcpy((char*)&mem[0x800+udk_ofs[11]],"nul");
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
            AffCar (x++,3,mem[0x800+udk_ofs[i+6*Offset]+j]);//General_Info.F_Key[i+(6*Offset)][j]);
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
    if (cursorTimer.elapsed()>500) {
        curOnOff = !curOnOff;
        pPC->pLCDC->redraw = true;
        cursorTimer.restart();
    }
    if (!pPC->pLCDC->redraw) return;
    if (!pPC->LcdImage) return;
    QPainter painter;
    painter.begin(pPC->LcdImage);
    for (x=0;x<120;x++)
        for (y=0;y<32;y++)
        {
            if (General_Info.Curseur &&
                curOnOff &&
                General_Info.Curs_X == (x/6) &&
                General_Info.Curs_Y == (y/8)) {

                painter.setPen( (General_Info.LcdOn &&
                                 (y == General_Info.Curs_Y * 8 + 6) &&
                                 (x!=General_Info.Curs_X*6+5)) ?
                                    pPC->pLCDC->Color_On :
                                    pPC->pLCDC->Color_Off );
                painter.drawPoint(x,y);
            }
            else {
                QColor col = (General_Info.LcdOn && Ram_Video[x][y])?
                                    pPC->pLCDC->Color_On : pPC->pLCDC->Color_Off;

                painter.setPen(  col  );
                painter.drawPoint(x,y);
            }
        }
//    if (General_Info.Curseur && cursorTimer.elapsed()>1000) cursorTimer.restart();
    painter.end();

    pPC->pLCDC->Refresh = true;
    Refresh_Display = true;
}

void CT6834::AffCurseur (void)
{
    if (!First)
    {
        if (General_Info.Curseur)
        {
            UINT8 y = ((Loc_Y+1) * General_Info.size_point_y * NB_POINT_CAR_Y) - General_Info.size_point_y;
            UINT8 x =   Loc_X    * General_Info.size_point_x * NB_POINT_CAR_X;

            for (int i=0;i<=NB_POINT_CAR_X*General_Info.size_point_x;i++)
                Ram_Video[x+i][y-1] = 1;

        }
    }
    else First = 0;

    Loc_X = General_Info.Curs_X;
    Loc_Y = General_Info.Curs_Y;
    if (General_Info.Curseur)
    {
        UINT8 y = ((Loc_Y+1) * General_Info.size_point_y * NB_POINT_CAR_Y) - General_Info.size_point_y;
        UINT8 x =   Loc_X    * General_Info.size_point_x * NB_POINT_CAR_X;
        for (int i=0;i<=NB_POINT_CAR_X*General_Info.size_point_x;i++)
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
        for (y = (General_Info.Scroll_Min_Y * NB_POINT_CAR_Y);
             y < (General_Info.Scroll_Max_Y * NB_POINT_CAR_Y);
             y++)
            if (y<((General_Info.Scroll_Max_Y - 1)*NB_POINT_CAR_Y))
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
void CT6834::Pset (int x, int y)
{
#if AFF_CMD_T6834
    fprintf (stderr,"Pset %d,%d ",x,y);
#endif
    if ( x>=0 && x <120 && y>=0 && y<32)
        Ram_Video[x][y]=1;

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void CT6834::Preset (int x, int y)
{
#if AFF_CMD_T6834
    fprintf (stderr,"Preset %d,%d ",x,y);
#endif
    if ( x>=0 && x <120 && y>=0 && y<32)
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
#if 1
// high accuracy
double xlim = sqrt((double)(r * r) / 2);

for(int cx = 0, cy = r; cx <= xlim+1 ; cx++) {
    double d1 = (cx * cx + cy * cy) - r * r;
    double d2 = (cx * cx + (cy - 1) * (cy - 1)) - r * r;
    if(abs(d1) > abs(d2)) {
        cy--;
    }
    Pset(cx + x, cy + y);
    Pset(cx + x, -cy + y);
    Pset(-cx + x, cy + y);
    Pset(-cx + x, -cy + y);
    Pset(cy + x, cx + y);
    Pset(cy + x, -cx + y);
    Pset(-cy + x, cx + y);
    Pset(-cy + x, -cx + y);
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


void CT6834::Load_Internal(QXmlStreamReader *xmlIn)
{
    if (xmlIn->readNextStartElement()) {
        if ( (xmlIn->name()=="cpu") &&
             (xmlIn->attributes().value("model").toString() == "t6834")) {
//            QByteArray ba_reg = QByteArray::fromBase64(xmlIn->attributes().value("registers").toString().toAscii());
//            memcpy((char *) &r,ba_reg.data(),sizeof(r));
            QByteArray ba_mem = QByteArray::fromBase64(xmlIn->attributes().value("Mem").toString().toAscii());
            memcpy((char *)mem,ba_mem.data(),ba_mem.size());
            QByteArray ba_lcd = QByteArray::fromBase64(xmlIn->attributes().value("Lcd").toString().toAscii());
            memcpy((char *)&Ram_Video,ba_lcd.data(),ba_lcd.size());
        }
        xmlIn->skipCurrentElement();
    }
}

void CT6834::save_internal(QXmlStreamWriter *xmlOut)
{
    xmlOut->writeStartElement("cpu");
        xmlOut->writeAttribute("model","t6834");
//        QByteArray ba_reg((char*)&r,sizeof(r));
//        xmlOut->writeAttribute("registers",ba_reg.toBase64());
        QByteArray ba_mem((char*)mem,0x2200*sizeof(UINT8));
        xmlOut->writeAttribute("Mem",ba_mem.toBase64());
        QByteArray ba_lcd((char*)&Ram_Video,sizeof(Ram_Video));
        xmlOut->writeAttribute("Lcd",ba_lcd.toBase64());
    xmlOut->writeEndElement();
}

bool CT6834::init()
{
    mem=(UINT8 *)malloc(0x2200*sizeof(UINT8));

    initUdk();

    General_Info.Scroll_Min_Y = 0;
    General_Info.Scroll_Max_Y = 4;
    General_Info.size_point_x = 1;
    General_Info.size_point_y = 1;
    General_Info.Curs_X       = 0;
    General_Info.Curs_Y       = 0;
    General_Info.Curseur      = false;
    General_Info.Aff_Udk      = 0;
    General_Info.Rem_Canal    = 0;
    General_Info.Stick        = 0x00;
    General_Info.Strig        = 0xFF;
    General_Info.Strig1       = 0xFF;
    General_Info.Break        = 0;
    General_Info.EnableKeyEntry = true;
    General_Info.LcdOn        = false;

    connect(this,SIGNAL(TurnOFFSig()),pPC,SLOT(TurnOFFSlot()));
}

void CT6834::Reset()
{
    // Ram_Video should be integrated into mem.
    memset((void*)&Ram_Video,0,sizeof (Ram_Video));
    memset(mem,0,0x2200);
    initUdk();
}

void CT6834::AddKey (UINT8 Key)
{
    Clavier.append(Key);
}

void CT6834::AddFKey (UINT8 F_Key)
{

    if (F_Key < 12)
        for (int i=3;(i<udk_size[i]) && mem[0x800+udk_ofs[F_Key]+i];i++)
            AddKey(mem[0x800+udk_ofs[F_Key]+i]);
}

void CT6834::keyRelease(QKeyEvent *event)
{

    kana=graph=shift=ctrl = false;

//    switch (event->modifiers()) {
//        case Qt::ShiftModifier : shift = true; break;
//        case Qt::AltModifier:   graph = true; break;
//        case Qt::ControlModifier: ctrl = true; break;
//    }

//    if(General_Info.Aff_Udk) {
//        AffUdkON(shift);
//    }

    switch(event->key()) {
    case Qt::Key_Backspace:	// bs->left
        General_Info.Stick = 0x00;
        break;

    case Qt::Key_Space:
        General_Info.Strig1 = 0xff;
        break;
    case Qt::Key_Up    : General_Info.Stick &= ~0x01; break;
    case Qt::Key_Right : General_Info.Stick &= ~0x02; break;
    case Qt::Key_Down  : General_Info.Stick &= ~0x04; break;
    case Qt::Key_Left  : General_Info.Stick &= ~0x08; break;
    case Qt::Key_F6:	// F6
        General_Info.Strig = 0xff;
        break;
    }
    event->accept();
}

void CT6834::keyPress(QKeyEvent *event)
{
    UINT8 code,val;



    switch (event->modifiers()) {
    case Qt::ShiftModifier : switch (event->key()) {
        case Qt::Key_F1    : AddFKey (6);break;
        case Qt::Key_F2    : AddFKey (7);break;
        case Qt::Key_F3    : AddFKey (8);break;
        case Qt::Key_F4    : AddFKey (9);break;
        case Qt::Key_F5    : AddFKey (10);break;
        case Qt::Key_F6    : AddFKey (11);break;
        } break;
    default:  switch (event->key()) {
        case Qt::Key_F1    : AddFKey (0);break;
        case Qt::Key_F2    : AddFKey (1);break;
        case Qt::Key_F3    : AddFKey (2);break;
        case Qt::Key_F4    : AddFKey (3);break;
        case Qt::Key_F5    : AddFKey (4);break;
        case Qt::Key_F6    : General_Info.Strig = 0; AddFKey (5);break;
        }
    }

    switch (event->key()) {
    case Qt::Key_F1    :
    case Qt::Key_F2    :
    case Qt::Key_F3    :
    case Qt::Key_F4    :
    case Qt::Key_F5    :
    case Qt::Key_F6    : break;
    case Qt::Key_F12   : TurnOFF(); break;

    case Qt::Key_Up    : General_Info.Stick |= 0x01; AddKey(0x1e);break;
    case Qt::Key_Right : General_Info.Stick |= 0x02; AddKey(0x1c);break;
    case Qt::Key_Down  : General_Info.Stick |= 0x04; AddKey(0x1f);break;
    case Qt::Key_Left  : General_Info.Stick |= 0x08; AddKey(0x1d);break;


    case Qt::Key_Return : AddKey(0x0d);break;

    case Qt::Key_Shift : shift = true; break;
    case Qt::Key_Control: ctrl = true; break;
    case Qt::Key_Alt:   graph = true; break;
    default:

        kana=graph=shift=ctrl = false;
        switch (event->modifiers()) {
            case Qt::ShiftModifier : shift = true; break;
            case Qt::AltModifier:   graph = true; break;
            case Qt::ControlModifier: ctrl = true; break;
        }
        code = event->key();
        if ((code == 0x03) || (ctrl && code==Qt::Key_C)) {
            General_Info.Break = 1;
            val = 0;
        }
        else
        if(ctrl) val = key_tbl_c[code];
        else if(kana) {
            if(shift) val = key_tbl_ks[code];
            else val = key_tbl_k[code];
        }
        else if(graph) val = key_tbl_g[code];
        //else if(shift) val = key_tbl_s[code];
        else {
            val = code;//key_tbl[code];
            // Manage lowercase
            if (shift ) {//&& !((val >=0x30) && (val <=0x39))) {
                if ((val >=0x41) && (val <= 0x5a)) val += 0x20;
                else if ((code == pPC->pKEYB->LastKey)/*&&(val >=0x30) && (val <= 0x39)*/) val =key_tbl_s[code];
            }
        }

        if(val) AddKey(val);

    }
    event->accept();
}


void CT6834::TurnOFF() {
    //      m_warm_start = 1;
    //      m_sleep = 0;
    mem[0x0006] &= 0xBF;      // bit 6 to 0 for SLEEP
    mem[0x0006] |= 0x01;      // bit 0 to 1 for OFF
    General_Info.LcdOn = false;
    emit TurnOFFSig();
//    pPC->TurnOFF();
}

/*
  E1FE
            VIDEO RAM
  E000

  CFFF
            Programme de lancement
  CE00

  CCFF
            Buffer clavier
  CC00
            Char user defined
  CA00
            F Key
  C800

  C000
  */
