/*****************************************************************************
 *
 *       i8085.c
 *       Portable I8085A emulator V1.2
 *
 *       Copyright (c) 1999 Juergen Buchmueller, all rights reserved.
 *       Partially based on information out of Z80Em by Marcel De Kogel
 *
 * changes in V1.2
 *       - corrected cycle counts for these classes of opcodes
 *         Thanks go to Jim Battle <frustum@pacbell.bet>
 *
 *                                      808x     Z80
 *         DEC A                   5       4    \
 *         INC A                   5       4     \
 *         LD A,B                  5       4      >-- Z80 is faster
 *         JP (HL)                 5       4     /
 *         CALL cc,nnnn: 11/17   10/17  /
 *
 *         INC HL                  5       6    \
 *         DEC HL                  5       6     \
 *         LD SP,HL        5       6      \
 *         ADD HL,BC      10      11       \
 *         INC (HL)       10      11            >-- 8080 is faster
 *         DEC (HL)       10      11       /
 *         IN A,(#)       10      11      /
 *         OUT (#),A      10      11     /
 *         EX (SP),HL     18      19    /
 *
 *       - This source code is released as freeware for non-commercial purposes.
 *       - You are free to use and redistribute this code in modified or
 *         unmodified form, provided you list me in the credits.
 *       - If you modify this source code, you must add a notice to each modified
 *         source file that it has been changed.  If you're a nice person, you
 *         will clearly mark each change too.  :)
 *       - If you wish to use this for commercial purposes, please contact me at
 *         pullmoll@t-online.de
 *       - The author of this copywritten work reserves the right to change the
 *         terms of its usage and license at any time, including retroactively
 *       - This entire notice must remain in the source code.
 *
 *****************************************************************************/
#include <QtCore>

#define I i85stat.regs

#define VERBOSE 0

#include "i8085.h"
#include "i8085cpu.h"
#include "i8085daa.h"
#include "pcxxxx.h"
#include "Inter.h"
#include "Debug.h"

#define LOG(x)

Ci8085::Ci8085(CPObject * parent): CCPU(parent)
{
    fn_log="i8085.log";


//    regwidget = (CregCPU*) new Cregsz80Widget(0,this);

}

Ci8085::~Ci8085()
{
}

bool Ci8085::init()
{
    init_tables();
    Check_Log();
    pDEBUG = new Cdebug_i8085(pPC);
    Reset();
    return true;
}

bool Ci8085::exit()
{
    /* nothing to do */
    return true;
}

void Ci8085::change_pc16(quint16 val) {

}


inline quint8 Ci8085::read8( quint16 address)
{
    return (((CpcXXXX *)pPC)->Get_8(address));
}
inline void Ci8085::write8(quint16 address, quint8 value)
{
    ((CpcXXXX *)pPC)->Set_8(address,value);
}

/*
    16bits READ/WRITE
*/
quint16 Ci8085::read16( quint16 address)
{
    return ((CpcXXXX *)pPC)->Get_16(address);

}
void Ci8085::write16( quint16 address, quint16 value)
{
    ((CpcXXXX *)pPC)->Set_16(address,value);
}


quint8 Ci8085::cpu_readport(quint8 address) {
    return pPC->in(address);
}

void Ci8085::cpu_writeport(quint8 address, quint8 x) {
    imem[address] = x;
    pPC->out(address,x);
}

quint8 Ci8085::ROP(void)
{
        return read8(i85stat.regs.PC.w.l++);
}

quint8 Ci8085::ARG(void)
{
        return read8(i85stat.regs.PC.w.l++);
}

quint16 Ci8085::ARG16(void)
{
        quint16 w;
        w  = read8(i85stat.regs.PC.d);
        i85stat.regs.PC.w.l++;
        w += read8(i85stat.regs.PC.d) << 8;
        i85stat.regs.PC.w.l++;
        return w;
}

quint8 Ci8085::RM(quint32 a)
{
        return read8(a);
}

void Ci8085::WM(quint32 a, quint8 v)
{
        write8(a, v);
}

 void Ci8085::illegal(void)
{
#if VERBOSE
        quint16 pc = i85stat.regs.PC.w.l - 1;
        LOG(("i8085 illegal instruction %04X $%02X\n", pc, cpu_readop(pc)));
#endif
}

