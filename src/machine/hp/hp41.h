#ifndef HP41_H
#define HP41_H

// *********************************************************************
//    Copyright (c) 1989-2002  Warren Furlow
//    Sound, Timer, Trace Copyright (c) 2001 Zharkoi Oleg
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
// HP41.h: header file
// *********************************************************************

#include "cpu.h"

typedef unsigned char flag;
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned int uint;

// ram registers each have 7 bytes
struct RAM_REG
  {
  byte Reg[7];
  };

extern word TypeA[];
enum {eSoundNone=0,eSoundSpeaker,eSoundWave};       // sound modes

#define NORMAL 0                      // for keys - normal position
#define MOVE 1                        // for keys - moved position

// Processor performance characteristics
// ProcCycles / ProcInterval=5.8 for a real machine: 578 cycles / 100 ms per interval
// 5780 inst/sec = 1E6 / 173 ms for a halfnut HP-41CX instruction (older models run at 158 ms)
// time in milliseconds between processor runs:
#define DEFAULT_PROC_INTERVAL 50
#define MAX_PROC_INTERVAL 100
#define MIN_PROC_INTERVAL 10
// number of processor cycles to run each time:
#define DEFAULT_PROC_CYCLES 578

#define DEFAULT_INST_SPEED 173        // milliseconds of actual instruction cycle to simulate for Speaker sound
#define DEFAULT_CONTRAST 5            // contrast value
#define DISPLAY_BOOST 75              // this value is necessary for CAT 2 in SST mode to prevent display glitches
#define MAX_RAM 0x400                 // maximum number of ram registers

// user code load/save
struct Cat1Label
  {
  char szText[16];
  int StartReg,StartByte;
  int EndReg,EndByte;
  };

/****************************/

// Module Memory Structures - see MOD file structures for field descriptions
struct ModuleHeader
  {
  char szFullFileName[_MAX_PATH];
  char szFileFormat[5];
  char szTitle[50];
  char szVersion[10];
  char szPartNumber[20];
  char szAuthor[50];
  char szCopyright[100];
  char szLicense[200];
  char szComments[255];
  byte Category;
  byte Hardware;
  byte MemModules;
  byte XMemModules;
  byte Original;
  byte AppAutoUpdate;
  byte NumPages;
  };
struct ModulePage
  {
  ModuleHeader *pModule;          // pointer to module that this page is a part of, or NULL if none
  ModulePage *pAltPage;           // pointer to alternate page if any (HEPAX, W&W RAMBOX2 use)
  char szName[50];
  char szID[9];
  byte Page;                      // file data - unchanged
  byte ActualPage;                // running data- the actual location this page is loaded in
  byte PageGroup;                 // file data - unchanged
  byte Bank;
  byte BankGroup;                 // file data - unchanged
  uint ActualBankGroup;           // running data - BankGroup is unique to file only
  flag fRAM;
  flag fWriteProtect;
  flag fFAT;                      // could be incorrectly set to false if a .ROM file loaded
  flag fHEPAX;                    // if a HEPAX page
  flag fWWRAMBOX;                 // if a W&W RAMBOX page
  word Image[4096];
  };

// functions
flag LoadBMP(char *pszFile,BITMAPINFOHEADER *&pBM,LOGPALETTE *&pPalette,BITMAPINFO *&pPaletteIndex);
void FreeBMP(BITMAPINFOHEADER *&pBM,LOGPALETTE *&pPalette,BITMAPINFO *&pPaletteIndex);
//void ConvertASCIItoLCD(char *szASCII,char *dis_str,flag fLCD4);

