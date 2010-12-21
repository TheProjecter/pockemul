/*** POCKEMUL [PC1600.CPP] ********************************/
/* PC1600 emulator main class                             */
/**********************************************************/
#include	<string.h>
#include	<stdlib.h>

#include    <QtGui>

#include	"common.h"
#include	"pc1600.h"
#include    "Lcdc_pc1600.h"
#include	"Inter.h"
#include	"Debug.h"
#include	"Keyb.h"
#include	"ce152.h"
#include	"dialoganalog.h"

extern int	g_DasmStep;
extern bool	UpdateDisplayRunning;


Cpc1600::Cpc1600(CPObject *parent)	: CpcXXXX(this)
{								//[constructor]
#ifndef QT_NO_DEBUG
    if (!fp_log) fp_log=fopen("pc1600.log","wt");	// Open log file
#endif

    setfrequency( (int) 3500000);
    setcfgfname(QString("pc1600"));

    SessionHeader	= "PC1600PKM";
    SessionHeaderLen= 9;
    Initial_Session_Fname ="pc1600.pkm";
    BackGroundFname	= ":/PC1600/pc1600/pc-1600.jpg";
    LcdFname		= ":/PC1600/pc1600/pc1600lcd.jpg";
    SymbFname		= ":/PC1500/pc1500/1500symb.jpg";
    memsize			= 0x0E0000;
    InitMemValue	= 0x00;

    SlotList.clear();

    KeyMap		= KeyMap1600;
    KeyMapLenght= KeyMap1600Lenght;

    Pc_Offset_X = Pc_Offset_Y = 0;

    Pc_DX		= 572;
    Pc_DY		= 254;

    Lcd_X		= 35;
    Lcd_Y		= 40;
    Lcd_DX		= 156;
    Lcd_DY		= 32;
    Lcd_ratio_X	= 2;
    Lcd_ratio_Y	= 2;

    Lcd_Symb_X	= 35;
    Lcd_Symb_Y	= 33;
    Lcd_Symb_DX	= 317;
    Lcd_Symb_DY	= 5;

    DialogExtensionID = 0;//IDD_EXT_PROPERTIES_1500;

    SoundOn			= false;
    lh5810_Access	= false;
    ce150_Access	= false;

    pLCDC		= new Clcdc_pc1600(this);


    pZ80        = new CZ80(this);
    pLH5803     = new CLH5803(this);

    pCPU		= pZ80;
    masterCPU   = true;
    cpuSwitchPending = false;

    pLU57813P   = new CLU57813P(this);

    pLH5810		= new CLH5810_PC1600(this);

    pTIMER		= new Ctimer(this);
    pCONNECTOR	= new Cconnector(this,60,"Connector 60 pins",false);	publish(pCONNECTOR);
    pSIOCONNECTOR=new Cconnector(this,15,"Connector 15 pins",false);	publish(pSIOCONNECTOR);
    pADCONNECTOR= new Cconnector(this,8,"Digital connector 2 pins",false);	publish(pADCONNECTOR);


    pKEYB		= new Ckeyb_pc1600(this);
    pce152		= new Cce152_PC15XX(this);
    delete pce152->pTIMER; pce152->pTIMER = pTIMER;

    pHD61102_1  = new CHD61102(this);
    pHD61102_2  = new CHD61102(this);

    pTC8576P = new CTC8576P(this,1288800);

    ce1600_connected = false;

    Tape_Base_Freq=2500;

    initExtension();
    extensionArray[0] = ext_60pins;
    extensionArray[1] = ext_MemSlot1;
    extensionArray[2] = ext_MemSlot2;

    SlotList.clear();

    // Bank 0
    SlotList.append(CSlot(16, 0x00000 ,	":/PC1600/pc1600/romI-0.bin"	, "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x04000 ,	":/PC1600/pc1600/romII-0.bin"	, "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x08000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0x0C000 ,	""								, "" , RAM , "RAM"));

    // Bank 1
    SlotList.append(CSlot(16, 0x10000 ,	""                              , "" , NOTUSED , "NOT USED"));
    SlotList.append(CSlot(16, 0x14000 ,	""                              , "" , NOTUSED , "NOT USED"));
    SlotList.append(CSlot(16, 0x18000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0x1C000 ,	""								, "" , NOTUSED , "NOT USED"));

    // Bank 2
    SlotList.append(CSlot(16, 0x20000 ,	""                              , "" , NOTUSED , "NOT USED"));
    SlotList.append(CSlot(16, 0x24000 ,	""                              , "" , NOTUSED , "NOT USED"));
    SlotList.append(CSlot(16, 0x28000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0x2C000 ,	""								, "" , NOTUSED , "NOT USED"));

    // Bank 3
    SlotList.append(CSlot(16, 0x30000 ,	":/PC1600/pc1600/rom3b.bin"     , "" , ROM , "ROM 3b"));
    SlotList.append(CSlot(16, 0x34000 ,	":/PC1600/pc1600/romIII-3.bin"	, "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x38000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0x3C000 ,	""								, "" , NOTUSED , "NOT USED"));

    // Bank 4
    SlotList.append(CSlot(16, 0x40000 ,	""                              , "" , NOTUSED , "NOT USED"));
    SlotList.append(CSlot(16, 0x44000 ,":/PC1600/pc1600/romce1600-1.bin", "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x48000 ,	""								, "" , NOTUSED , "NOT USED"));
    SlotList.append(CSlot(16, 0x4C000 ,	""								, "" , NOTUSED , "NOT USED"));

    // Bank 5
    SlotList.append(CSlot(16, 0x50000 ,	""                              , "" , NOTUSED , "NOT USED"));
    SlotList.append(CSlot(16, 0x54000 ,":/PC1600/pc1600/romce1600-2.bin", "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x58000 ,	""								, "" , NOTUSED , "NOT USED"));
    SlotList.append(CSlot(16, 0x5C000 ,	""								, "" , NOTUSED , "NOT USED"));

    // Bank 6
    SlotList.append(CSlot(16, 0x60000 ,	""                              , "" , NOTUSED , "NOT USED"));
    SlotList.append(CSlot(16, 0x64000 ,	""                              , "" , NOTUSED , "NOT USED"));
    SlotList.append(CSlot(16, 0x68000 ,	":/PC1600/pc1600/romIV-6.bin"	, "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x6C000 ,	""								, "" , NOTUSED , "NOT USED"));

    // Initialize bank nummber
    bank1 = bank2 = bank3 = bank4 = 0;

    // MOD 1 MAP
    SlotList.append(CSlot(8 , 0x70000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(8 , 0x72000 ,	""								, "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x74000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(8 , 0x78000 ,	""								, "" , NOTUSED , "NOT USED"));
    SlotList.append(CSlot(8 , 0x7A000 ,	""								, "" , ROM , "ROM"));
    SlotList.append(CSlot(16, 0x7C000 ,	":/PC1600/pc1600/rom1500.bin"	, "" , ROM , "SYSTEM ROM"));
    SlotList.append(CSlot(64, 0x80000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(8 , 0x90000 ,	""								, "" , ROM , "ROM"));
    SlotList.append(CSlot(8 , 0x92000 ,	""								, "" , ROM , "ROM"));
    SlotList.append(CSlot(8 , 0x94000 ,	":/PC1500/pc1500/CE-150.ROM"	, "" , ROM , "CE-150 ROM"));

    // RAM BANK
    SlotList.append(CSlot(16, 0xA0000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xA4000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xA8000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xAC000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xB0000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xB4000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xB8000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xBC000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xC0000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xC4000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xC8000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xCC000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xD0000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xD4000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xD8000 ,	""								, "" , RAM , "RAM"));
    SlotList.append(CSlot(16, 0xDC000 ,	""								, "" , RAM , "RAM"));

}
void Cpc1600::Reset(void)
{
    pZ80->Reset();
    pLH5803->Reset();
    pLU57813P->Reset();
    pLH5810->Reset();

    CpcXXXX::Reset();
}

void Cpc1600::TurnON(void)
{
    AddLog(LOG_FUNC,"Cpc1600::TurnOn");

    //remove(Initial_Session_Fname.toStdString().c_str());
    pCPU->Reset();
    CpcXXXX::TurnON();
}

void	Cpc1600::initExtension(void)
{
    // initialise ext_MemSlot1  S1
    ext_MemSlot1 = new CExtensionArray("Memory Slot 1 (S1)","Add memory module");
    ext_MemSlot1->setAvailable(ID_CE151,true,false);
    ext_MemSlot1->setAvailable(ID_CE155,true,false);
    ext_MemSlot1->setAvailable(ID_CE159,true,false);
    ext_MemSlot1->setAvailable(ID_CE161,true,false);
    ext_MemSlot1->setAvailable(ID_CE1600M,true,false);
//    ext_MemSlot1->setAvailable(ID_CE1601M,true,false);
    addExtMenu(ext_MemSlot1);

    // initialise ext_MemSlot2  S2
    ext_MemSlot2 = new CExtensionArray("Memory Slot 2 (S2)","Add memory module");
    ext_MemSlot2->setAvailable(ID_CE161,true,false);
    ext_MemSlot2->setAvailable(ID_CE1600M,true,false);
    ext_MemSlot2->setAvailable(ID_CE1601M,true,false);
    ext_MemSlot2->setAvailable(ID_CE16096,true,false);
    ext_MemSlot2->setAvailable(ID_CE16128,true,false);
    ext_MemSlot2->setAvailable(ID_CE16160,true,false);
    ext_MemSlot2->setAvailable(ID_CE16192,true,false);
    ext_MemSlot2->setAvailable(ID_CE16224,true,false);
    ext_MemSlot2->setAvailable(ID_CE16256,true,false);

    addExtMenu(ext_MemSlot2);
}

bool Cpc1600::CompleteDisplay(void)
{
    QPainter painter;

//	AddLog(LOG_FUNC,"Cpc1600::CompleteDisplay");

    CpcXXXX::CompleteDisplay();

    return TRUE;
}


bool Cpc1600::InitDisplay(void)
{
    AddLog(LOG_FUNC,"Cpc1600::InitDisplay");

    CpcXXXX::InitDisplay();

    return(1);
}



bool Cpc1600::LoadConfig(QFile *file)
{
    AddLog(LOG_FUNC,"Cpc1600::LoadConfig");
    CpcXXXX::LoadConfig(file);

    QDataStream in(file);
    qint8 b1,b2,b3,b4,csp,mcpu;

    in >> b1 >> b2 >> b3 >> b4;
    in >> csp >> mcpu;
    bank1 = b1;
    bank2 = b2;
    bank3 = b3;
    bank4 = b4;
    cpuSwitchPending = csp;
    masterCPU = mcpu;

    pZ80->Load_Internal(file);
    pLH5803->Load_Internal(file);
    pLH5810->Load_Internal(file);
    pHD61102_1->Load_Internal(file);
    pHD61102_2->Load_Internal(file);
    pLU57813P->Load_Internal(file);


    //--	fread(&Extension,1,sizeof(TExtension),fp);
    return(1);
}

bool Cpc1600::SaveConfig(QFile *file)
{
    AddLog(LOG_FUNC,"Cpc1600::SaveConfig");
    CpcXXXX::SaveConfig(file);

    QDataStream out(file);
    out << (qint8)bank1 << (qint8)bank2 << (qint8)bank3 << (qint8)bank4;
    out << (qint8)cpuSwitchPending << (qint8) masterCPU;

    pZ80->save_internal(file);
    pLH5803->save_internal(file);
    pLH5810->save_internal(file);
    pHD61102_1->save_internal(file);
    pHD61102_2->save_internal(file);
    pLU57813P->save_internal(file);



//--	fwrite(&Extension,1,sizeof(TExtension),fp);
    return(1);
}

bool Cpc1600::init(void)				// initialize
{
    fp_CRVA = 0;
    // if DEBUG then log CPU
#ifndef QT_NO_DEBUG
    pZ80->logsw = true;
#endif
    CpcXXXX::init();

    masterCPU = false;
    pCPU = pLH5803;

    // if DEBUG then log CPU
#ifndef QT_NO_DEBUG
//    pCPU->logsw = true;
#endif

    pCPU->init();
    masterCPU=true;
    pCPU = pZ80;

    pLU57813P->init();
    pLH5810->init();

    WatchPoint.remove(this);

    WatchPoint.add(&pCONNECTOR_value,64,60,this,"Standard 60pins connector");
    WatchPoint.add((qint64 *) &(pLH5810->lh5810.r_opa),8,8,this,"LH5810 Port A");
    WatchPoint.add((qint64 *) &(pLH5810->lh5810.r_opb),8,8,this,"LH5810 Port B");
    WatchPoint.add((qint64 *) &(pLH5810->lh5810.r_opc),8,8,this,"LH5810 Port C");

    return true;
}

bool Cpc1600::run(void)
{
    DWORD previous_pc;
    DWORD Current_PC;

    previous_pc = pCPU->get_PC();

// ---------------------------------------------------------
    CpcXXXX::run();

    if (off) return true;

    if (cpuSwitchPending)
    {
        if (masterCPU)
        {
            if (pZ80->z80.r.halt) {
                // Switch CPU
                if (pCPU->fp_log) fprintf(pCPU->fp_log,"\nSWITCH CPU\n\n");
                //pLH5803->lh5801.HLT = 0;
                pCPU = pLH5803;
                cpuSwitchPending = false;
                masterCPU = false;
                for (int i=0x30;i<0x40;i++) {
                     mem[0x8A000+i] = pZ80->imem[i];
                }
            }
        }
        else
        {
            if (pCPU->fp_log) fprintf(pCPU->fp_log,"\nSWITCH CPU\n\n");
            //pLH5803->lh5801.HLT = 1;
            pCPU = pZ80;
            pZ80->z80.r.halt = false;
            cpuSwitchPending = false;
            masterCPU = true;
            // Copy 0x30-0x3f to 1A030-1A03F
            for (int i=0x30;i<0x40;i++) {
                pZ80->imem[i] = mem[0x8A000+i];
            }
        }
    }

// ---------------------------------------------------------

    Current_PC = pCPU->get_PC();

#ifndef QT_NO_DEBUG
    hack(Current_PC);
#endif

    //----------------------------------
    // SOUND BUFFER (quite simple no?)
    //----------------------------------
    pLH5810->SetRegBit(LH5810_OPC,6,pLH5810->GetReg(LH5810_OPC) & 0x80);
    fillSoundBuffer((pLH5810->GetReg(LH5810_OPC) & 0x40 ? 0x00 : 0xff));

    //----------------------------------



    // 1/64s and 1/2s interrupt
    PUT_BIT(pCPU->imem[0x32],4,pTIMER->GetTP( pLU57813P->Get_tpIndex64()));

#if 1
    pLU57813P->step();
    PUT_BIT(pCPU->imem[0x32],6,pTIMER->GetTP( pLU57813P->Get_tpIndex2()));
#else

    bool tips = pTIMER->GetTP( pLU57813P->Get_tpIndex2());
    if (READ_BIT(pCPU->imem[0x32],6) != tips) {
        PUT_BIT(pCPU->imem[0x32],6,tips);
        pLU57813P->step();
    }
#endif
    if (pLU57813P->Get_Kon())
    {
        pLH5810->step();
    }

    //pTC8576P->step();

    return(1);
}



INLINE void Cpc1600::hack(DWORD pc)
{
    // HACK Program Counter
    //#define FUNC_CALL(ADDR,LIB) case ADDR: AddLog(LOG_ROM,tr(LIB).arg(((CLH5801 *)pCPU)->get_PC(),5,16,QChar('0')));
    #define FUNC_CALL(BANK,ADDR,LIB) \
        case ADDR: { \
            if ( ((ADDR >= 0x0000) && (ADDR <= 0x3FFF) && (BANK == bank1)) || \
                 ((ADDR >= 0x4000) && (ADDR <= 0x7FFF) && (BANK == bank2)) || \
                 ((ADDR >= 0x8000) && (ADDR <= 0xBFFF) && (BANK == bank3)) || \
                 ((ADDR >= 0xC000) && (ADDR <= 0xFFFF) && (BANK == bank4)) ) {\
            fprintf(fp_log,tr(LIB).arg(((CLH5801 *)pCPU)->get_PC(),5,16,QChar('0')).toStdString().c_str(),pZ80->z80.r.c); \
            if (pCPU->fp_log) fprintf(pCPU->fp_log,tr(LIB).arg(((CLH5801 *)pCPU)->get_PC(),5,16,QChar('0')).toStdString().c_str(),pZ80->z80.r.c); \
            }\
        }

    #define FUNC_CALL_C(BANK,ADDR,C,LIB) \
        case C: { \
            if ( ((ADDR >= 0x0000) && (ADDR <= 0x3FFF) && (BANK == bank1)) || \
                 ((ADDR >= 0x4000) && (ADDR <= 0x7FFF) && (BANK == bank2)) || \
                 ((ADDR >= 0x8000) && (ADDR <= 0xBFFF) && (BANK == bank3)) || \
                 ((ADDR >= 0xC000) && (ADDR <= 0xFFFF) && (BANK == bank4)) ) {\
            fprintf(fp_log,tr(LIB).arg(((CLH5801 *)pCPU)->get_PC(),5,16,QChar('0')).toStdString().c_str(),pZ80->z80.r.c); \
            if (pCPU->fp_log) fprintf(pCPU->fp_log,tr(LIB).arg(((CLH5801 *)pCPU)->get_PC(),5,16,QChar('0')).toStdString().c_str(),pZ80->z80.r.c); \
            }\
        }

    if (fp_log == 0) return;

    switch (pc)
    {

    FUNC_CALL(0,0x0100,QT_TR_NOOP("[%1] - PRTANK  - Display one character\n"));             break;
    FUNC_CALL(0,0x00EB,QT_TR_NOOP("[%1] - PRTASTR - Display a string of characters\n"));    break;
    FUNC_CALL(0,0x0115,QT_TR_NOOP("[%1] - CRSRSET - Set the cursor position\n"));           break;
    FUNC_CALL(0,0x0118,QT_TR_NOOP("[%1] - CRSRPOS - Read the current cursor position\n"));  break;
    FUNC_CALL(0,0x011B,QT_TR_NOOP("[%1] - RVSCHR  - Cursor DEreg,Areg* Zeichen invertiere\n"));  break;
    FUNC_CALL(0,0x011E,QT_TR_NOOP("[%1] - CRSRSTAT- Specify the cursor type\n"));			break;
    FUNC_CALL(0,0x012D,QT_TR_NOOP("[%1] - UPSCRL  - scroll up the screen\n"));              break;
    FUNC_CALL(0,0x0130,QT_TR_NOOP("[%1] - DWNSCRL - Srool down the screen\n"));				break;
    FUNC_CALL(0,0x0142,QT_TR_NOOP("[%1] - INS1LN  - Insert a blank line\n"));               break;
    FUNC_CALL(0,0x0145,QT_TR_NOOP("[%1] - ERS1LN  - Erase the contents of a line\n"));      break;
    FUNC_CALL(0,0x013F,QT_TR_NOOP("[%1] - ERSSTR  - Display a specified number of space\n"));break;
    FUNC_CALL(0,0x013C,QT_TR_NOOP("[%1] - SMBLSET - Set the state of the status line symbols\n"));				break;
    FUNC_CALL(0,0x0139,QT_TR_NOOP("[%1] - SMBLREAD-Read the state of the status une symbots.\n"));							break;
    FUNC_CALL(0,0x0109,QT_TR_NOOP("[%1] - SETANK  - Set the display to the character mode.\n"));							break;
    FUNC_CALL(0,0x0127,QT_TR_NOOP("[%1] - DOTSET  - Display a dot in the set/presetlreverse mode.\n"));							break;
    FUNC_CALL(0,0x012A,QT_TR_NOOP("[%1] - DOTREAD - Read the display state of a dot.\n"));							break;
    FUNC_CALL(0,0x0121,QT_TR_NOOP("[%1] - LINE    - Drawa une.\n"));							break;
    FUNC_CALL(0,0x0124,QT_TR_NOOP("[%1] - BOX     - Draw a box.\n"));							break;
    FUNC_CALL(0,0x014B,QT_TR_NOOP("[%1] - GCRSRSET- Set the graphics Cursor position.\n"));							break;
    FUNC_CALL(0,0x0148,QT_TR_NOOP("[%1] - GCRSRPOS- Read the current graphics cursor position.\n"));							break;
    FUNC_CALL(0,0x014E,QT_TR_NOOP("[%1] - PRTGCHR - Display a character at the current graphics cursor position.\n"));							break;
    FUNC_CALL(0,0x00EE,QT_TR_NOOP("[%1] - PRTGSTR - Display a string of characters from the current graphics cursor position.\n"));							break;
    FUNC_CALL(0,0x0154,QT_TR_NOOP("[%1] - PRTGPTN - Display a 1 X 8 dot pattern at the current graphics cursor position.\n"));							break;
    FUNC_CALL(0,0x015A,QT_TR_NOOP("[%1] - GPTNREAD- Read the 1 X 8 dot pattern atthe Current graphics cursor position.\n"));							break;
    FUNC_CALL(0,0x0133,QT_TR_NOOP("[%1] - CGMODE  - Change the character generator mode between the PC-1500 mode and the PC-1600 mode.\n"));							break;
    FUNC_CALL(0,0x0157,QT_TR_NOOP("[%1] - CPY1500LCD- Copy the contents of the fourth Une of the screen to the PC-1500 mode\n"));							break;
    FUNC_CALL(0,0x0112,QT_TR_NOOP("[%1] - CLS     - Clear the screen display.\n"));		break;
    FUNC_CALL(0,0x00E5,QT_TR_NOOP("[%1] - BSPCTR  - Enable/disable the LCD.\n"));							break;
    FUNC_CALL(0,0x015D,QT_TR_NOOP("[%1] - SAVELCD - Save the 156x 8 dot pattern of the specified me to RAM.\n"));						break;
    FUNC_CALL(0,0x0160,QT_TR_NOOP("[%1] - LOADLCD - Load the 156 X 8 dot pattern from RAM to the specified une.\n"));                break;



    FUNC_CALL(6,0x809C,QT_TR_NOOP("[%1] - CRSRCL  - Cursor 1öschen\n"));                break;
    FUNC_CALL(6,0x80A2,QT_TR_NOOP("[%1] - CRSRSTATN-Setze Cursor wie 011E,aber vorher Cursor nicht löschen\n"));                break;
    FUNC_CALL(6,0x809F,QT_TR_NOOP("[%1] - CHARADR - Pixe1code des Zeichens Aregermitteln,Startadresse>HLreg\n"));                break;




    FUNC_CALL(0,0x0166,QT_TR_NOOP("[%1] - KEYGET     - Read cne character from the key buffer-. If the key buffer- is empty, the routine waits for e key input.\n"));                break;
    FUNC_CALL(0,0x0169,QT_TR_NOOP("[%1] - KEYGETR    - Same function as KEYGET except the routine does net wait for a key input even if the key buffer is empty.\n"));                break;
    FUNC_CALL(0,0x016C,QT_TR_NOOP("[%1] - KBUFSET    - Load data inte the key buffer or clear the key buffer-.\n"));                break;
    FUNC_CALL(0,0x016F,QT_TR_NOOP("[%1] - BREAKCHK   - Read the state of the BREAK key.\n"));                break;
    FUNC_CALL(0,0x0172,QT_TR_NOOP("[%1] - CURUDCHK   - Read the state of the up and down arrow.\n"));                break;
    FUNC_CALL(0,0x0175,QT_TR_NOOP("[%1] - KEYDIRECT  - Scan the keys and read the key code of the key that has been pressed when the routine is called.\n"));                break;
    FUNC_CALL(0,0x0178,QT_TR_NOOP("[%1] - KEYSTRB    - Scan one row of keys te identify a particular ene of more than one key pressed at the same time.\n"));                break;
    FUNC_CALL(0,0x017B,QT_TR_NOOP("[%1] - KEVAUX     - Specify the key input device (main keyboard or RS-232C).\n"));                break;
    FUNC_CALL(0,0x017E,QT_TR_NOOP("[%1] - KEYSTATSET - Specify the key repeat function and the key click function.\n"));                break;
    FUNC_CALL(0,0x0181,QT_TR_NOOP("[%1] - KEYSTATREAD- Read the settings cf the key repeat and click functions and the cur-r-ent key input device.\n"));                break;
    FUNC_CALL(0,0x0184,QT_TR_NOOP("[%1] - OFFCHK     - Read the state cf the OFF key.\n"));                break;
    FUNC_CALL(0,0x0187,QT_TR_NOOP("[%1] - KEYGETND   - Read the first char-acter in the key buffer without changing the contents of the key buffer.\n"));                break;
    FUNC_CALL(0,0x018A,QT_TR_NOOP("[%1] - BREAKRESET - Clear the latch cf the BREAK key.\n"));                break;


    case 0x01D8: switch (pZ80->z80.r.c) {
    FUNC_CALL_C(0,0x01D8,0x01,QT_TR_NOOP("RS232 / SIO - [%1] - CWCOM      - Set communication parameters.\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x02,QT_TR_NOOP("RS232 / SIO - [%1] - CRCOM      - Read the current communication parameters.\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x03,QT_TR_NOOP("RS232 / SIO - [%1] - CSNDA      - Transmit one byte cf data.\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x04,QT_TR_NOOP("RS232 / SIO - [%1] - CRCVA      - Receive one byte cf data (if there is no data te read, wait until data are sent in.)\n"));
                        Hack_CRVA();
                        break;
    FUNC_CALL_C(0,0x01D8,0x07,QT_TR_NOOP("RS232 / SIO - [%1] - CRCV1      - Receive one byte of data (do net wait even if there is no data te read.)\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x0E,QT_TR_NOOP("RS232 / SIO - [%1] - CSETHS     - Set RS and ER signais to high in the auto handshake mode.\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x0F,QT_TR_NOOP("RS232 / SIO - [%1] - CRESHS     - Set RS and ER signais to low in the auto handshake mode.\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x10,QT_TR_NOOP("RS232 / SIO - [%1] - CWOUTS     - Set the state of the outgoing controi signaIs (RS and ER).\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x11,QT_TR_NOOP("RS232 / SIO - [%1] - CRCTRL     - Read the state of the control signals.\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x12,QT_TR_NOOP("RS232 / SIO - [%1] - CWDEV      - Select a channel and set the input and output device selection parameters for that channel.\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x13,QT_TR_NOOP("RS232 / SIO - [%1] - CRDEV      - Read the currently seiected channel number and the input and output device selection parameters set by CWDEV routine.\n"));
    pCPU->logsw=1;
    pCPU->Check_Log();
    break;
    FUNC_CALL_C(0,0x01D8,0x14,QT_TR_NOOP("RS232 / SIO - [%1] - CESND      - Enable the transmission on(y when the specified incoming control signal (or signaIs) is high.\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x15,QT_TR_NOOP("RS232 / SIO - [%1] - CERCV      - Enabie the reception only when the specified incoming control signal (or- signaIs) is low.\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x16,QT_TR_NOOP("RS232 / SIO - [%1] - CSBRK      - Send the specified number et break characters\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x17,QT_TR_NOOP("RS232 / SIO - [%1] - CSRCVB     - Reserve a receive buffer in memory.\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x18,QT_TR_NOOP("RS232 / SIO - [%1] - CCLRSB     - Clear the work area for transmission.\n"));                break;
    FUNC_CALL_C(0,0x01D8,0x1C,QT_TR_NOOP("RS232 / SIO - [%1] - CCLRRB     - Clearthe receive buffer.\n"));                break;

    }
    break;

    case 0x01D5: switch (pZ80->z80.r.c) {
    FUNC_CALL_C(0,0x01D5,0x00,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SINIT      - Initialize the timers and the analog input port.\n"));                break;
    FUNC_CALL_C(0,0x01D5,0x01,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SBEEP      - Generate a key clicking sound.\n"));                break;
    FUNC_CALL_C(0,0x01D5,0x02,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SWRT       - Set a date and time for the calendar dock.\n"));
//    pCPU->logsw=1;
//    pCPU->Check_Log();
    break;
    FUNC_CALL_C(0,0x01D5,0x03,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SRRT       - Read the current date and time of the calendar dock.\n"));
//                                                    pCPU->logsw=1;
//                                                    pCPU->Check_Log();
//                                                    ((CZ80 *)pCPU)->z80.r.c = 0x23;
    break;
    FUNC_CALL_C(0,0x01D5,0x04,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SWWT       - Set a date and time for the wakeup timer.\n"));
//                                                pCPU->logsw=1;
//                                                pCPU->Check_Log();
                                                break;
    FUNC_CALL_C(0,0x01D5,0x05,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SWRT       - Read the current settings of date and time of the wakeup timer.\n"));
//                                                pCPU->logsw=1;
//                                                pCPU->Check_Log();
                                                break;
    FUNC_CALL_C(0,0x01D5,0x06,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SWA1T      - Set a date and time for the alarm1 timer (same as ON TIME$ command).\n"));                break;
    FUNC_CALL_C(0,0x01D5,0x07,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SRA1T      - Read the current settings of date and time of the alarm timer.\n"));                break;
    FUNC_CALL_C(0,0x01D5,0x08,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SWA2T      - Set a date and time for the alarm2 timer (same as ALARM$ command).\n"));                break;
    FUNC_CALL_C(0,0x01D5,0x09,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SRA2T      - Read the current settings of date and time ofthe alarm2 timer.\n"));                break;
    FUNC_CALL_C(0,0x01D5,0x10,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SWMSK      - Set the interrupt mask for SC-7852.\n"));
//                                                        pCPU->logsw=1;  // 0E, 7C, 5F ????
//                                                        pCPU->Check_Log();
                                                        break;
    FUNC_CALL_C(0,0x01D5,0x11,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SRMSK      - Read the current setting of the interrupt mask for SC-7852.\n"));
//                                                    pCPU->logsw=1;  // 5E
//                                                    pCPU->Check_Log();
                                                        break;
    FUNC_CALL_C(0,0x01D5,0x12,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SRIRQ      - Read the current interrupt cause for SC-7852.\n"));
//                                                    pCPU->logsw=1;    // 5D
//                                                    pCPU->Check_Log();
                                                        break;
    FUNC_CALL_C(0,0x01D5,0x13,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SRINP      - Read the state of the CI signal cf RS-232c port.\n"));
//                                                    pCPU->logsw=1;    // 5C
//                                                    pCPU->Check_Log();
                                                        break;
    FUNC_CALL_C(0,0x01D5,0x14,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SWPON      - Set a mask for the power-on conditions.\n"));                break;

    FUNC_CALL_C(0,0x01D5,0x18,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SRAO       - Read the digital value of the supply voltage of the PC-1600 main unit. \n"));
//                                                        pCPU->logsw=1;    //
//                                                        pCPU->Check_Log();
    break;
    FUNC_CALL_C(0,0x01D5,0x19,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SRA1       - Read the digital value of the voltage input at the analog input port. \n"));
//    pCPU->logsw=1;    //
//    pCPU->Check_Log();
    break;
    FUNC_CALL_C(0,0x01D5,0x1A,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SRA2       - Read the digital value of the CE-1600P battery voltage.\n"));
//    pCPU->logsw=1;    //
//    pCPU->Check_Log();
    break;

    FUNC_CALL_C(0,0x01D5,0x21,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SRPON      - Read the settings of the mask for the power-on conditions. 96 6F \n"));                break;
    FUNC_CALL_C(0,0x01D5,0x22,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SWAB       - Set the conditions for alarm beep generation. \n"));                break;
    FUNC_CALL_C(0,0x01D5,0x23,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SRAB       - Read the current settings of the alarm beep generation conditions set by SWAB routine. 95 6F\n"));                break;
    FUNC_CALL_C(0,0x01D5,0x24,QT_TR_NOOP("TIMER/ ANALOG PORT - [%1] - SWA1A      - Set the software interrupt trigger levels for a digital value of the analog input port.\n"));                break;
        }
    break;


    case 0x01DE: switch (pZ80->z80.r.c) {
    FUNC_CALL_C(0,0x01DE,0x0F,QT_TR_NOOP("FILES - [%1] - OPEN      - Open a file.\n"));                break;
    FUNC_CALL_C(0,0x01DE,0x10,QT_TR_NOOP("FILES - [%1] - CLOSE     - Close a file.\n"));                break;
    FUNC_CALL_C(0,0x01DE,0x11,QT_TR_NOOP("FILES - [%1] - SEARCH FIRST- Search for the first file.\n"));                break;
    FUNC_CALL_C(0,0x01DE,0x12,QT_TR_NOOP("FILES - [%1] - SEARCH NEXT - Search for the next file.\n"));                break;
    FUNC_CALL_C(0,0x01DE,0x13,QT_TR_NOOP("FILES - [%1] - DELETE    - Delete a file.\n"));                break;
    FUNC_CALL_C(0,0x01DE,0x14,QT_TR_NOOP("FILES - [%1] - SEQUENTIAL RD- Read data sequentiaily from a file.\n"));                break;
    FUNC_CALL_C(0,0x01DE,0x15,QT_TR_NOOP("FILES - [%1] - SEQUENTIAL WR- Write data sequentially te o file.\n"));                break;
    FUNC_CALL_C(0,0x01DE,0x16,QT_TR_NOOP("FILES - [%1] - CREATE FILE  - Create a file.\n"));
//                                                        pCPU->logsw=1;
//                                                        pCPU->Check_Log();
    break;
    FUNC_CALL_C(0,0x01DE,0x17,QT_TR_NOOP("FILES - [%1] - RENAME FILE  - Rename e file.\n"));                break;
    FUNC_CALL_C(0,0x01DE,0x1A,QT_TR_NOOP("FILES - [%1] - SET DMA      - Set a transfer- address.\n"));                break;
    FUNC_CALL_C(0,0x01DE,0x1B,QT_TR_NOOP("FILES - [%1] - GET ALLOC    - Get the file device information such as the number cf empty clusters.\n"));                break;
    FUNC_CALL_C(0,0x01DE,0x1E,QT_TR_NOOP("FILES - [%1] - SET ATTRS    - Set file attributes.\n"));                break;
    }
    break;


    FUNC_CALL(0,0x0196,QT_TR_NOOP("[%1] - SLOT1MAP   - \n"));                break;
    FUNC_CALL(0,0x0199,QT_TR_NOOP("[%1] - SLOT2MAP   - \n"));                break;
    FUNC_CALL(0,0x00E8,QT_TR_NOOP("[%1] - SLOTST     - \n"));
//                                                    pCPU->logsw=1;
//                                                    pCPU->Check_Log();
    break;


//    FUNC_CALL(3,0x4051,QT_TR_NOOP("[%1] - KEYSTRBSCAN- \n"));                break;
    FUNC_CALL(3,0x4054,QT_TR_NOOP("[%1] - KEYCODCNV  - \n"));                break;
//    FUNC_CALL(6,0x80B7,QT_TR_NOOP("[%1] - KEYSCAN    - \n"));                break;


    FUNC_CALL(4,0x4020,QT_TR_NOOP("PRINTER - [%1] - PCHEK    - Check whether the printer is ready.\n")); break;
    FUNC_CALL(4,0x4023,QT_TR_NOOP("PRINTER - [%1] - POUT     - Send one byte of character code to the printer\n")); break;
    FUNC_CALL(4,0x4026,QT_TR_NOOP("PRINTER - [%1] - PKOUT    - Send one byte of character code to the printer\n")); break;

    case 0x4008: switch (pZ80->z80.r.c) {
    FUNC_CALL_C(4,0x4008,0x00,QT_TR_NOOP("PRINTER - [%1] - PINIT      - Initialize the printer.\n"));                break;
    FUNC_CALL_C(4,0x4008,0x01,QT_TR_NOOP("PRINTER - [%1] - PTEXT      - Set the printer in the text mode.\n"));                break;
    FUNC_CALL_C(4,0x4008,0x02,QT_TR_NOOP("PRINTER - [%1] - PGRAPH      - Set the printer in the graphics mode.\n"));                break;
    FUNC_CALL_C(4,0x4008,0x03,QT_TR_NOOP("PRINTER - [%1] - PCSIZE      - Set the character size\n"));                break;
    FUNC_CALL_C(4,0x4008,0x04,QT_TR_NOOP("PRINTER - [%1] - PCOLOR      - Set the printer color\n"));                break;
    FUNC_CALL_C(4,0x4008,0x06,QT_TR_NOOP("PRINTER - [%1] - PWIDTH      - Set the une width (characters/line).\n"));                break;
    FUNC_CALL_C(4,0x4008,0x07,QT_TR_NOOP("PRINTER - [%1] - PLEFTM      - Set the ieft marg(n.\n"));                break;
    FUNC_CALL_C(4,0x4008,0x08,QT_TR_NOOP("PRINTER - [%1] - PPITCH      - Set the character pitch and the fine height.\n"));                break;
    FUNC_CALL_C(4,0x4008,0x09,QT_TR_NOOP("PRINTER - [%1] - PPAPER      - Set the paper type.\n"));                break;
    FUNC_CALL_C(4,0x4008,0x0a,QT_TR_NOOP("PRINTER - [%1] - PSCRL      - Sot the printing area on paper in the Y (vertical) direction.\n"));                break;
    FUNC_CALL_C(4,0x4008,0x0b,QT_TR_NOOP("PRINTER - [%1] - PEOL      - Define the printer action for e CR code (ODH).\n"));                break;
    FUNC_CALL_C(4,0x4008,0x0c,QT_TR_NOOP("PRINTER - [%1] - PZONE      - Set the print zone length for LPRINT command.\n"));                break;
    FUNC_CALL_C(4,0x4008,0x0d,QT_TR_NOOP("PRINTER - [%1] - PPENUP      - Lift up or push down the pen.\n"));                break;
    FUNC_CALL_C(4,0x4008,0x0e,QT_TR_NOOP("PRINTER - [%1] - PROTATE      - Set the direction of the print characters.\n"));                break;


    } break;
    }

}

void Cpc1600::LoadSIO(void) {
    fclose(fp_CRVA);
        // Display the Open dialog box.
        QString ofn = QFileDialog::getOpenFileName(
                        mainwindow,
                        "Choose a file",
                        ".",
                        "Text Files (*.*)");

        if (ofn.isEmpty())
        {
            return ;
        }

        char * str = qstrdup(ofn.toLocal8Bit());

        if ((fp_CRVA = fopen(str,"r"))==NULL) {
            MSG_ERROR(tr("Failed to open file"));
            return ;
        }

}

void Cpc1600::Hack_CRVA(void){
    // Is a file open ?
    // If no open it
    int c;

    if (fp_CRVA == 0) {
            return;
    }
    else {
        c=fgetc(fp_CRVA);
        if (c==0x0A) c=fgetc(fp_CRVA);
    }
    if (c==EOF)
    {
        fclose(fp_CRVA);
        fp_CRVA = 0;
        c = 0x1A;
    }
    if (fp_log) fprintf(fp_log,"SEND DATA [%02x]\n",c);
    ((CZ80 *)pCPU)->z80.r.a = c;
    ((CZ80 *)pCPU)->z80retn(&((CZ80 *)pCPU)->z80);

}

FILE *fp_tmp2;
void Cpc1600::ReadQuarterTape(void){}


INLINE bool Cpc1600::lh5810_write(void)
{
//	AddLog(LOG_FUNC,"Cpc1600::lh5810_write");

    pLH5810->SetReg(LH5810_RESET,	pPC->pCPU->imem[0x14]);
    pLH5810->SetReg(LH5810_U  ,	pPC->pCPU->imem[0x15]);
    pLH5810->SetReg(LH5810_L,	pPC->pCPU->imem[0x16]);

    pLH5810->SetReg(LH5810_OPC,	pPC->pCPU->imem[0x18]);
    pLH5810->SetReg(LH5810_G  ,	pPC->pCPU->imem[0x19]);
    pLH5810->SetReg(LH5810_MSK,	pPC->pCPU->imem[0x1A]);
    pLH5810->SetReg(LH5810_IF ,	pPC->pCPU->imem[0x1B]);
    pLH5810->SetReg(LH5810_DDA,	pPC->pCPU->imem[0x1C]);
    pLH5810->SetReg(LH5810_DDB,	pPC->pCPU->imem[0x1D]);
    pLH5810->SetReg(LH5810_OPA,	pPC->pCPU->imem[0x1E]);
    pLH5810->SetReg(LH5810_OPB,	pPC->pCPU->imem[0x1F]);


    return(1);
}

INLINE bool Cpc1600::lh5810_read(void)
{
//    pLH5810->step();
//	AddLog(LOG_FUNC,"Cpc1600::lh5810_read");



    pPC->pCPU->imem[0x15] = pLH5810->GetReg(LH5810_U);
    pPC->pCPU->imem[0x16] = pLH5810->GetReg(LH5810_L);

    pPC->pCPU->imem[0x18] = pLH5810->GetReg(LH5810_OPC);
    pPC->pCPU->imem[0x19] = pLH5810->GetReg(LH5810_G);
    pPC->pCPU->imem[0x1A] = pLH5810->GetReg(LH5810_MSK);
    pPC->pCPU->imem[0x1B] = pLH5810->GetReg(LH5810_IF);
    pPC->pCPU->imem[0x1C] = pLH5810->GetReg(LH5810_DDA);
    pPC->pCPU->imem[0x1D] = pLH5810->GetReg(LH5810_DDB);
    pPC->pCPU->imem[0x1E] = pLH5810->GetReg(LH5810_OPA);
    pPC->pCPU->imem[0x1F] = pLH5810->GetReg(LH5810_OPB);
    if (pPC->pCPU->imem[0x1F] == 0x20) {
        pPC->pCPU->imem[0x1F] = 0x20;
    }

    return(1);
}

bool Cpc1600::Mem_Mirror(DWORD *d)
{
    // LH5803 mirror
    if (!masterCPU)
    {
        if ( (*d>=0x7400) && (*d<=0x744F) )	{ *d+=0x200; return(1); }
        if ( (*d>=0x7500) && (*d<=0x754F) )	{ *d+=0x200; return(1); }
    }


    return(1);
}


bool Cpc1600::Chk_Adr(DWORD *d,DWORD data)
{
    Mem_Mirror(d);


    if (masterCPU)
    {
        if ( (*d>=0x0000) && (*d<=0x3FFF) ) { *d += bank1 * 0x10000; return false; }
        if ( (*d>=0x4000) && (*d<=0x7FFF) )
        {
            *d += bank2 * 0x10000;
            if (!(pCPU->imem[0x3D] & 0x04) && (bank2 == 3)) { *d -= 0x4000; }
            return false;
        }
        if ( (*d>=0x8000) && (*d<=0xBFFF) )
        {
            bool ret = false;
            int rambank = pCPU->imem[0x28];

            int subBank = 0;
            switch (bank3)
            {
            case 0:
                if ( (*d>=0xB000) && (*d<=0xBFFF)) subBank = 0;
                if ( (*d>=0xA000) && (*d<=0xAFFF)) subBank = 1;
                if ( (*d>=0x8000) && (*d<=0x9FFF)) subBank = 2;
                switch (subBank)
                {
                case 0: ret = (S1_EXTENSION_CE151_CHECK | S1_EXTENSION_CE155_CHECK | S1_EXTENSION_CE159_CHECK | S1_EXTENSION_CE161_CHECK | S1_EXTENSION_CE1600M_CHECK | S1_EXTENSION_CE1601M_CHECK); break;
                case 1: ret = (S1_EXTENSION_CE155_CHECK | S1_EXTENSION_CE159_CHECK | S1_EXTENSION_CE161_CHECK | S1_EXTENSION_CE1600M_CHECK | S1_EXTENSION_CE1601M_CHECK); break;
                case 2: ret = (S1_EXTENSION_CE161_CHECK | S1_EXTENSION_CE1600M_CHECK | S1_EXTENSION_CE1601M_CHECK); break;
                }
                return ret;
            case 1: ret =  (S1_EXTENSION_CE1600M_CHECK | S1_EXTENSION_CE1601M_CHECK);
                *d += 0x10000;
                return ret;

            case 2:
            case 3: switch (rambank)
                    {
                    case 0: switch (bank3) {
                            case 2: ret =  (S2_EXTENSION_CE161_CHECK | S2_EXTENSION_CE1600M_CHECK | S2_EXTENSION_CE1601M_CHECK | S2_EXTENSION_CE16096_CHECK | S2_EXTENSION_CE16128_CHECK |S2_EXTENSION_CE16160_CHECK|S2_EXTENSION_CE16192_CHECK|S2_EXTENSION_CE16224_CHECK | S2_EXTENSION_CE16256_CHECK); break;
                            case 3: ret =  (S2_EXTENSION_CE1600M_CHECK | S2_EXTENSION_CE1601M_CHECK | S2_EXTENSION_CE16096_CHECK | S2_EXTENSION_CE16128_CHECK |S2_EXTENSION_CE16160_CHECK|S2_EXTENSION_CE16192_CHECK|S2_EXTENSION_CE16224_CHECK | S2_EXTENSION_CE16256_CHECK);               break;
                            }
                        *d += bank3 * 0x10000;
                        return ret;
                        break;
                    case 1: ret = (S2_EXTENSION_CE1601M_CHECK | S2_EXTENSION_CE16096_CHECK | S2_EXTENSION_CE16128_CHECK |S2_EXTENSION_CE16160_CHECK|S2_EXTENSION_CE16192_CHECK|S2_EXTENSION_CE16224_CHECK | S2_EXTENSION_CE16256_CHECK ); break;
                    case 2: ret = (S2_EXTENSION_CE16096_CHECK | S2_EXTENSION_CE16128_CHECK |S2_EXTENSION_CE16160_CHECK|S2_EXTENSION_CE16192_CHECK|S2_EXTENSION_CE16224_CHECK | S2_EXTENSION_CE16256_CHECK ); break;
                    case 3: ret = (S2_EXTENSION_CE16128_CHECK | S2_EXTENSION_CE16160_CHECK|S2_EXTENSION_CE16192_CHECK|S2_EXTENSION_CE16224_CHECK | S2_EXTENSION_CE16256_CHECK ); break;
                    case 4: ret = (S2_EXTENSION_CE16160_CHECK | S2_EXTENSION_CE16192_CHECK|S2_EXTENSION_CE16224_CHECK | S2_EXTENSION_CE16256_CHECK ); break;
                    case 5: ret = (S2_EXTENSION_CE16192_CHECK | S2_EXTENSION_CE16224_CHECK | S2_EXTENSION_CE16256_CHECK ); break;
                    case 6: ret = (S2_EXTENSION_CE16224_CHECK | S2_EXTENSION_CE16256_CHECK ); break;
                    case 7: ret = (S2_EXTENSION_CE16256_CHECK ); break;
                    }

                    *d += 0xA0000 + (rambank-1) * 0x8000 + (bank3 - 2) * 0x4000 - 0x8000;
                    return ret;
                    break;
            case 4:
            case 5:
            case 6:
            case 7: *d += bank3 * 0x10000;
                    return false;
                    break;
            }
        }
        if ( (*d>=0xC000) && (*d<=0xFFFF) ) { *d += bank4 * 0x10000; return ( bank4 ? false : true); }
    }
    else
    {
        //if (pCPU->fp_log) fprintf(pCPU->fp_log,"Check adr [%05x]",*d);

        if ( (*d>=0x0000) && (*d<=0x1FFF) )	{ *d+=0x8000;return(EXTENSION_CE161_CHECK); }						// ROM area(0000-3FFF) 16K
        if ( (*d>=0x2000) && (*d<=0x37FF) )	{ *d+=0x8000;return(EXTENSION_CE161_CHECK | EXTENSION_CE159_CHECK); }	// ROM area(0000-3FFF) 16K
        if ( (*d>=0x3800) && (*d<=0x3FFF) )	{ *d+=0x8000;return(EXTENSION_CE161_CHECK | EXTENSION_CE159_CHECK | EXTENSION_CE155_CHECK); }		// ROM area(0000-3FFF) 16K
        if ( (*d>=0x4000) && (*d<=0x57FF) )	{ *d+=0x8000;return(1); }										// RAM area(0000-3FFF) 16K
        if ( (*d>=0x5800) && (*d<=0x67FF) )	{ *d+=0x8000;return(EXTENSION_CE155_CHECK | EXTENSION_CE151_CHECK); }	// RAM area(0000-3FFF) 16K
        if ( (*d>=0x6800) && (*d<=0x6FFF) )	{ *d+=0x8000;return(EXTENSION_CE155_CHECK); }						// RAM area(0000-3FFF) 16K
        if ( (*d>=0x7000) && (*d<=0x7FFF) ) { *d+=0x8000;return(1);}										// RAM area(7800-7BFF)
        if ( (*d>=0x8000) && (*d<=0x9FFF) ) { *d+=0x70000;return(0); }										// RAM area(4000-7FFFF)
        if ( (*d>=0xA000) && (*d<=0xBFFF) ) { *d+=0x70000;return(0); }										// RAM area(4000-7FFFF)
        if ( (*d>=0xC000) && (*d<=0xFFFF) ) { *d+=0x70000;return(0); }										// RAM area(4000-7FFFF)
        if ( (*d>=0x1A030)&&(*d<=0x1A03F) )
        {
            if (*d == 0x1A038) cpuSwitchPending = true;
            *d+=0x70000; return (1);
        }
        if ( (*d>=0x1B000)&&(*d<=0x1B00F) ) { ce150_Access = TRUE;*d+=0x70000;	return(1); }
        if ( (*d>=0x1F000)&&(*d<=0x1F00F) )	{ //lh5810_Access = true;
                                              //if (*d==0x1F006) pLH5810->New_L=true;
                                              *d+=0x70000;return(1);}										// I/O area(LH5810)

        if ( (*d>=0x1D000)&&(*d<=0x1D00F) ) { *d+=0x70000;return(1); }
        if ( (*d>=0x18000)&&(*d<=0x1800F) ) { *d+=0x70000;return(1); }
        *d+=0x70000;return(1);
    }

    // else it's ROM
    return (false);
}




bool Cpc1600::Chk_Adr_R(DWORD *d,DWORD data)
{
    Mem_Mirror(d);

    if (masterCPU)
    {
        if ( (*d>=0x0000) && (*d<=0x3FFF) ) { *d += bank1 * 0x10000; return true; }
        if ( (*d>=0x4000) && (*d<=0x7FFF) )
        {
            *d += bank2 * 0x10000;
            if (!ce1600_connected && ((bank2==4)||(bank2==5))) *d -= 0x4000;

            //*d -= (pCPU->imem[0x3D] & 0x04) ? 0x00 : 0x4000;
            if (!(pCPU->imem[0x3D] & 0x04) && (bank2 == 3))
            {
                *d -= 0x4000;
//                if (pCPU->fp_log) fprintf(pCPU->fp_log,"Acces Bank 3b\n");
            }
            return true;
        }
        if ( (*d>=0x8000) && (*d<=0xBFFF) ) {
            int rambank = pCPU->imem[0x28];
            switch (bank3)
            {
            case 0: break;
            case 1: *d += 0x10000; break;
            case 2:
            case 3:
                if (rambank == 0)
                {
                    *d += bank3 * 0x10000;
                }
                else
                {
                    *d += 0xA0000 + (rambank -1 ) * 0x8000 + (bank3 - 2) * 0x4000 - 0x8000;
                }
                break;
            case 4:
            case 5:
            case 6:
            case 7: *d += bank3 * 0x10000; break;
            }
            return true;
        }
        if ( (*d>=0xC000) && (*d<=0xFFFF) ) { *d += bank4 * 0x10000; return true; }
    }
    else
    {
        if ( (*d>=0x0000) && (*d<=0x7FFF) )	{ *d+=0x8000;return(1); }						// ROM area(0000-3FFF) 16K

        if ( (*d>=0x8000) && (*d<=0x9FFF) ) { *d+=0x70000;return(1); }										// RAM area(4000-7FFFF)
        if ( (*d>=0xA000) && (*d<=0xBFFF) ) { *d+=0x70000;return(1); }										// RAM area(4000-7FFFF)
        if ( (*d>=0xC000) && (*d<=0xFFFF) ) { *d+=0x70000;return(1); }
        //if (pCPU->fp_log) fprintf(pCPU->fp_log,"Check adr_R [%05x] = %02x",*d,mem[*d+0x70000]);
        if ( (*d>=0x1A000) && (*d<=0x1A00F) ) {	*d+=0x70000;	return(1);	}

        if ( (*d>=0x1B000) && (*d<=0x1B00F) ) {	*d+=0x70000;	return(1);	}
        if ( (*d>=0x1D000) && (*d<=0x1D00F) ) {	*d+=0x70000;	return(1);	}
        if ( (*d>=0x1F000) && (*d<=0x1F00F) ) {	*d+=0x70000;	return(1);	}

        if ( (ce150_connected) && (*d>=0xA000) && (*d<=0xBFFF) &&  (pLH5803->lh5801.pv==0) )
        {
            //*d+=0x1A000;
            *d+=0x8A000;
            return(1);
        }
        *d+=0x70000;
    }

    return(1);
}



void Cpc1600::Set_Port(PORTS Port,BYTE data){};
BYTE Cpc1600::Get_Port(PORTS Port){return(0);};

UINT8 Cpc1600::out(UINT8 address,UINT8 value)
{
    if (pCPU->fp_log) {
        pCPU->Regs_Info(1);
        fprintf(pCPU->fp_log,"%-40s   iff=%i OUT [%02X]=%02X\t\t\t%s\n"," ",((CZ80 *)pCPU)->z80.r.iff,address,value,pCPU->Regs_String);
    }    // Manage CPU port modification


    if ((address >= 0x10) && (address <= 0x1f))
    {
        switch (address) {
            // LH5810
        case 0x14: break;
        case 0x15: break;
        case 0x16: break;
        case 0x18: break;
        case 0x19: break;
        case 0x1a: break;
        case 0x1b: break;
        case 0x1c: break;
        case 0x1d: break;
        case 0x1e: break;
        case 0x1f: break;
        }
        if (fp_log) fprintf(fp_log,"OUT [%02X]=%02X\n",address,value);
        lh5810_write(); pLH5810->step();
    }

    switch(address) {
    case 0x20: if (fp_log) fprintf(fp_log,"OUT [%02X]=%02X\n",address,value);
        break;
    case 0x21:  // Send a command to the sub-CPU ( the sub-CPU answer on port 33h)
        // In fact it send the command via he UART parallel port !!!!
        if (fp_log) fprintf(fp_log,"OUT [%02X]=%02X   - SEND //\n",address,value);
        pLU57813P->command(value);
        break;
    case 0x22: //pTC8576P->in(value);
        if (fp_log) fprintf(fp_log,"OUT [%02X]=%02X\n",address,value);
        break;
    case 0x23: //pTC8576P->instruction(value);
        if (fp_log) fprintf(fp_log,"OUT [%02X]=%02X\n",address,value);
        break;
    case 0x28:
        if (fp_log) fprintf(fp_log,"OUT [%02X]=%02X\n",address,value);
        break;

    case 0x30: break;
        // RomBank = port 31h
    case 0x31:
        bank1 = value & 0x01;
        bank2 = (value >> 1) & 0x07;
        bank3 = (value >> 4) & 0x07;
        bank4 = (value >> 7) & 0x01;
        if (pCPU->fp_log) fprintf(pCPU->fp_log," r1=%i   r2=%i   r3=%i   r4=%i  [3d]=%02x\n",bank1,bank2,bank3,bank4,pCPU->imem[0x3d]);
        break;

    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36: break;

    case 0x37:  break;

    case 0x38: // switch control between CPUs
        cpuSwitchPending = true;
        break;
    case 0x39:
    case 0x3a:
    case 0x3b: break;
    case 0x3c: if (fp_log) fprintf(fp_log,"OUT [%02X]=%02X\n",address,value);
        break;

    case 0x3d:
    case 0x3e:
    case 0x3f: break;



        //////////////////////////////////////////////////////////
       // DISPLAY CONTROL
      //////////////////////////////////////////////////////////
    case 0x50:  pHD61102_1->instruction(value);
                pHD61102_2->instruction(value);
                pLCDC->Refresh = true;
                break;
    case 0x54:  pHD61102_2->instruction(value);
                pLCDC->Refresh = true;
                break;
    case 0x58:  pHD61102_1->instruction(value);
                pLCDC->Refresh = true;
                break;

    case 0x52:  pHD61102_1->instruction(0x100|value);
                pHD61102_2->instruction(0x100|value);
                pLCDC->Refresh = true;
                break;
    case 0x56:  pHD61102_2->instruction(0x100|value);
                pLCDC->Refresh = true;
                break;
    case 0x5a:  pHD61102_1->instruction(0x100|value);
                pLCDC->Refresh = true;
                break;

    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:if (fp_log) fprintf(fp_log,"PRINTER - OUT [%02X]=%02X\n",address,value);
        break;
    }

    return 1;
}



UINT8 Cpc1600::in(UINT8 address)
{
    if (pCPU->fp_log) {
        pCPU->Regs_Info(0);
        fprintf(pCPU->fp_log,"%-40s   IN [%02X]\t\t\t%s\n"," ",address,pCPU->Regs_String);
    }
    switch (address) {
    case 0x20: if (fp_log) fprintf(fp_log,"IN [%02X]=%02X\n",address,pCPU->imem[address]);
        break;
    case 0x21: if (fp_log) fprintf(fp_log,"IN [%02X]=%02X\n",address,pCPU->imem[address]);
        break;
    case 0x22:
        pCPU->imem[address] = 0;//pTC8576P->get_ssr();
//        if (fp_log) fprintf(fp_log,"IN [%02X]=%02X\n",address,pCPU->imem[address]);
        break;
    case 0x23:
        pCPU->imem[address] = 0;//pTC8576P->get_psr();
        if (fp_log) fprintf(fp_log,"IN [%02X]=%02X\n",address,pCPU->imem[address]);
        break;// bit 5 to 0
    case 0x28:
        if (fp_log) fprintf(fp_log,"IN [%02X]=%02X\n",address,pCPU->imem[address]);
        break;

    case 0x14:
    case 0x15:
    case 0x16:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:  if (fp_log) fprintf(fp_log,"IN [%02X]=%02X\n",address,pCPU->imem[address]);
                pLH5810->step();lh5810_read(); break;

    case 0x33: if (pLU57813P->output_pending) pCPU->imem[address] = pLU57813P->Get_reg_out(); break;

    case 0x37:  {
            BYTE c = getKey();
                pCPU->imem[address] = c;
//                if (pCPU->fp_log) fprintf(pCPU->fp_log,"Last Key = %02x   Get key = %02x\n",c,pKEYB->LastKey);
            }
                break;


      //////////////////////////////////////////////////////////
     // DISPLAY CONTROL
    //////////////////////////////////////////////////////////
    case 0x55: pCPU->imem[address] = pHD61102_2->instruction(0x200); break;
    case 0x59: pCPU->imem[address] = pHD61102_1->instruction(0x200); break;

    case 0x57: pCPU->imem[address] = pHD61102_2->instruction(0x300); break;
    case 0x5b: pCPU->imem[address] = pHD61102_1->instruction(0x300); break;



    }

    return 1;

}



void Cpc1600::Regs_Info(UINT8 Type)
{

    strcat(Regs_String,	"");
    //CpcXXXX::Regs_Info(0);
    //pLH5810->Regs_Info(0);
    pCPU->Regs_Info(1);
    //strcat(Regs_String,	"\r\n");
    //strcat(Regs_String,pLH5810->Regs_String);
}

#define LOGPC() \
        if (fp_log) { \
            pPC->pCPU->Regs_Info(0); \
            fprintf(fp_log,"IN [%02X]\t%s\n",address,pPC->pCPU->Regs_Info); \
                }

#define SIO_GNDP	1
#define SIO_SD 		2   // TXD
#define SIO_RD		3   // RXD
#define SIO_RS		4   // RTS
#define SIO_CS		5   // CTS
#define SIO_DS		6   // DSR
#define SIO_GND		7   // SG
#define SIO_CD		8
#define SIO_CI		9
#define SIO_VC1		10
#define SIO_RR		11  // NC
#define SIO_PAK		12  // NC
#define SIO_VC2		13  // NC
#define SIO_ER		14  // DTR
#define SIO_PRQ		15  // NC

bool Cpc1600::Set_Connector(void)
{
    pCONNECTOR->Set_pin(1	,0);

    // MANAGE SERIAL CONNECTOR
    // TO DO
//    pSIOCONNECTOR->Set_pin(SIO_SD	,READ_BIT(v,b));



    return true;
}

bool Cpc1600::Get_Connector(void)
{
    ce1600_connected = pCONNECTOR->Get_pin(1);

    // MANAGE SERIAL CONNECTOR
    // TO DO
    PUT_BIT(pCPU->imem[0x23],2,pSIOCONNECTOR->Get_pin(SIO_DS));
    PUT_BIT(pCPU->imem[0x23],1,pSIOCONNECTOR->Get_pin(SIO_CD));
    PUT_BIT(pCPU->imem[0x23],0,!pSIOCONNECTOR->Get_pin(SIO_CS));

    PUT_BIT(pCPU->imem[0x23],5,!pSIOCONNECTOR->Get_pin(SIO_CI));

    // A/D Connector
    pLU57813P->SetDigital(pADCONNECTOR->Get_values());

    return true;
}


void Cpc1600::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    BuildContextMenu(&menu);

    menu.addSeparator();

        //-----------------------------------------------//
       // Specific Tape menu for the PC1500				//
      // I use a hack to manage tape read and write	   //
     // as i need to improve LH5810 serial emulation  //
    //-----------------------------------------------//
    QMenu * menuTape = menu.addMenu(tr("SIO"));
        menuTape->addAction(tr("Load..."),this,SLOT(LoadSIO()));
        menuTape->addAction(tr("Play"),pce152,SLOT(Play()));
        menuTape->addAction(tr("Stop"),pce152,SLOT(StopPlay()));
        menuTape->addAction(tr("Record"),pce152,SLOT(RecTape()));
    //--------------------------------------------------

    menu.exec(event->globalPos () );
}

bool	CLH5810_PC1600::init(void)
{

    return(1);
}



#define KEY(c)	( toupper(pKEYB->LastKey) == toupper(c) )
BYTE Cpc1600::getKey()
{

    UINT8 data=0;

    if (pKEYB->LastKey)
    {
        BYTE ks1 = pLH5810->GetReg(LH5810_OPB);
        if (!( ks1 & 0x40)) {
            if (KEY(K_CTRL))		data|=0x01;
            if (KEY(K_KBII))		data|=0x02;
            if (KEY(K_BS))			data|=0x04;
        }
    }
    BYTE ks = pKEYB->Get_KS() ^ 0xff;
    if ((pKEYB->LastKey) && ks )
    {     
        if (ks&1) {
            if (KEY('2'))			data|=0x01;
            if (KEY('5'))			data|=0x02;
            if (KEY('8'))			data|=0x04;
            if (KEY('H'))			data|=0x08;
            if (KEY(K_SHT))			data|=0x10;
            if (KEY('Y'))			data|=0x20;
            if (KEY('N'))			data|=0x40;
            if (KEY(K_UA))			data|=0x80;			// UP ARROW
        }
        if (ks&2) {
            if (KEY('.'))			data|=0x01;
            if (KEY('-'))			data|=0x02;
            if (KEY(K_OF))			data|=0x04;			// OFF
            if (KEY('S'))			data|=0x08;
            if (KEY(K_F1))			data|=0x10;
            if (KEY('W'))			data|=0x20;
            if (KEY('X'))			data|=0x40;
            if (KEY(K_RSV))			data|=0x80;
        }
        if (ks&4) {
            if (KEY('1'))			data|=0x01;
            if (KEY('4'))			data|=0x02;
            if (KEY('7'))			data|=0x04;
            if (KEY('J'))			data|=0x08;
            if (KEY(K_F5))			data|=0x10;
            if (KEY('U'))			data|=0x20;
            if (KEY('M'))			data|=0x40;
            if (KEY('0'))			data|=0x80;
        }
        if (ks&8) {
            if (KEY(')'))			data|=0x01;
            if (KEY('L'))			data|=0x02;
            if (KEY('O'))			data|=0x04;
            if (KEY('K'))			data|=0x08;
            if (KEY(K_F6))			data|=0x10;
            if (KEY('I'))			data|=0x20;
            if (KEY('('))			data|=0x40;
            if (KEY(K_RET))			data|=0x80;
        }
        if (ks&0x10) {
            if (KEY('+'))			data|=0x01;			// +
            if (KEY('*'))			data|=0x02;			// *
            if (KEY('/'))			data|=0x04;			// /
            if (KEY('D'))			data|=0x08;
            if (KEY(K_F2))			data|=0x10;			// Key F2
            if (KEY('E'))			data|=0x20;
            if (KEY('C'))			data|=0x40;
            if (KEY(K_RCL))			data|=0x80;
        }
        if (ks&0x20) {
            if (KEY('='))			data|=0x01;			// =
            if (KEY(K_LA))			data|=0x02;			// LEFT ARROW
            if (KEY('P'))			data|=0x04;
            if (KEY('F'))			data|=0x08;
            if (KEY(K_F3))			data|=0x10;
            if (KEY('R'))			data|=0x20;
            if (KEY('V'))			data|=0x40;
            if (KEY(' '))			data|=0x80;
        }
        if (ks&0x40) {
            if (KEY(K_RA))			data|=0x01;			// R ARROW
            if (KEY(K_MOD))			data|=0x02;			// MODE
            if (KEY(K_CLR))			data|=0x04;			// CLS
            if (KEY('A'))			data|=0x08;
            if (KEY(K_DEF))			data|=0x10;
            if (KEY('Q'))			data|=0x20;
            if (KEY('Z'))			data|=0x40;
            if (KEY(K_SML))			data|=0x80;
        }
        if (ks&0x80) {
            if (KEY('3'))			data|=0x01;
            if (KEY('6'))			data|=0x02;
            if (KEY('9'))			data|=0x04;
            if (KEY('G'))			data|=0x08;
            if (KEY(K_F4))			data|=0x10;			// Key F4
            if (KEY('T'))			data|=0x20;
            if (KEY('B'))			data|=0x40;
            if (KEY(K_DA))			data|=0x80;			// DOWN ARROW
        }

//        if (fp_log) fprintf(fp_log,"Read key [%02x]: strobe=%02x result=%02x\n",pKEYB->LastKey,ks,data^0xff);
        //SetReg(LH5810_OPA,data^0xff);
    }
    return data^0xff;

}


bool CLH5810_PC1600::step()
{
    ////////////////////////////////////////////////////////////////////
    //	INT FROM connector to IRQ
    ////////////////////////////////////////////////////////////////////
//    IRQ = pPC->pCONNECTOR->Get_pin(30);

    ////////////////////////////////////////////////////////////////////
    //	Timer pulse to PB5
    ////////////////////////////////////////////////////////////////////
//    SetRegBit(LH5810_OPB,5,pPC->pTIMER->GetTP( ((Cpc1600 *)pPC)->tpIndex64));    // 1/64s signal le timer doit etre en mode 4
    SetRegBit(LH5810_OPB,5,pPC->pTIMER->GetTP( ((Cpc1600 *)pPC)->pLU57813P->Get_tpIndex64()));    // 1/64s signal le timer doit etre en mode 4

    ////////////////////////////////////////////////////////////////////
    //	ON/Break
    ////////////////////////////////////////////////////////////////////

    SetRegBit(LH5810_OPB,7,((Cpc1600 *)pPC)->pLU57813P->Get_Kon());



      //----------------------//
     // Standard LH5810 STEP //
    //----------------------//
    CLH5810::step();


    pPC->pKEYB->Set_KS(GetReg(LH5810_OPA));
//    if (pPC->fp_log) fprintf(pPC->fp_log,"Set_KS( %02x )\n",GetReg(LH5810_OPA));


    return(1);
}