INLINE void Ci8085::execute_one(int opcode)
{
        switch (opcode)
        {
                case 0x00: i8085_ICount -= 4;   /* NOP  */
                        /* no op */
                        break;
                case 0x01: i8085_ICount -= 10;  /* LXI  B,nnnn */
                        i85stat.regs.BC.w.l = ARG16();
                        break;
                case 0x02: i8085_ICount -= 7;   /* STAX B */
                        WM(i85stat.regs.BC.d, i85stat.regs.AF.b.h);
                        break;
                case 0x03: i8085_ICount -= 5;   /* INX  B */
                        i85stat.regs.BC.w.l++;
                        break;
                case 0x04: i8085_ICount -= 5;   /* INR  B */
                        M_INR(i85stat.regs.BC.b.h);
                        break;
                case 0x05: i8085_ICount -= 5;   /* DCR  B */
                        M_DCR(i85stat.regs.BC.b.h);
                        break;
                case 0x06: i8085_ICount -= 7;   /* MVI  B,nn */
                        M_MVI(i85stat.regs.BC.b.h);
                        break;
                case 0x07: i8085_ICount -= 4;   /* RLC  */
                        M_RLC;
                        break;

                case 0x08: i8085_ICount -= 4;   /* ???? */
                        illegal();
                        break;
                case 0x09: i8085_ICount -= 10;  /* DAD  B */
                        M_DAD(BC);
                        break;
                case 0x0a: i8085_ICount -= 7;   /* LDAX B */
                        i85stat.regs.AF.b.h = RM(i85stat.regs.BC.d);
                        break;
                case 0x0b: i8085_ICount -= 5;   /* DCX  B */
                        i85stat.regs.BC.w.l--;
                        break;
                case 0x0c: i8085_ICount -= 5;   /* INR  C */
                        M_INR(i85stat.regs.BC.b.l);
                        break;
                case 0x0d: i8085_ICount -= 5;   /* DCR  C */
                        M_DCR(i85stat.regs.BC.b.l);
                        break;
                case 0x0e: i8085_ICount -= 7;   /* MVI  C,nn */
                        M_MVI(i85stat.regs.BC.b.l);
                        break;
                case 0x0f: i8085_ICount -= 4;   /* RRC  */
                        M_RRC;
                        break;

                case 0x10: i8085_ICount -= 8;   /* ????  */
                        illegal();
                        break;
                case 0x11: i8085_ICount -= 10;  /* LXI  D,nnnn */
                        i85stat.regs.DE.w.l = ARG16();
                        break;
                case 0x12: i8085_ICount -= 7;   /* STAX D */
                        WM(i85stat.regs.DE.d, i85stat.regs.AF.b.h);
                        break;
                case 0x13: i8085_ICount -= 5;   /* INX  D */
                        i85stat.regs.DE.w.l++;
                        break;
                case 0x14: i8085_ICount -= 5;   /* INR  D */
                        M_INR(i85stat.regs.DE.b.h);
                        break;
                case 0x15: i8085_ICount -= 5;   /* DCR  D */
                        M_DCR(i85stat.regs.DE.b.h);
                        break;
                case 0x16: i8085_ICount -= 7;   /* MVI  D,nn */
                        M_MVI(i85stat.regs.DE.b.h);
                        break;
                case 0x17: i8085_ICount -= 4;   /* RAL  */
                        M_RAL;
                        break;

                case 0x18: i8085_ICount -= 7;   /* ????? */
                        illegal();
                        break;
                case 0x19: i8085_ICount -= 10;  /* DAD  D */
                        M_DAD(DE);
                        break;
                case 0x1a: i8085_ICount -= 7;   /* LDAX D */
                        i85stat.regs.AF.b.h = RM(i85stat.regs.DE.d);
                        break;
                case 0x1b: i8085_ICount -= 5;   /* DCX  D */
                        i85stat.regs.DE.w.l--;
                        break;
                case 0x1c: i8085_ICount -= 5;   /* INR  E */
                        M_INR(i85stat.regs.DE.b.l);
                        break;
                case 0x1d: i8085_ICount -= 5;   /* DCR  E */
                        M_DCR(i85stat.regs.DE.b.l);
                        break;
                case 0x1e: i8085_ICount -= 7;   /* MVI  E,nn */
                        M_MVI(i85stat.regs.DE.b.l);
                        break;
                case 0x1f: i8085_ICount -= 4;   /* RAR  */
                        M_RAR;
                        break;

                case 0x20:
                        if( i85stat.regs.cputype )
                        {
                                i8085_ICount -= 7;              /* RIM  */
                                i85stat.regs.AF.b.h = i85stat.regs.IM;
                        }
                        else
                        {
                                i8085_ICount -= 7;              /* ???  */
                        }
                        break;
                case 0x21: i8085_ICount -= 10;  /* LXI  H,nnnn */
                        i85stat.regs.HL.w.l = ARG16();
                        break;
                case 0x22: i8085_ICount -= 16;  /* SHLD nnnn */
                        i85stat.regs.XX.w.l = ARG16();
                        WM(i85stat.regs.XX.d, i85stat.regs.HL.b.l);
                        i85stat.regs.XX.w.l++;
                        WM(i85stat.regs.XX.d, i85stat.regs.HL.b.h);
                        break;
                case 0x23: i8085_ICount -= 5;   /* INX  H */
                        i85stat.regs.HL.w.l++;
                        break;
                case 0x24: i8085_ICount -= 5;   /* INR  H */
                        M_INR(i85stat.regs.HL.b.h);
                        break;
                case 0x25: i8085_ICount -= 5;   /* DCR  H */
                        M_DCR(i85stat.regs.HL.b.h);
                        break;
                case 0x26: i8085_ICount -= 7;   /* MVI  H,nn */
                        M_MVI(i85stat.regs.HL.b.h);
                        break;
                case 0x27: i8085_ICount -= 4;   /* DAA  */
                        i85stat.regs.XX.d = i85stat.regs.AF.b.h;
                        if (i85stat.regs.AF.b.l & CF) i85stat.regs.XX.d |= 0x100;
                        if (i85stat.regs.AF.b.l & HF) i85stat.regs.XX.d |= 0x200;
                        if (i85stat.regs.AF.b.l & NF) i85stat.regs.XX.d |= 0x400;
                        i85stat.regs.AF.w.l = DAA[i85stat.regs.XX.d];
                        break;

                case 0x28: i8085_ICount -= 7;   /* ???? */
                        illegal();
                        break;
                case 0x29: i8085_ICount -= 10;  /* DAD  H */
                        M_DAD(HL);
                        break;
                case 0x2a: i8085_ICount -= 16;  /* LHLD nnnn */
                        i85stat.regs.XX.d = ARG16();
                        i85stat.regs.HL.b.l = RM(i85stat.regs.XX.d);
                        i85stat.regs.XX.w.l++;
                        i85stat.regs.HL.b.h = RM(i85stat.regs.XX.d);
                        break;
                case 0x2b: i8085_ICount -= 5;   /* DCX  H */
                        i85stat.regs.HL.w.l--;
                        break;
                case 0x2c: i8085_ICount -= 5;   /* INR  L */
                        M_INR(i85stat.regs.HL.b.l);
                        break;
                case 0x2d: i8085_ICount -= 5;   /* DCR  L */
                        M_DCR(i85stat.regs.HL.b.l);
                        break;
                case 0x2e: i8085_ICount -= 7;   /* MVI  L,nn */
                        M_MVI(i85stat.regs.HL.b.l);
                        break;
                case 0x2f: i8085_ICount -= 4;   /* CMA  */
                        i85stat.regs.AF.b.h ^= 0xff;
                        i85stat.regs.AF.b.l |= HF + NF;
                        break;

                case 0x30:
                        if( i85stat.regs.cputype )
                        {
                                i8085_ICount -= 7;              /* SIM  */
                                if ((i85stat.regs.IM ^ i85stat.regs.AF.b.h) & 0x80)
                                        if (i85stat.regs.sod_callback) (*i85stat.regs.sod_callback)(i85stat.regs.AF.b.h >> 7);
                                i85stat.regs.IM &= (IM_SID + IM_IEN + IM_TRAP);
                                i85stat.regs.IM |= (i85stat.regs.AF.b.h & ~(IM_SID + IM_SOD + IM_IEN + IM_TRAP));
                                if (i85stat.regs.AF.b.h & 0x80) i85stat.regs.IM |= IM_SOD;
                        }
                        else
                        {
                                i8085_ICount -= 4;              /* ???  */
                        }
                        break;
                case 0x31: i8085_ICount -= 10;  /* LXI SP,nnnn */
                        i85stat.regs.SP.w.l = ARG16();
                        break;
                case 0x32: i8085_ICount -= 13;  /* STAX nnnn */
                        i85stat.regs.XX.d = ARG16();
                        WM(i85stat.regs.XX.d, i85stat.regs.AF.b.h);
                        break;
                case 0x33: i8085_ICount -= 5;   /* INX  SP */
                        i85stat.regs.SP.w.l++;
                        break;
                case 0x34: i8085_ICount -= 10;  /* INR  M */
                        i85stat.regs.XX.b.l = RM(i85stat.regs.HL.d);
                        M_INR(i85stat.regs.XX.b.l);
                        WM(i85stat.regs.HL.d, i85stat.regs.XX.b.l);
                        break;
                case 0x35: i8085_ICount -= 10;  /* DCR  M */
                        i85stat.regs.XX.b.l = RM(i85stat.regs.HL.d);
                        M_DCR(i85stat.regs.XX.b.l);
                        WM(i85stat.regs.HL.d, i85stat.regs.XX.b.l);
                        break;
                case 0x36: i8085_ICount -= 10;  /* MVI  M,nn */
                        i85stat.regs.XX.b.l = ARG();
                        WM(i85stat.regs.HL.d, i85stat.regs.XX.b.l);
                        break;
                case 0x37: i8085_ICount -= 4;   /* STC  */
                        i85stat.regs.AF.b.l = (i85stat.regs.AF.b.l & ~(HF + NF)) | CF;
                        break;

                case 0x38: i8085_ICount -= 7;   /* ???? */
                        illegal();
                        break;
                case 0x39: i8085_ICount -= 10;  /* DAD SP */
                        M_DAD(SP);
                        break;
                case 0x3a: i8085_ICount -= 13;  /* LDAX nnnn */
                        i85stat.regs.XX.d = ARG16();
                        i85stat.regs.AF.b.h = RM(i85stat.regs.XX.d);
                        break;
                case 0x3b: i8085_ICount -= 5;   /* DCX  SP */
                        i85stat.regs.SP.w.l--;
                        break;
                case 0x3c: i8085_ICount -= 5;   /* INR  A */
                        M_INR(i85stat.regs.AF.b.h);
                        break;
                case 0x3d: i8085_ICount -= 5;   /* DCR  A */
                        M_DCR(i85stat.regs.AF.b.h);
                        break;
                case 0x3e: i8085_ICount -= 7;   /* MVI  A,nn */
                        M_MVI(i85stat.regs.AF.b.h);
                        break;
                case 0x3f: i8085_ICount -= 4;   /* CMF  */
                        i85stat.regs.AF.b.l = ((i85stat.regs.AF.b.l & ~(HF + NF)) |
                                           ((i85stat.regs.AF.b.l & CF) << 4)) ^ CF;
                        break;

                case 0x40: i8085_ICount -= 5;   /* MOV  B,B */
                        /* no op */
                        break;
                case 0x41: i8085_ICount -= 5;   /* MOV  B,C */
                        i85stat.regs.BC.b.h = i85stat.regs.BC.b.l;
                        break;
                case 0x42: i8085_ICount -= 5;   /* MOV  B,D */
                        i85stat.regs.BC.b.h = i85stat.regs.DE.b.h;
                        break;
                case 0x43: i8085_ICount -= 5;   /* MOV  B,E */
                        i85stat.regs.BC.b.h = i85stat.regs.DE.b.l;
                        break;
                case 0x44: i8085_ICount -= 5;   /* MOV  B,H */
                        i85stat.regs.BC.b.h = i85stat.regs.HL.b.h;
                        break;
                case 0x45: i8085_ICount -= 5;   /* MOV  B,L */
                        i85stat.regs.BC.b.h = i85stat.regs.HL.b.l;
                        break;
                case 0x46: i8085_ICount -= 7;   /* MOV  B,M */
                        i85stat.regs.BC.b.h = RM(i85stat.regs.HL.d);
                        break;
                case 0x47: i8085_ICount -= 5;   /* MOV  B,A */
                        i85stat.regs.BC.b.h = i85stat.regs.AF.b.h;
                        break;

                case 0x48: i8085_ICount -= 5;   /* MOV  C,B */
                        i85stat.regs.BC.b.l = i85stat.regs.BC.b.h;
                        break;
                case 0x49: i8085_ICount -= 5;   /* MOV  C,C */
                        /* no op */
                        break;
                case 0x4a: i8085_ICount -= 5;   /* MOV  C,D */
                        i85stat.regs.BC.b.l = i85stat.regs.DE.b.h;
                        break;
                case 0x4b: i8085_ICount -= 5;   /* MOV  C,E */
                        i85stat.regs.BC.b.l = i85stat.regs.DE.b.l;
                        break;
                case 0x4c: i8085_ICount -= 5;   /* MOV  C,H */
                        i85stat.regs.BC.b.l = i85stat.regs.HL.b.h;
                        break;
                case 0x4d: i8085_ICount -= 5;   /* MOV  C,L */
                        i85stat.regs.BC.b.l = i85stat.regs.HL.b.l;
                        break;
                case 0x4e: i8085_ICount -= 7;   /* MOV  C,M */
                        i85stat.regs.BC.b.l = RM(i85stat.regs.HL.d);
                        break;
                case 0x4f: i8085_ICount -= 5;   /* MOV  C,A */
                        i85stat.regs.BC.b.l = i85stat.regs.AF.b.h;
                        break;

                case 0x50: i8085_ICount -= 5;   /* MOV  D,B */
                        i85stat.regs.DE.b.h = i85stat.regs.BC.b.h;
                        break;
                case 0x51: i8085_ICount -= 5;   /* MOV  D,C */
                        i85stat.regs.DE.b.h = i85stat.regs.BC.b.l;
                        break;
                case 0x52: i8085_ICount -= 5;   /* MOV  D,D */
                        /* no op */
                        break;
                case 0x53: i8085_ICount -= 5;   /* MOV  D,E */
                        i85stat.regs.DE.b.h = i85stat.regs.DE.b.l;
                        break;
                case 0x54: i8085_ICount -= 5;   /* MOV  D,H */
                        i85stat.regs.DE.b.h = i85stat.regs.HL.b.h;
                        break;
                case 0x55: i8085_ICount -= 5;   /* MOV  D,L */
                        i85stat.regs.DE.b.h = i85stat.regs.HL.b.l;
                        break;
                case 0x56: i8085_ICount -= 7;   /* MOV  D,M */
                        i85stat.regs.DE.b.h = RM(i85stat.regs.HL.d);
                        break;
                case 0x57: i8085_ICount -= 5;   /* MOV  D,A */
                        i85stat.regs.DE.b.h = i85stat.regs.AF.b.h;
                        break;

                case 0x58: i8085_ICount -= 5;   /* MOV  E,B */
                        i85stat.regs.DE.b.l = i85stat.regs.BC.b.h;
                        break;
                case 0x59: i8085_ICount -= 5;   /* MOV  E,C */
                        i85stat.regs.DE.b.l = i85stat.regs.BC.b.l;
                        break;
                case 0x5a: i8085_ICount -= 5;   /* MOV  E,D */
                        i85stat.regs.DE.b.l = i85stat.regs.DE.b.h;
                        break;
                case 0x5b: i8085_ICount -= 5;   /* MOV  E,E */
                        /* no op */
                        break;
                case 0x5c: i8085_ICount -= 5;   /* MOV  E,H */
                        i85stat.regs.DE.b.l = i85stat.regs.HL.b.h;
                        break;
                case 0x5d: i8085_ICount -= 5;   /* MOV  E,L */
                        i85stat.regs.DE.b.l = i85stat.regs.HL.b.l;
                        break;
                case 0x5e: i8085_ICount -= 7;   /* MOV  E,M */
                        i85stat.regs.DE.b.l = RM(i85stat.regs.HL.d);
                        break;
                case 0x5f: i8085_ICount -= 5;   /* MOV  E,A */
                        i85stat.regs.DE.b.l = i85stat.regs.AF.b.h;
                        break;

                case 0x60: i8085_ICount -= 5;   /* MOV  H,B */
                        i85stat.regs.HL.b.h = i85stat.regs.BC.b.h;
                        break;
                case 0x61: i8085_ICount -= 5;   /* MOV  H,C */
                        i85stat.regs.HL.b.h = i85stat.regs.BC.b.l;
                        break;
                case 0x62: i8085_ICount -= 5;   /* MOV  H,D */
                        i85stat.regs.HL.b.h = i85stat.regs.DE.b.h;
                        break;
                case 0x63: i8085_ICount -= 5;   /* MOV  H,E */
                        i85stat.regs.HL.b.h = i85stat.regs.DE.b.l;
                        break;
                case 0x64: i8085_ICount -= 5;   /* MOV  H,H */
                        /* no op */
                        break;
                case 0x65: i8085_ICount -= 5;   /* MOV  H,L */
                        i85stat.regs.HL.b.h = i85stat.regs.HL.b.l;
                        break;
                case 0x66: i8085_ICount -= 7;   /* MOV  H,M */
                        i85stat.regs.HL.b.h = RM(i85stat.regs.HL.d);
                        break;
                case 0x67: i8085_ICount -= 5;   /* MOV  H,A */
                        i85stat.regs.HL.b.h = i85stat.regs.AF.b.h;
                        break;

                case 0x68: i8085_ICount -= 5;   /* MOV  L,B */
                        i85stat.regs.HL.b.l = i85stat.regs.BC.b.h;
                        break;
                case 0x69: i8085_ICount -= 5;   /* MOV  L,C */
                        i85stat.regs.HL.b.l = i85stat.regs.BC.b.l;
                        break;
                case 0x6a: i8085_ICount -= 5;   /* MOV  L,D */
                        i85stat.regs.HL.b.l = i85stat.regs.DE.b.h;
                        break;
                case 0x6b: i8085_ICount -= 5;   /* MOV  L,E */
                        i85stat.regs.HL.b.l = i85stat.regs.DE.b.l;
                        break;
                case 0x6c: i8085_ICount -= 5;   /* MOV  L,H */
                        i85stat.regs.HL.b.l = i85stat.regs.HL.b.h;
                        break;
                case 0x6d: i8085_ICount -= 5;   /* MOV  L,L */
                        /* no op */
                        break;
                case 0x6e: i8085_ICount -= 7;   /* MOV  L,M */
                        i85stat.regs.HL.b.l = RM(i85stat.regs.HL.d);
                        break;
                case 0x6f: i8085_ICount -= 5;   /* MOV  L,A */
                        i85stat.regs.HL.b.l = i85stat.regs.AF.b.h;
                        break;

                case 0x70: i8085_ICount -= 7;   /* MOV  M,B */
                        WM(i85stat.regs.HL.d, i85stat.regs.BC.b.h);
                        break;
                case 0x71: i8085_ICount -= 7;   /* MOV  M,C */
                        WM(i85stat.regs.HL.d, i85stat.regs.BC.b.l);
                        break;
                case 0x72: i8085_ICount -= 7;   /* MOV  M,D */
                        WM(i85stat.regs.HL.d, i85stat.regs.DE.b.h);
                        break;
                case 0x73: i8085_ICount -= 7;   /* MOV  M,E */
                        WM(i85stat.regs.HL.d, i85stat.regs.DE.b.l);
                        break;
                case 0x74: i8085_ICount -= 7;   /* MOV  M,H */
                        WM(i85stat.regs.HL.d, i85stat.regs.HL.b.h);
                        break;
                case 0x75: i8085_ICount -= 7;   /* MOV  M,L */
                        WM(i85stat.regs.HL.d, i85stat.regs.HL.b.l);
                        break;
                case 0x76: i8085_ICount -= 4;   /* HALT */
                        i85stat.regs.PC.w.l--;
                        i85stat.regs.HALT = 1;
                        if (i8085_ICount > 0) i8085_ICount = 0;
                        break;
                case 0x77: i8085_ICount -= 7;   /* MOV  M,A */
                        WM(i85stat.regs.HL.d, i85stat.regs.AF.b.h);
                        break;

                case 0x78: i8085_ICount -= 5;   /* MOV  A,B */
                        i85stat.regs.AF.b.h = i85stat.regs.BC.b.h;
                        break;
                case 0x79: i8085_ICount -= 5;   /* MOV  A,C */
                        i85stat.regs.AF.b.h = i85stat.regs.BC.b.l;
                        break;
                case 0x7a: i8085_ICount -= 5;   /* MOV  A,D */
                        i85stat.regs.AF.b.h = i85stat.regs.DE.b.h;
                        break;
                case 0x7b: i8085_ICount -= 5;   /* MOV  A,E */
                        i85stat.regs.AF.b.h = i85stat.regs.DE.b.l;
                        break;
                case 0x7c: i8085_ICount -= 5;   /* MOV  A,H */
                        i85stat.regs.AF.b.h = i85stat.regs.HL.b.h;
                        break;
                case 0x7d: i8085_ICount -= 5;   /* MOV  A,L */
                        i85stat.regs.AF.b.h = i85stat.regs.HL.b.l;
                        break;
                case 0x7e: i8085_ICount -= 7;   /* MOV  A,M */
                        i85stat.regs.AF.b.h = RM(i85stat.regs.HL.d);
                        break;
                case 0x7f: i8085_ICount -= 5;   /* MOV  A,A */
                        /* no op */
                        break;

                case 0x80: i8085_ICount -= 4;   /* ADD  B */
                        M_ADD(i85stat.regs.BC.b.h);
                        break;
                case 0x81: i8085_ICount -= 4;   /* ADD  C */
                        M_ADD(i85stat.regs.BC.b.l);
                        break;
                case 0x82: i8085_ICount -= 4;   /* ADD  D */
                        M_ADD(i85stat.regs.DE.b.h);
                        break;
                case 0x83: i8085_ICount -= 4;   /* ADD  E */
                        M_ADD(i85stat.regs.DE.b.l);
                        break;
                case 0x84: i8085_ICount -= 4;   /* ADD  H */
                        M_ADD(i85stat.regs.HL.b.h);
                        break;
                case 0x85: i8085_ICount -= 4;   /* ADD  L */
                        M_ADD(i85stat.regs.HL.b.l);
                        break;
                case 0x86: i8085_ICount -= 7;   /* ADD  M */
                        M_ADD(RM(i85stat.regs.HL.d));
                        break;
                case 0x87: i8085_ICount -= 4;   /* ADD  A */
                        M_ADD(i85stat.regs.AF.b.h);
                        break;

                case 0x88: i8085_ICount -= 4;   /* ADC  B */
                        M_ADC(i85stat.regs.BC.b.h);
                        break;
                case 0x89: i8085_ICount -= 4;   /* ADC  C */
                        M_ADC(i85stat.regs.BC.b.l);
                        break;
                case 0x8a: i8085_ICount -= 4;   /* ADC  D */
                        M_ADC(i85stat.regs.DE.b.h);
                        break;
                case 0x8b: i8085_ICount -= 4;   /* ADC  E */
                        M_ADC(i85stat.regs.DE.b.l);
                        break;
                case 0x8c: i8085_ICount -= 4;   /* ADC  H */
                        M_ADC(i85stat.regs.HL.b.h);
                        break;
                case 0x8d: i8085_ICount -= 4;   /* ADC  L */
                        M_ADC(i85stat.regs.HL.b.l);
                        break;
                case 0x8e: i8085_ICount -= 7;   /* ADC  M */
                        M_ADC(RM(i85stat.regs.HL.d));
                        break;
                case 0x8f: i8085_ICount -= 4;   /* ADC  A */
                        M_ADC(i85stat.regs.AF.b.h);
                        break;

                case 0x90: i8085_ICount -= 4;   /* SUB  B */
                        M_SUB(i85stat.regs.BC.b.h);
                        break;
                case 0x91: i8085_ICount -= 4;   /* SUB  C */
                        M_SUB(i85stat.regs.BC.b.l);
                        break;
                case 0x92: i8085_ICount -= 4;   /* SUB  D */
                        M_SUB(i85stat.regs.DE.b.h);
                        break;
                case 0x93: i8085_ICount -= 4;   /* SUB  E */
                        M_SUB(i85stat.regs.DE.b.l);
                        break;
                case 0x94: i8085_ICount -= 4;   /* SUB  H */
                        M_SUB(i85stat.regs.HL.b.h);
                        break;
                case 0x95: i8085_ICount -= 4;   /* SUB  L */
                        M_SUB(i85stat.regs.HL.b.l);
                        break;
                case 0x96: i8085_ICount -= 7;   /* SUB  M */
                        M_SUB(RM(i85stat.regs.HL.d));
                        break;
                case 0x97: i8085_ICount -= 4;   /* SUB  A */
                        M_SUB(i85stat.regs.AF.b.h);
                        break;

                case 0x98: i8085_ICount -= 4;   /* SBB  B */
                        M_SBB(i85stat.regs.BC.b.h);
                        break;
                case 0x99: i8085_ICount -= 4;   /* SBB  C */
                        M_SBB(i85stat.regs.BC.b.l);
                        break;
                case 0x9a: i8085_ICount -= 4;   /* SBB  D */
                        M_SBB(i85stat.regs.DE.b.h);
                        break;
                case 0x9b: i8085_ICount -= 4;   /* SBB  E */
                        M_SBB(i85stat.regs.DE.b.l);
                        break;
                case 0x9c: i8085_ICount -= 4;   /* SBB  H */
                        M_SBB(i85stat.regs.HL.b.h);
                        break;
                case 0x9d: i8085_ICount -= 4;   /* SBB  L */
                        M_SBB(i85stat.regs.HL.b.l);
                        break;
                case 0x9e: i8085_ICount -= 7;   /* SBB  M */
                        M_SBB(RM(i85stat.regs.HL.d));
                        break;
                case 0x9f: i8085_ICount -= 4;   /* SBB  A */
                        M_SBB(i85stat.regs.AF.b.h);
                        break;

                case 0xa0: i8085_ICount -= 4;   /* ANA  B */
                        M_ANA(i85stat.regs.BC.b.h);
                        break;
                case 0xa1: i8085_ICount -= 4;   /* ANA  C */
                        M_ANA(i85stat.regs.BC.b.l);
                        break;
                case 0xa2: i8085_ICount -= 4;   /* ANA  D */
                        M_ANA(i85stat.regs.DE.b.h);
                        break;
                case 0xa3: i8085_ICount -= 4;   /* ANA  E */
                        M_ANA(i85stat.regs.DE.b.l);
                        break;
                case 0xa4: i8085_ICount -= 4;   /* ANA  H */
                        M_ANA(i85stat.regs.HL.b.h);
                        break;
                case 0xa5: i8085_ICount -= 4;   /* ANA  L */
                        M_ANA(i85stat.regs.HL.b.l);
                        break;
                case 0xa6: i8085_ICount -= 7;   /* ANA  M */
                        M_ANA(RM(i85stat.regs.HL.d));
                        break;
                case 0xa7: i8085_ICount -= 4;   /* ANA  A */
                        M_ANA(i85stat.regs.AF.b.h);
                        break;

                case 0xa8: i8085_ICount -= 4;   /* XRA  B */
                        M_XRA(i85stat.regs.BC.b.h);
                        break;
                case 0xa9: i8085_ICount -= 4;   /* XRA  C */
                        M_XRA(i85stat.regs.BC.b.l);
                        break;
                case 0xaa: i8085_ICount -= 4;   /* XRA  D */
                        M_XRA(i85stat.regs.DE.b.h);
                        break;
                case 0xab: i8085_ICount -= 4;   /* XRA  E */
                        M_XRA(i85stat.regs.DE.b.l);
                        break;
                case 0xac: i8085_ICount -= 4;   /* XRA  H */
                        M_XRA(i85stat.regs.HL.b.h);
                        break;
                case 0xad: i8085_ICount -= 4;   /* XRA  L */
                        M_XRA(i85stat.regs.HL.b.l);
                        break;
                case 0xae: i8085_ICount -= 7;   /* XRA  M */
                        M_XRA(RM(i85stat.regs.HL.d));
                        break;
                case 0xaf: i8085_ICount -= 4;   /* XRA  A */
                        M_XRA(i85stat.regs.AF.b.h);
                        break;

                case 0xb0: i8085_ICount -= 4;   /* ORA  B */
                        M_ORA(i85stat.regs.BC.b.h);
                        break;
                case 0xb1: i8085_ICount -= 4;   /* ORA  C */
                        M_ORA(i85stat.regs.BC.b.l);
                        break;
                case 0xb2: i8085_ICount -= 4;   /* ORA  D */
                        M_ORA(i85stat.regs.DE.b.h);
                        break;
                case 0xb3: i8085_ICount -= 4;   /* ORA  E */
                        M_ORA(i85stat.regs.DE.b.l);
                        break;
                case 0xb4: i8085_ICount -= 4;   /* ORA  H */
                        M_ORA(i85stat.regs.HL.b.h);
                        break;
                case 0xb5: i8085_ICount -= 4;   /* ORA  L */
                        M_ORA(i85stat.regs.HL.b.l);
                        break;
                case 0xb6: i8085_ICount -= 7;   /* ORA  M */
                        M_ORA(RM(i85stat.regs.HL.d));
                        break;
                case 0xb7: i8085_ICount -= 4;   /* ORA  A */
                        M_ORA(i85stat.regs.AF.b.h);
                        break;

                case 0xb8: i8085_ICount -= 4;   /* CMP  B */
                        M_CMP(i85stat.regs.BC.b.h);
                        break;
                case 0xb9: i8085_ICount -= 4;   /* CMP  C */
                        M_CMP(i85stat.regs.BC.b.l);
                        break;
                case 0xba: i8085_ICount -= 4;   /* CMP  D */
                        M_CMP(i85stat.regs.DE.b.h);
                        break;
                case 0xbb: i8085_ICount -= 4;   /* CMP  E */
                        M_CMP(i85stat.regs.DE.b.l);
                        break;
                case 0xbc: i8085_ICount -= 4;   /* CMP  H */
                        M_CMP(i85stat.regs.HL.b.h);
                        break;
                case 0xbd: i8085_ICount -= 4;   /* CMP  L */
                        M_CMP(i85stat.regs.HL.b.l);
                        break;
                case 0xbe: i8085_ICount -= 7;   /* CMP  M */
                        M_CMP(RM(i85stat.regs.HL.d));
                        break;
                case 0xbf: i8085_ICount -= 4;   /* CMP  A */
                        M_CMP(i85stat.regs.AF.b.h);
                        break;

                case 0xc0: i8085_ICount -= 5;   /* RNZ  */
                        M_RET( !(i85stat.regs.AF.b.l & ZF) );
                        break;
                case 0xc1: i8085_ICount -= 10;  /* POP  B */
                        M_POP(BC);
                        break;
                case 0xc2: i8085_ICount -= 10;  /* JNZ  nnnn */
                        M_JMP( !(i85stat.regs.AF.b.l & ZF) );
                        break;
                case 0xc3: i8085_ICount -= 10;  /* JMP  nnnn */
                        M_JMP(1);
                        break;
                case 0xc4: i8085_ICount -= 11;  /* CNZ  nnnn */
                        M_CALL( !(i85stat.regs.AF.b.l & ZF) );
                        break;
                case 0xc5: i8085_ICount -= 11;  /* PUSH B */
                        M_PUSH(BC);
                        break;
                case 0xc6: i8085_ICount -= 7;   /* ADI  nn */
                        i85stat.regs.XX.b.l = ARG();
                        M_ADD(i85stat.regs.XX.b.l);
                                break;
                case 0xc7: i8085_ICount -= 11;  /* RST  0 */
                        M_RST(0);
                        break;

                case 0xc8: i8085_ICount -= 5;   /* RZ   */
                        M_RET( i85stat.regs.AF.b.l & ZF );
                        break;
                case 0xc9: i8085_ICount -= 4;   /* RET  */
                        M_RET(1);
                        break;
                case 0xca: i8085_ICount -= 10;  /* JZ   nnnn */
                        M_JMP( i85stat.regs.AF.b.l & ZF );
                        break;
                case 0xcb: i8085_ICount -= 4;   /* ???? */
                        illegal();
                        break;
                case 0xcc: i8085_ICount -= 11;  /* CZ   nnnn */
                        M_CALL( i85stat.regs.AF.b.l & ZF );
                        break;
                case 0xcd: i8085_ICount -= 11;  /* CALL nnnn */
                        M_CALL(1);
                        break;
                case 0xce: i8085_ICount -= 7;   /* ACI  nn */
                        i85stat.regs.XX.b.l = ARG();
                        M_ADC(i85stat.regs.XX.b.l);
                        break;
                case 0xcf: i8085_ICount -= 11;  /* RST  1 */
                        M_RST(1);
                        break;

                case 0xd0: i8085_ICount -= 5;   /* RNC  */
                        M_RET( !(i85stat.regs.AF.b.l & CF) );
                        break;
                case 0xd1: i8085_ICount -= 10;  /* POP  D */
                        M_POP(DE);
                        break;
                case 0xd2: i8085_ICount -= 10;  /* JNC  nnnn */
                        M_JMP( !(i85stat.regs.AF.b.l & CF) );
                        break;
                case 0xd3: i8085_ICount -= 10;  /* OUT  nn */
                        M_OUT;
                        break;
                case 0xd4: i8085_ICount -= 11;  /* CNC  nnnn */
                        M_CALL( !(i85stat.regs.AF.b.l & CF) );
                        break;
                case 0xd5: i8085_ICount -= 11;  /* PUSH D */
                        M_PUSH(DE);
                        break;
                case 0xd6: i8085_ICount -= 7;   /* SUI  nn */
                        i85stat.regs.XX.b.l = ARG();
                        M_SUB(i85stat.regs.XX.b.l);
                        break;
                case 0xd7: i8085_ICount -= 11;  /* RST  2 */
                        M_RST(2);
                        break;

                case 0xd8: i8085_ICount -= 5;   /* RC   */
                        M_RET( i85stat.regs.AF.b.l & CF );
                        break;
                case 0xd9: i8085_ICount -= 4;   /* ???? */
                        illegal();
                        break;
                case 0xda: i8085_ICount -= 10;  /* JC   nnnn */
                        M_JMP( i85stat.regs.AF.b.l & CF );
                        break;
                case 0xdb: i8085_ICount -= 10;  /* IN   nn */
                        M_IN;
                        break;
                case 0xdc: i8085_ICount -= 11;  /* CC   nnnn */
                        M_CALL( i85stat.regs.AF.b.l & CF );
                        break;
                case 0xdd: i8085_ICount -= 4;   /* ???? */
                        illegal();
                        break;
                case 0xde: i8085_ICount -= 7;   /* SBI  nn */
                        i85stat.regs.XX.b.l = ARG();
                        M_SBB(i85stat.regs.XX.b.l);
                        break;
                case 0xdf: i8085_ICount -= 11;  /* RST  3 */
                        M_RST(3);
                        break;

                case 0xe0: i8085_ICount -= 5;   /* RPE    */
                        M_RET( !(i85stat.regs.AF.b.l & VF) );
                        break;
                case 0xe1: i8085_ICount -= 10;  /* POP  H */
                        M_POP(HL);
                        break;
                case 0xe2: i8085_ICount -= 10;  /* JPE  nnnn */
                        M_JMP( !(i85stat.regs.AF.b.l & VF) );
                        break;
                case 0xe3: i8085_ICount -= 18;  /* XTHL */
                        M_POP(XX);
                        M_PUSH(HL);
                        i85stat.regs.HL.d = i85stat.regs.XX.d;
                        break;
                case 0xe4: i8085_ICount -= 11;  /* CPE  nnnn */
                        M_CALL( !(i85stat.regs.AF.b.l & VF) );
                        break;
                case 0xe5: i8085_ICount -= 11;  /* PUSH H */
                        M_PUSH(HL);
                        break;
                case 0xe6: i8085_ICount -= 7;   /* ANI  nn */
                        i85stat.regs.XX.b.l = ARG();
                        M_ANA(i85stat.regs.XX.b.l);
                        break;
                case 0xe7: i8085_ICount -= 11;  /* RST  4 */
                        M_RST(4);
                        break;

                case 0xe8: i8085_ICount -= 5;   /* RPO  */
                        M_RET( i85stat.regs.AF.b.l & VF );
                        break;
                case 0xe9: i8085_ICount -= 5;   /* PCHL */
                        i85stat.regs.PC.d = i85stat.regs.HL.w.l;
                        change_pc16(i85stat.regs.PC.d);
                        break;
                case 0xea: i8085_ICount -= 10;  /* JPO  nnnn */
                        M_JMP( i85stat.regs.AF.b.l & VF );
                        break;
                case 0xeb: i8085_ICount -= 4;   /* XCHG */
                        i85stat.regs.XX.d = i85stat.regs.DE.d;
                        i85stat.regs.DE.d = i85stat.regs.HL.d;
                        i85stat.regs.HL.d = i85stat.regs.XX.d;
                        break;
                case 0xec: i8085_ICount -= 11;  /* CPO  nnnn */
                        M_CALL( i85stat.regs.AF.b.l & VF );
                        break;
                case 0xed: i8085_ICount -= 4;   /* ???? */
                        illegal();
                        break;
                case 0xee: i8085_ICount -= 7;   /* XRI  nn */
                        i85stat.regs.XX.b.l = ARG();
                        M_XRA(i85stat.regs.XX.b.l);
                        break;
                case 0xef: i8085_ICount -= 11;  /* RST  5 */
                        M_RST(5);
                        break;

                case 0xf0: i8085_ICount -= 5;   /* RP   */
                        M_RET( !(i85stat.regs.AF.b.l&SF) );
                        break;
                case 0xf1: i8085_ICount -= 10;  /* POP  A */
                        M_POP(AF);
                        break;
                case 0xf2: i8085_ICount -= 10;  /* JP   nnnn */
                        M_JMP( !(i85stat.regs.AF.b.l & SF) );
                        break;
                case 0xf3: i8085_ICount -= 4;   /* DI   */
                        /* remove interrupt enable */
                        i85stat.regs.IM &= ~IM_IEN;
                        break;
                case 0xf4: i8085_ICount -= 11;  /* CP   nnnn */
                        M_CALL( !(i85stat.regs.AF.b.l & SF) );
                        break;
                case 0xf5: i8085_ICount -= 11;  /* PUSH A */
                        M_PUSH(AF);
                        break;
                case 0xf6: i8085_ICount -= 7;   /* ORI  nn */
                        i85stat.regs.XX.b.l = ARG();
                        M_ORA(i85stat.regs.XX.b.l);
                        break;
                case 0xf7: i8085_ICount -= 11;  /* RST  6 */
                        M_RST(6);
                        break;

                case 0xf8: i8085_ICount -= 5;   /* RM   */
                        M_RET( i85stat.regs.AF.b.l & SF );
                        break;
                case 0xf9: i8085_ICount -= 5;   /* SPHL */
                        i85stat.regs.SP.d = i85stat.regs.HL.d;
                        break;
                case 0xfa: i8085_ICount -= 10;  /* JM   nnnn */
                        M_JMP( i85stat.regs.AF.b.l & SF );
                        break;
                case 0xfb: i8085_ICount -= 4;   /* EI */
                        /* set interrupt enable */
                        i85stat.regs.IM |= IM_IEN;
                        /* remove serviced IRQ flag */
                        i85stat.regs.IREQ &= ~i85stat.regs.ISRV;
                        /* reset serviced IRQ */
                        i85stat.regs.ISRV = 0;
                        if( i85stat.regs.irq_state[0] != CLEAR_LINE )
                        {
                                LOG(("i8085 EI sets INTR\n"));
                                i85stat.regs.IREQ |= IM_INTR;
                                i85stat.regs.INTR = I8085_INTR;
                        }
                        if( i85stat.regs.cputype )
                        {
                                if( i85stat.regs.irq_state[1] != CLEAR_LINE )
                                {
                                        LOG(("i8085 EI sets RST5.5\n"));
                                        i85stat.regs.IREQ |= IM_RST55;
                                }
                                if( i85stat.regs.irq_state[2] != CLEAR_LINE )
                                {
                                        LOG(("i8085 EI sets RST6.5\n"));
                                        i85stat.regs.IREQ |= IM_RST65;
                                }
                                if( i85stat.regs.irq_state[3] != CLEAR_LINE )
                                {
                                        LOG(("i8085 EI sets RST7.5\n"));
                                        i85stat.regs.IREQ |= IM_RST75;
                                }
                                /* find highest priority IREQ flag with
                                   IM enabled and schedule for execution */
                                if( !(i85stat.regs.IM & IM_RST75) && (i85stat.regs.IREQ & IM_RST75) )
                                {
                                        i85stat.regs.ISRV = IM_RST75;
                                        i85stat.regs.IRQ2 = ADDR_RST75;
                                }
                                else
                                if( !(i85stat.regs.IM & IM_RST65) && (i85stat.regs.IREQ & IM_RST65) )
                                {
                                        i85stat.regs.ISRV = IM_RST65;
                                        i85stat.regs.IRQ2 = ADDR_RST65;
                                }
                                else
                                if( !(i85stat.regs.IM & IM_RST55) && (i85stat.regs.IREQ & IM_RST55) )
                                {
                                        i85stat.regs.ISRV = IM_RST55;
                                        i85stat.regs.IRQ2 = ADDR_RST55;
                                }
                                else
                                if( !(i85stat.regs.IM & IM_INTR) && (i85stat.regs.IREQ & IM_INTR) )
                                {
                                        i85stat.regs.ISRV = IM_INTR;
                                        i85stat.regs.IRQ2 = i85stat.regs.INTR;
                                }
                        }
                        else
                        {
                                if( !(i85stat.regs.IM & IM_INTR) && (i85stat.regs.IREQ & IM_INTR) )
                                {
                                        i85stat.regs.ISRV = IM_INTR;
                                        i85stat.regs.IRQ2 = i85stat.regs.INTR;
                                }
                        }
                        break;
                case 0xfc: i8085_ICount -= 11;  /* CM   nnnn */
                        M_CALL( i85stat.regs.AF.b.l & SF );
                        break;
                case 0xfd: i8085_ICount -= 4;   /* ???? */
                        illegal();
                        break;
                case 0xfe: i8085_ICount -= 7;   /* CPI  nn */
                        i85stat.regs.XX.b.l = ARG();
                        M_CMP(i85stat.regs.XX.b.l);
                        break;
                case 0xff: i8085_ICount -= 11;  /* RST  7 */
                        M_RST(7);
                        break;
        }
}