/****************************/
// The HP-41 class
/****************************/
class Chp41:public CCPU{
  {
  friend class CMcodeDlg;
  friend class CBreakPointsDlg;
public:
  Chp41();
  ~Chp41();
  static Chp41 *pThis;      // to give callback procs a this pointer
  enum {eAwake=0,eLightSleep=1,eDeepSleep=2};       // sleep modes
  void Wakeup(void);
  void Sleep(int eNewMode);
  int IsSleeping(void);
  void MemoryLost(void);
  void EnableRun(void);
  void DisableRun(void);
  void Run(void);
  void Halt(void);
  void SetProcParams(int ProcIntervalIn,int ProcCyclesIn);
  void GetProcParams(int &ProcIntervalOut,int &ProcCyclesOut);
  static void CALLBACK ExecuteProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime);
  static void CALLBACK TimerProc(UINT uId,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2);

  // HP41Graphics.cpp:
  void Draw(CDC *pCDC);
  void GetLCD(char *str);
  void GetDisplay(char *str);
  void GetAnnunciator(char *str);
  void GetShortAnnunciator(char *str);
  int GetContrast(void);
  void SetContrast(int ContrastVal);
  void SetFontColor(int ColorVal);
  int GetFontColor(void);
  int GetIndicator() {return(Indicator);}
  void SetIndicator(int IndicatorIn) {Indicator=IndicatorIn;}
  LOGPALETTE *GetPalKeyboard(void);

  // HP41File.cpp:
  int LoadMOD(ModuleHeader *&pModuleOut,char *pszFullPath);
  int SaveMOD(ModuleHeader *pModule,char *pszFullPath);
  void UnloadMOD(ModuleHeader *pModule);
  void FreePage(uint page,uint bank);
  int LoadConfig(char *pszLodFile);
  int SaveConfig(char *pszLodFile);
  int GetUserCode(char *pszUCFile,char *pszError);
  int Catalog1(flag &fFirst,Cat1Label *pLbl);
  int PutUserCode(char *pszUCFile,char *pszError,Cat1Label *pLbl);

  // HP41Keyboard.cpp:
  enum {eKeyboardNone=0,eKeyboardTiny=4,eKeyboardSmall=1,eKeyboardMedium=2,eKeyboardLarge=3};
  enum {eFontLCD1,eFontLCD2,eFontLCD3,eFontLCD4};
  void SetKeyDown(byte KeyCode);
  void SetKeyUp(void);
  void SetKeyboard(int eKbd,flag fTrueType,flag fShowTitle,flag fShowMenu);
  int GetKeyboard(void);
  byte MapPointToKey(CPoint pt);
  byte MapKeyToKey(flag fAlt,flag fExtended,int PCKey);
  void MoveKey(byte Key,int State);

  // HP41Trace.cpp
  void FindGlobalName(uint addr, char *name);  // give address to get label name
  uint FindGlobalAddr(char *name);
  void ChangeInstSet(uint newset);
  uint GetInstSet(void);
  char *GetOpcodeName(uint opc);
  char *GetTEFName(uint tef);
  void PrintRegisters(void);
  void StartTrace(void);
  void StopTrace(void);
  void SwitchTrace(void);
  flag GetTrace(void);
  void TraceOut(void);
  void trace(void);
  void trace_class0(void);
  void trace_subclass0(void);
  void trace_subclass1(void);
  void trace_subclass2(void);
  void trace_subclass3(void);
  void trace_subclass4(void);
  void trace_subclass5(void);
  void trace_subclass6(void);
  void trace_subclass7(void);
  void trace_subclass8(void);
  void trace_subclass9(void);
  void trace_subclassA(void);
  void trace_subclassB(void);
  void trace_subclassC(void);
  void trace_subclassD(void);
  void trace_subclassE(void);
  void trace_subclassF(void);
  void trace_class1(void);
  void trace_class2(void);
  void trace_class3(void);

  void SetSoundMode(int val);
  int GetSoundMode(void);
  CFont *GetFontLCD(void) { return(&CFontLCD); }

  // trace
  FILE *hLogFile;
  uint InstSetIndex;      // instruction set: 0 - HP, 1 - Zencode, 2 - JDA, 3 - Special
  uint TEFIndex;          // TEF format  - corresponds to instruction set above
  flag fTrace;            // trace on/off
  char szTraceLabel[14];  // global label
  char szTraceOut[100];
  word PC_LAST,PC_TRACE;  // PC_REG for last executed instruction and for tracing at any address
  word TraceTyte1;        // same as Tyte1 but for tracing use only

