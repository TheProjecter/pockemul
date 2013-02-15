#include "upd16434.h"
#include "pcxxxx.h"
#include "cpu.h"
#include "Log.h"

/*
STOP        Set Stop Mode                       01
DISP OFF    Display Off                         08
DISP ON     Display On                          09
SFF         Set Frame Frequency                 10 to 14
SMM         Set Multiplexing Mode               18 to 1F
BRESET      Bit Reset                           20 to 3F
BSET        Bit set                             40 to 5F
SRM         Set Read Mode                       60 to 63
SWM         Set Write Mode                      64 to 67
SORM        Set OR Mode                         68 to 6B
SANDM       Set AND Mode                        6C to 6F
SCML        Set Character Mode with Left entry  71
SCMR        Set Character Mode with Right entry 72
CLCURS      Clear Cursor                        7C
WRCURS      Write Cursor                        7D
LDPI        Load Data Pointer with Immediate    80 to B1, C0 to F1
*/

#define MASK_STOP       0x01
#define MASK_DISPOFF    0x08
#define MASK_DISPON     0x09
#define MASK_SFF        0x10
#define MASK_SMM        0x18
#define MASK_BRESET     0x20
#define MASK_BSET       0x40
#define MASK_SRM        0x60
#define MASK_SWM        0x64
#define MASK_SORM       0x68
#define MASK_SANDM      0x6C
#define MASK_SCML       0x71
#define MASK_SCMR       0x72
#define MASK_CLCURS     0x7C
#define MASK_WRCURS     0x7D
#define MASK_LDPI       0x80


CUPD16434::CUPD16434(CpcXXXX *parent)
{
    pPC = parent;

    for (int i = 0 ; i < 0x80 ; i++)
    {
        info.imem[i] = 0;
    }

    info.on_off = 0;
    info.dataPointer = 0;
    info.Xadr = 0;
    info.Yadr = 0;
    info.status = 0;
    updated = false;
}

CUPD16434::~CUPD16434() {

}



bool CUPD16434::init()
{
}

bool CUPD16434::exit()
{
}

void CUPD16434::Reset()
{
}

bool CUPD16434::step()
{
}

BYTE CUPD16434::data(quint8 cmd)
{
    info.imem[info.dataPointer] = cmd;
}

BYTE CUPD16434::instruction(quint8 cmd)
{
    if (pPC->pCPU->fp_log)fprintf(pPC->pCPU->fp_log,"UPD16434 CMD: %04x\n",cmd);

    AddLog(LOG_DISPLAY,tr("UPD16434 CMD:%1").arg(cmd,4,16,QChar('0')));

    if ((cmd & MASK_LDPI) == MASK_LDPI ) { return cmd_LDPI(cmd); }
    else
    if ((cmd & MASK_WRCURS) == MASK_WRCURS ) { return cmd_WRCURS(cmd); }
    else
    if ((cmd & MASK_CLCURS) == MASK_CLCURS ) { return cmd_CLCURS(cmd); }
    else
    if ((cmd & MASK_SCMR) == MASK_SCMR ) { return cmd_SCMR(cmd); }
    else
    if ((cmd & MASK_SCML) == MASK_SCML ) { return cmd_SCML(cmd); }
    else
    if ((cmd & MASK_SANDM) == MASK_SANDM ) { return cmd_SANDM(cmd); }
    else
    if ((cmd & MASK_SORM) == MASK_SORM ) { return cmd_SORM(cmd); }
    else
    if ((cmd & MASK_SWM) == MASK_SWM ) { return cmd_SWM(cmd); }
    else
    if ((cmd & MASK_SRM) == MASK_SRM ) { return cmd_SRM(cmd); }
    else
    if ((cmd & MASK_BSET) == MASK_BSET ) { return cmd_BSET(cmd); }
    else
    if ((cmd & MASK_BRESET) == MASK_BRESET ) { return cmd_BRESET(cmd); }
    else
    if ((cmd & MASK_SMM) == MASK_SMM ) { return cmd_SMM(cmd); }
    else
    if ((cmd & MASK_SFF) == MASK_SFF ) { return cmd_SFF(cmd); }
    else
    if ((cmd & MASK_DISPON) == MASK_DISPON ) { return cmd_DISPON(cmd); }
    else
    if ((cmd & MASK_DISPOFF) == MASK_DISPOFF ) { return cmd_DISPOFF(cmd); }
    else
    if ((cmd & MASK_STOP) == MASK_STOP ) { return cmd_STOP(cmd); }

    return 0;
}


BYTE CUPD16434::cmd_LDPI(quint8 cmd)
{
    info.dataPointer = cmd | 0x80;
    return 0;
}

BYTE CUPD16434::cmd_WRCURS(quint8 cmd)
{
}

BYTE CUPD16434::cmd_CLCURS(quint8 cmd)
{
}

BYTE CUPD16434::cmd_SCMR(quint8 cmd)
{
}

BYTE CUPD16434::cmd_SCML(quint8 cmd)
{
}

BYTE CUPD16434::cmd_SANDM(quint8 cmd)
{
}

BYTE CUPD16434::cmd_SORM(quint8 cmd)
{
}

BYTE CUPD16434::cmd_SWM(quint8 cmd)
{
}

BYTE CUPD16434::cmd_SRM(quint8 cmd)
{
}

BYTE CUPD16434::cmd_BSET(quint8 cmd)
{
}

BYTE CUPD16434::cmd_BRESET(quint8 cmd)
{
}

BYTE CUPD16434::cmd_SMM(quint8 cmd)
{
}

BYTE CUPD16434::cmd_SFF(quint8 cmd)
{
}

BYTE CUPD16434::cmd_DISPON(quint8 cmd)
{
}

BYTE CUPD16434::cmd_DISPOFF(quint8 cmd)
{
}

BYTE CUPD16434::cmd_STOP(quint8 cmd)
{
}