void Ci8085::Interrupt(void)
{

        if( i85stat.regs.HALT )            /* if the CPU was halted */
        {
                i85stat.regs.PC.w.l++;     /* skip HALT instr */
                i85stat.regs.HALT = 0;
        }
        i85stat.regs.IM &= ~IM_IEN;                /* remove general interrupt enable bit */

        if( i85stat.regs.ISRV == IM_INTR )
        {
                LOG(("Interrupt get INTR vector\n"));
                i85stat.regs.IRQ1 = (i85stat.regs.irq_callback)(0);
        }

        if( i85stat.regs.cputype )
        {
                if( i85stat.regs.ISRV == IM_RST55 )
                {
                        LOG(("Interrupt get RST5.5 vector\n"));
                        i85stat.regs.IRQ1 = I8085_RST55;//(i85stat.regs.irq_callback)(1);
                }

                if( i85stat.regs.ISRV == IM_RST65  )
                {
                        LOG(("Interrupt get RST6.5 vector\n"));
                        i85stat.regs.IRQ1 = I8085_RST65;//(i85stat.regs.irq_callback)(2);
                }

                if( i85stat.regs.ISRV == IM_RST75 )
                {
                        LOG(("Interrupt get RST7.5 vector\n"));
                        i85stat.regs.IRQ1 = I8085_RST75;//(i85stat.regs.irq_callback)(3);
                }
        }

        switch( i85stat.regs.IRQ1 & 0xff0000 )
        {
                case 0xcd0000:  /* CALL nnnn */
                        i8085_ICount -= 7;
                        M_PUSH(PC);
                case 0xc30000:  /* JMP  nnnn */
                        i8085_ICount -= 10;
                        i85stat.regs.PC.d = i85stat.regs.IRQ1 & 0xffff;
                        change_pc16(i85stat.regs.PC.d);
                        break;
                default:
                        switch( i85stat.regs.IRQ1 )
                        {
                                case I8085_TRAP:
                                case I8085_RST75:
                                case I8085_RST65:
                                case I8085_RST55:
                                        M_PUSH(PC);
                                        if (i85stat.regs.IRQ1 != I8085_RST75)
                                                i85stat.regs.PC.d = i85stat.regs.IRQ1;
                                        else
                                                i85stat.regs.PC.d = 0x3c;
                                        change_pc16(i85stat.regs.PC.d);
                                        if (fp_log) fprintf(fp_log,"INTERRUPT\n");
                                        break;
                                default:
                                        LOG(("i8085 take int $%02x\n", i85stat.regs.IRQ1));
                                        execute_one(i85stat.regs.IRQ1 & 0xff);
                        }
        }
}