  // graphics
  BITMAPINFOHEADER *pBMKeyboard;
  CBitmap CBMKeyboard;
  LOGPALETTE *pPalKeyboard;
  BITMAPINFO *pPalIndexKeyboard;
  CPalette CPalKeyboard;
  flag fUsePal;
  RECT ActiveRect;       //Clicks outside this area used for dragging if !MainWindow.ShowTitle

  // keyboard and font
  CFont CFontLCD;
  CFont CFontAnnun;
  RECT *pRectKeyboard;
  RECT *pRectLCD;
  RECT *pRectAnnun;
  int eKeyboard;                         // current keyboard
  int eFont;                             // current font
  int UseAltPunc;                        // use alternate punc chars (for ttf wider punc chars only)

  // activity dot
  word Indicator;                        // enables activity indicator
  RECT RectIndicator;
  CBrush brushRed,brushGray;

  // ROM variables
  CPtrList ModuleList;                   // pointers to the loaded modules
  ModulePage *PageMatrix[16][4];         // matrix of page pointers
  ModulePage *pCurPage;                  // current page pointer
  word CurPage;                          // current page number
  byte active_bank[16];                  // array[page] of banks that are enabled
  word NextActualBankGroup;              // counter for loading actual bank groups

  // CPU registers
  RAM_REG *pRAM;
  byte A_REG[14],B_REG[14],C_REG[14],M_REG[14],N_REG[14];
  word G_REG,F_REG,ST_REG,Q_REG,P_REG,KEY_REG,XST_REG;
  word *PT_REG;                          // set to address of Q_REG or P_REG
  word PT_PREV;                          // set to value of previous PT
  word FI_REG;                           //14 bits used - there is no actual physical register - this is just an input stream from peripherals
  word CARRY,KEYDOWN,BATTERY;
  word PC_REG,RET_STK0,RET_STK1,RET_STK2,RET_STK3;
  word BASE;                             // ==10 for decimal mode, ==16 for hex mode

  // CPU variables
  unsigned short eSleepMode;
  word perph_in_control,perph_selected,asdf,ram_selected,wqer;    // modes dependant on previous instructions that set modes
  word control_perph;              // number of perph in control
  word Tyte1;                      // current instruction (ten bit byte)
  word Tyte2;                      // valid only if tyte1 is two byte instruction
  word TytePrev;                   // previous instruction code
  word Modifier;                   // instruction modifier
  word FirstTEF,LastTEF;           // starting and ending tef pointers (always starting at least significant digit)
  word Boost;                      // boost the main loop by this many more cycles if it is near completion

  // display registers and variables
  byte DIS_C_REG[12];              // bit 8 for chars 1-12 (lower bit)
  byte DIS_B_REG[12];              // bits 7-4 for chars 1-12 (occupy lower nybbles)
  byte DIS_A_REG[12];              // bits 3-0 for chars 1-12 (occupy lower nybbles)
  word DIS_ANNUN_REG;              // 12 bit annunciator register
  word UpdateDisplay;              // set to 1 when display needs to be updated
  word UpdateAnnun;                // set to 1 when annunciators need to be updated
  word DisplayOn;                  // set to 1 when LCD is turned on
  word Contrast;                   // 0-15 value for contrast (half nut)
  COLORREF FontColor;              // RBG font color value