int Ci8085::i8085_execute(int cycles)
{

        i8085_ICount = cycles;
        do
        {


                /* interrupts enabled or TRAP pending ? */
                if ( (i85stat.regs.IM & IM_IEN) || (i85stat.regs.IREQ & IM_TRAP) )
                {
                        /* copy scheduled to executed interrupt request */
                        i85stat.regs.IRQ1 = i85stat.regs.IRQ2;
                        /* reset scheduled interrupt request */
                        i85stat.regs.IRQ2 = 0;
                        /* interrupt now ? */
                        if (i85stat.regs.IRQ1) Interrupt();
                }

                /* here we go... */
                execute_one(ROP());

        } while (i8085_ICount > 0);

        return cycles - i8085_ICount;
}

/****************************************************************************
 * Initialise the various lookup tables used by the emulation code
 ****************************************************************************/
void Ci8085::init_tables (void)
{
        quint8 zs;
        int i, p;
        for (i = 0; i < 256; i++)
        {
                zs = 0;
                if (i==0) zs |= ZF;
                if (i&128) zs |= SF;
                p = 0;
                if (i&1) ++p;
                if (i&2) ++p;
                if (i&4) ++p;
                if (i&8) ++p;
                if (i&16) ++p;
                if (i&32) ++p;
                if (i&64) ++p;
                if (i&128) ++p;
                ZS[i] = zs;
                ZSP[i] = zs | ((p&1) ? 0 : VF);
        }
}



/****************************************************************************
 * Set the current 8085 PC
 ****************************************************************************/
void Ci8085::i8085_set_pc(unsigned val)
{
        i85stat.regs.PC.w.l = val;
        //change_pc(i85stat.regs.PC.d);
}

/****************************************************************************
 * Get the current 8085 SP
 ****************************************************************************/
unsigned Ci8085::i8085_get_sp(void)
{
        return i85stat.regs.SP.d;
}

/****************************************************************************
 * Set the current 8085 SP
 ****************************************************************************/
void Ci8085::i8085_set_sp(unsigned val)
{
        i85stat.regs.SP.w.l = val;
}

/****************************************************************************
 * Get a specific register
 ****************************************************************************/
#define REG_SP_CONTENTS -2
unsigned Ci8085::i8085_get_reg(int regnum)
{
        switch( regnum )
        {
                case I8085_PC: return i85stat.regs.PC.w.l;
                case I8085_SP: return i85stat.regs.SP.w.l;
                case I8085_AF: return i85stat.regs.AF.w.l;
                case I8085_BC: return i85stat.regs.BC.w.l;
                case I8085_DE: return i85stat.regs.DE.w.l;
                case I8085_HL: return i85stat.regs.HL.w.l;
                case I8085_IM: return i85stat.regs.IM;
                case I8085_HALT: return i85stat.regs.HALT;
                case I8085_IREQ: return i85stat.regs.IREQ;
                case I8085_ISRV: return i85stat.regs.ISRV;
                case I8085_VECTOR: return i85stat.regs.INTR;
                case I8085_TRAP_STATE: return i85stat.regs.nmi_state;
                case I8085_INTR_STATE: return i85stat.regs.irq_state[I8085_INTR_LINE];
                case I8085_RST55_STATE: return i85stat.regs.irq_state[I8085_RST55_LINE];
                case I8085_RST65_STATE: return i85stat.regs.irq_state[I8085_RST65_LINE];
                case I8085_RST75_STATE: return i85stat.regs.irq_state[I8085_RST75_LINE];
                case -1: return 0; /* previous pc not supported */
                default:
                        if( regnum <= REG_SP_CONTENTS )
                        {
                                unsigned offset = i85stat.regs.SP.w.l + 2 * (REG_SP_CONTENTS - regnum);
                                if( offset < 0xffff )
                                        return RM( offset ) + ( RM( offset+1 ) << 8 );
                        }
        }
        return 0;
}