  // timer registers
  word TimerSelA;                  // ==1 if TIMER=A, ==0 if TIMER=B
  UINT64 ClockA,ClockB;
  UINT64 AlarmA,AlarmB;
  UINT64 IntTimer,IntTimerEnd;     // interval timer counter and terminal value
  byte CLK_A[14], CLK_B[14];       // clock A&B - 56 bits
  byte ALM_A[14], ALM_B[14];       // alarm A&B - 56 bits
  byte SCR_A[14], SCR_B[14];       // scratch A&B - 56 bits
  byte INTV_CNT[14], INTV_TV[14];  // interval timer - only low 20 bits used - INTV_CNT is counter, INTV_TV is terminal value
  byte ACC_F[14];                  // accuracy factor - only low 13 bits used
  byte TMR_S[14];                  // status bits - only low 13 bits used
  MMRESULT TimerEvent;             // ==NULL if no timer, higher precision multimedia timers used for callbacks

  // instruction delay
  LARGE_INTEGER PCPerf, PCCount[2];
  UINT64 InstrNSec;

  // control and state variables
  UINT ExecuteEvent;              // ==0 if no timer, non zero if timer is set - main loop execution timer callback
  int MinCLRKEY;                  // a counter for keeping a key pressed for a minimum number of CLRKEY instructions
  flag fEnableCLRKEY;             // ==1 when CLRKEY instruction is enabled
  int ProcInterval;                  // time in milliseconds between processor runs
  int ProcCycles;                 // number of processor cycles to run each time
  word MemModules;                // number of regular Memory modules (1-4)
  word XMemModules;               // number of Extended Memory modules (1-3)
  flag fPrinter,fCardReader,fTimer,fWand,fHPIL,fInfrared;    // 1 if the associated hardware is loaded by a module
  int SoundMode;                  // current sound mode
  flag fRunEnable;                // ==0 if Run() is disabled

  // breakpoint control
  flag fBreakPtsEnable;           // ==1 if breakpoints are enabled
  flag fBreak;                    // ==1 if breakpoint raised
  int nBreakPts;                  // count of breakpoints
  word BreakPts[100];             // ordered list of breakpoints

  word inline GetNextTyte(void);
  void SetPC(word addr);
  int RamExist(word addr);
  void Execute(void);

  void Class0(void);
  void Subclass0(void);
  void Enbank(int BankSet);
  void Subclass1(void);
  void Subclass2(void);
  void Subclass3(void);
  void Subclass4(void);
  void Subclass5(void);
  void Subclass6(void);
  void Subclass7(void);
  void Subclass8(void);
  void Subclass9(void);
  void SubclassA(void);
  void SubclassB(void);
  void SubclassC(void);
  void SubclassD(void);
  void SubclassE(void);
  void SubclassF(void);
  void wdata(void);
  void rdata(void);

  void Class1(void);
  void PushReturn(word addr);
  word PopReturn(void);
  void LongJump(word addr,flag fPush);

  void Class2(void);
  int ArithCarry(unsigned short tyte);
  void ConvertTEF(void);
  byte Adder(byte nib1,byte nib2);
  byte Subtractor(byte nib1,byte nib2);

  void Class3(void);

  void DisplayRead(void);
  void DisplayWrite(void);
  void DisplayRotRight(byte REG[],int Times);
  void DisplayRotLeft(byte REG[],int Times);
  void AnnunRead(void);
  void AnnunWrite(void);
  void HalfnutRead(void);
  void HalfnutWrite(void);

  void InitTimer(void);
  void DeInitTimer(void);
  void ResetTimer(void);
  void SaveTimer(void);
  void RestoreTimer(void);
  void ContinueTimer(void);
  void ConvertToReg14(byte *DEST_REG,UINT64 Src);
  void ConvertToUINT64(UINT64 *Dest,byte *SRC_REG);
  void TimerWrite(void);
  void TimerRead(void);

  void exec_perph_printer(void);
  void error_message(short num);
  void Speaker(short Freq, int Duration);

  // user code support
  int DecodeUCByte(int PrevType,byte CurrByte);
  void CalcOffset(int LowReg,int LowByte,int HighReg,int HighByte,int &RegOff,int &ByteOff);
  void PrevGlobal(int CurrReg,int CurrByte,int &PrevReg,int &PrevByte,int &RegOff,int &ByteOff);
  };


#endif // HP41_H