/****************************************************************************
 * Set a specific register
 ****************************************************************************/
void Ci8085::i8085_set_reg(int regnum, unsigned val)
{
        switch( regnum )
        {
                case I8085_PC: i85stat.regs.PC.w.l = val; break;
                case I8085_SP: i85stat.regs.SP.w.l = val; break;
                case I8085_AF: i85stat.regs.AF.w.l = val; break;
                case I8085_BC: i85stat.regs.BC.w.l = val; break;
                case I8085_DE: i85stat.regs.DE.w.l = val; break;
                case I8085_HL: i85stat.regs.HL.w.l = val; break;
                case I8085_IM: i85stat.regs.IM = val; break;
                case I8085_HALT: i85stat.regs.HALT = val; break;
                case I8085_IREQ: i85stat.regs.IREQ = val; break;
                case I8085_ISRV: i85stat.regs.ISRV = val; break;
                case I8085_VECTOR: i85stat.regs.INTR = val; break;
                case I8085_TRAP_STATE: i85stat.regs.nmi_state = val; break;
                case I8085_INTR_STATE: i85stat.regs.irq_state[I8085_INTR_LINE] = val; break;
                case I8085_RST55_STATE: i85stat.regs.irq_state[I8085_RST55_LINE] = val; break;
                case I8085_RST65_STATE: i85stat.regs.irq_state[I8085_RST65_LINE] = val; break;
                case I8085_RST75_STATE: i85stat.regs.irq_state[I8085_RST75_LINE] = val; break;
                default:
                        if( regnum <= REG_SP_CONTENTS )
                        {
                                unsigned offset = i85stat.regs.SP.w.l + 2 * (REG_SP_CONTENTS - regnum);
                                if( offset < 0xffff )
                                {
                                        WM( offset, val&0xff );
                                        WM( offset+1, (val>>8)&0xff );
                                }
                        }
        }
}

/****************************************************************************/
/* Set the 8085 SID input signal state                                                                          */
/****************************************************************************/
void Ci8085::i8085_set_SID(int state)
{
        LOG(("i8085: SID %d\n", state));
        if (state)
                i85stat.regs.IM |= IM_SID;
        else
                i85stat.regs.IM &= ~IM_SID;
}

/****************************************************************************/
/* Set a callback to be called at SOD output change                                             */
/****************************************************************************/
//void i8085_set_sod_callback(void (*callback)(int state))
//{
//        i85stat.regs.sod_callback = callback;
//}

/****************************************************************************/
/* Set TRAP signal state                                                                                                        */
/****************************************************************************/
void Ci8085::i8085_set_TRAP(int state)
{
        LOG(("i8085: TRAP %d\n", state));
        if (state)
        {
                i85stat.regs.IREQ |= IM_TRAP;
                if( i85stat.regs.ISRV & IM_TRAP ) return;  /* already servicing TRAP ? */
                i85stat.regs.ISRV = IM_TRAP;                               /* service TRAP */
                i85stat.regs.IRQ2 = ADDR_TRAP;
        }
        else
        {
                i85stat.regs.IREQ &= ~IM_TRAP;                     /* remove request for TRAP */
        }
}

/****************************************************************************/
/* Set RST7.5 signal state                                                                                                      */
/****************************************************************************/
void Ci8085::i8085_set_RST75(int state)
{
//        LOG(("i8085: RST7.5 %d\n", state));
        if( state )
        {

                i85stat.regs.IREQ |= IM_RST75;                     /* request RST7.5 */
                if( i85stat.regs.IM & IM_RST75 ) return;   /* if masked, ignore it for now */
                if( !i85stat.regs.ISRV )                                   /* if no higher priority IREQ is serviced */
                {
                        i85stat.regs.ISRV = IM_RST75;                      /* service RST7.5 */
                        i85stat.regs.IRQ2 = ADDR_RST75;
                }
        }
        /* RST7.5 is reset only by SIM or end of service routine ! */
}

/****************************************************************************/
/* Set RST6.5 signal state                                                                                                      */
/****************************************************************************/
void Ci8085::i8085_set_RST65(int state)
{
        LOG(("i8085: RST6.5 %d\n", state));
        if( state )
        {
                i85stat.regs.IREQ |= IM_RST65;                     /* request RST6.5 */
                if( i85stat.regs.IM & IM_RST65 ) return;   /* if masked, ignore it for now */
                if( !i85stat.regs.ISRV )                                   /* if no higher priority IREQ is serviced */
                {
                        i85stat.regs.ISRV = IM_RST65;                      /* service RST6.5 */
                        i85stat.regs.IRQ2 = ADDR_RST65;
                }
        }
        else
        {
                i85stat.regs.IREQ &= ~IM_RST65;                    /* remove request for RST6.5 */
        }
}

/****************************************************************************/
/* Set RST5.5 signal state                                                                                                      */
/****************************************************************************/
void Ci8085::i8085_set_RST55(int state)
{
        LOG(("i8085: RST5.5 %d\n", state));
        if( state )
        {
                i85stat.regs.IREQ |= IM_RST55;                     /* request RST5.5 */
                if( i85stat.regs.IM & IM_RST55 ) return;   /* if masked, ignore it for now */
                if( !i85stat.regs.ISRV )                                   /* if no higher priority IREQ is serviced */
                {
                        i85stat.regs.ISRV = IM_RST55;                      /* service RST5.5 */
                        i85stat.regs.IRQ2 = ADDR_RST55;
                }
        }
        else
        {
                i85stat.regs.IREQ &= ~IM_RST55;                    /* remove request for RST5.5 */
        }
}

/****************************************************************************/
/* Set INTR signal                                                                                                                      */
/****************************************************************************/
void Ci8085::i8085_set_INTR(int state)
{
        LOG(("i8085: INTR %d\n", state));
        if( state )
        {
                i85stat.regs.IREQ |= IM_INTR;                              /* request INTR */
                i85stat.regs.INTR = state;
                if( i85stat.regs.IM & IM_INTR ) return;    /* if masked, ignore it for now */
                if( !i85stat.regs.ISRV )                                   /* if no higher priority IREQ is serviced */
                {
                        i85stat.regs.ISRV = IM_INTR;                       /* service INTR */
                        i85stat.regs.IRQ2 = i85stat.regs.INTR;
                }
        }
        else
        {
                i85stat.regs.IREQ &= ~IM_INTR;                     /* remove request for INTR */
        }
}

void Ci8085::i8085_set_nmi_line(int state)
{
        i85stat.regs.nmi_state = state;
        if( state != CLEAR_LINE )
                i8085_set_TRAP(1);
}

void Ci8085::i8085_set_irq_line(int irqline, int state)
{
        i85stat.regs.irq_state[irqline] = state;
        if (state == CLEAR_LINE)
        {
                if( !(i85stat.regs.IM & IM_IEN) )
                {
                        switch (irqline)
                        {
                                case I8085_INTR_LINE: i8085_set_INTR(0); break;
                                case I8085_RST55_LINE: i8085_set_RST55(0); break;
                                case I8085_RST65_LINE: i8085_set_RST65(0); break;
                                case I8085_RST75_LINE: i8085_set_RST75(0); break;
                        }
                }
        }
        else
        {
                if( i85stat.regs.IM & IM_IEN )
                {
                        switch( irqline )
                        {
                                case I8085_INTR_LINE: i8085_set_INTR(1); break;
                                case I8085_RST55_LINE: i8085_set_RST55(1); break;
                                case I8085_RST65_LINE: i8085_set_RST65(1); break;
                                case I8085_RST75_LINE: i8085_set_RST75(1); break;
                        }
                }
        }
}

//void Ci8085::i8085_set_irq_callback(int (*callback)(int))
//{
//        i85stat.regs.irq_callback = callback;
//}




void Ci8085::step()
{

    pPC->pTIMER->state += i8085_execute(0);

}

void Ci8085::Reset()
{
    init_tables();
    memset(&I, 0, sizeof(I));
    i85stat.regs.cputype = 1;
    change_pc16(i85stat.regs.PC.d);
}



void Ci8085::Load_Internal(QXmlStreamReader *)
{
}

void Ci8085::save_internal(QXmlStreamWriter *)
{
}

DWORD Ci8085::get_PC()
{
    return i85stat.regs.PC.d;
}

void Ci8085::Regs_Info(quint8)
{
    sprintf(Regs_String,"EMPTY");
    char buf[32];

    sprintf(
    Regs_String,
    "PC=%04X SP=%04X AF=%04X BC=%04X DE=%04X HL=%04X XX=%04X ",
     i85stat.regs.PC.d,
                i85stat.regs.SP.d,
                i85stat.regs.AF.d,
                i85stat.regs.BC.d,
                i85stat.regs.DE.d,
                i85stat.regs.HL.d,
                i85stat.regs.XX.d
    );
}
