#include "cf79107pj.h"
#include "pcxxxx.h"
#include "cpu.h"

CCF79107PJ::CCF79107PJ(CpcXXXX *parent)
{
    pPC = parent;

}

CCF79107PJ::~CCF79107PJ()
{
}


bool CCF79107PJ::init()
{
}

bool CCF79107PJ::exit()
{
}

void CCF79107PJ::Reset()
{
}

bool CCF79107PJ::step()
{
}
UINT8 CCF79107PJ::get_status()
{
    return BCDret;
}
bool CCF79107PJ::instruction1(UINT8 cmd)
{
    switch(cmd) {
    case 0x10:  // X <- 0
        if (pPC->fp_log) fprintf(pPC->fp_log,"\nX<-0\nbeforeCCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        for(int i = 0x400; i <= 0x408; i++)
            pPC->mem[i]= 0;
        if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        break;
    case 0x11: // Y <- 0
        if (pPC->fp_log) fprintf(pPC->fp_log,"\nY<-0\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        for(int i = 0x410; i <= 0x418; i++)
            pPC->mem[i]= 0;
        if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        break;
    case 0x80: //  X <-> Y
        if (pPC->fp_log) fprintf(pPC->fp_log,"\nX<->Y\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        for(int i = 0x410; i <= 0x418; i++) {
            quint8 _t = pPC->mem[i-0x10];
            pPC->mem[i-0x10] = pPC->mem[i];
            pPC->mem[i] = _t;
        }
        if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        break;
    default:
        masterCMD = cmd;
    }

    return true;
}

bool CCF79107PJ::instruction2(UINT8 cmd)
{
    switch (cmd) {

    case 0x41:
        if (pPC->fp_log) fprintf(pPC->fp_log,"\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        cmd_41();
        if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        break;

    default:
        switch (masterCMD) {
        case 0x04: {
            switch (cmd) {
            case 0x48: // BCD Shift(4bits shift)
                if (pPC->fp_log) fprintf(pPC->fp_log,"before CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                for(int i = 0x406; i >= 0x401; i--) {
                    quint8 _tmp = pPC->mem[i]&0x0f;
                    pPC->mem[i] = (pPC->mem[i-1]&0xf0)>>4;
                    pPC->mem[i] |= _tmp<<4;
                    //pPC->mem[i] = ((pPC->mem[i-1]&0x0f)<<4) | ((pPC->mem[i] & 0xf0 ) >> 4);
                }
                pPC->mem[0x400]=(pPC->mem[0x400]&0x0f)<<4;
                if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                break;
            case 0x99:
                if (pPC->fp_log) fprintf(pPC->fp_log,"before CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                for(int i = 0x406; i >= 0x401; i--){
                    quint8 _tmp = pPC->mem[i]&0x0f;
                    pPC->mem[i] = (pPC->mem[i-1]&0xf0)>>4;
                    pPC->mem[i] |= _tmp<<4;
                    //pPC->mem[i] = ((pPC->mem[i-1]&0x0f)<<4) | ((pPC->mem[i] & 0xf0 ) >> 4);
                }
                pPC->mem[0x400]=(pPC->mem[0x400]&0x0f)<<4;
                pPC->mem[0x407]=make_bcd_sub(pPC->mem[0x407],1);
                    //pPC->mem[i]=pPC->mem[i-1];
                if (pPC->fp_log) fprintf(pPC->fp_log,"before CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                break;
            case 0x4c:    // divide the mantissa by 10
            {
                // shift left 400h-406h (4bits)
                if (pPC->fp_log) fprintf(pPC->fp_log,"\ndivide the mantissa by 10, shift left 400h-406h (4bits)\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                for(int i = 0x400; i < 0x406; i++) {
                    quint8 _tmp = pPC->mem[i]&0xf0;
                    pPC->mem[i] = (pPC->mem[i+1]&0x0f)<<4;
                    pPC->mem[i] |= _tmp>>4;
                    //pPC->mem[i] = ((pPC->mem[i]&0x0f)<<4) | ((pPC->mem[i+1] & 0xf0 ) >> 4);
                }
                pPC->mem[0x406] = (pPC->mem[0x406]&0xf0)>>4;
                if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
             }   break;
            case 0xc0:
                if (pPC->fp_log) fprintf(pPC->fp_log,"\nX <- X + Y\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                cmd_c0();
                if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                break;
            case 0x91:
                if (pPC->fp_log) fprintf(pPC->fp_log,"\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                cmd_0491();
//                pPC->mem[0x407]=make_bcd_sub(pPC->mem[0x407],1);
                if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                break;
            case 0x9c: // exchange H06 with L07
            {
                // shift left 400h-406h (4bits)
                if (pPC->fp_log) fprintf(pPC->fp_log,"\ndivide the mantissa by 10, shift left 400h-406h (4bits)\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                for(int i = 0x400; i < 0x406; i++) {
                    quint8 _tmp = pPC->mem[i]&0xf0;
                    pPC->mem[i] = (pPC->mem[i+1]&0x0f)<<4;
                    pPC->mem[i] |= _tmp>>4;
                    //pPC->mem[i] = ((pPC->mem[i]&0x0f)<<4) | ((pPC->mem[i+1] & 0xf0 ) >> 4);
                }
                pPC->mem[0x406] = (pPC->mem[0x406]&0xf0)>>4;
                cmd_inc_exp();
                if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
            }
                break;
            }
        } break;
        case 0x05: {
            switch(cmd) {
            case 0xd0: // X -> Y
            {
                if (pPC->fp_log) fprintf(pPC->fp_log,"\nX -> Y\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                for(int i = 0x400; i <= 0x408; i++)
                    pPC->mem[i+0x10] = pPC->mem[i];
                if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
            }
                break;
            }
        }
        break;
        case 0x08: {
            switch(cmd) {
            case 0x00:
                if (pPC->fp_log) fprintf(pPC->fp_log,"\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                cmd_0800();
                if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
                dumpXYW();
                break;
            }
        }
            break;
        }
    }

    return true;
}

void CCF79107PJ::cmd_0491(void) //sbbw
{
    UINT16 arg = 0x407;
    UINT16 src = 0x417;
    UINT16 res0, res1;

    res0 = make_bcd_sub(pPC->mem[arg], 1);
    pPC->mem[arg] = res0 & 0xff;
    res1 = (res0>0xff) ? 1 : 0 ;
    res1 = make_bcd_sub(pPC->mem[arg+1],  res1);
    pPC->mem[arg+1] = res1 & 0xff;

    BCDret = ((res0 || res1)==0 ? 0x40 : 0x00) | (res1 > 0xff ? 0x01 : 0x00);
}

void CCF79107PJ::cmd_inc_exp(void) //sbbw
{
    UINT16 arg = 0x407;
    UINT16 res0, res1;

    res0 = make_bcd_add(pPC->mem[arg], 1);
    pPC->mem[arg] = res0 & 0xff;
    res1 = (res0>0xff) ? 1 : 0 ;
    res1 = make_bcd_add(pPC->mem[arg+1],  res1);
    pPC->mem[arg+1] = res1 & 0xff;

    BCDret = ((res0 || res1)==0 ? 0x40 : 0x00) | (res1 > 0xff ? 0x01 : 0x00);
}

void CCF79107PJ::cmd_0800(void) //adbw
{
    UINT16 arg = 0x407;
    UINT16 src = 0x417;
    UINT16 res0, res1;

    res0 = make_bcd_add(pPC->mem[arg], pPC->mem[src]);
    pPC->mem[arg] = res0 & 0xff;
    res1 = (res0>0xff) ? 1 : 0 ;
    res1 = make_bcd_add(pPC->mem[arg+1], pPC->mem[src+1] + res1);
    pPC->mem[arg+1] = res1 & 0xff;

    BCDret = ((res0 || res1)==0 ? 0x40 : 0x00) | (res1 > 0xff ? 0x01 : 0x00);
}

// adbm	$10,$sz,7
// X = X + Y    return 40h if ok
void CCF79107PJ::cmd_c0(void) {
    UINT16 dst = 0x400;
    UINT16 src = 0x410;
    UINT8 c, f;
    UINT16 res = 0;

    c = f = 0;
    for (int n=7; n>0; n--)
    {
        res = make_bcd_add(pPC->mem[dst], pPC->mem[src] + c);
        c = (res > 0xff) ? 1 : 0;
        pPC->mem[dst] = res&0xff;
        fprintf(pPC->fp_log,"mem[%04x]=%02X\n",dst,res&0xff);
        f |= (res&0xff);
        dst++; src++;
    }
    BCDret = (f==0 ? 0x40 : 0x00) | (res > 0xff ? 0x01 : 0x00);

}

void CCF79107PJ::cmd_41(void) {
    // X - Y
    UINT8 c, f;
    UINT16 res = 0;

    c = f = 0;
    for (int n=7; n>0; n--)
    {
        res = make_bcd_sub(pPC->mem[0x400+n], pPC->mem[0x410+n] + c);
        c = (res > 0xff) ? 1 : 0;
        f |= (res&0xff);
    }
    BCDret = (f==0 ? 0x40 : 0x00) | (res > 0xff ? 0x01 : 0x00);

}


void CCF79107PJ::Load_Internal(QXmlStreamReader *)
{
}

void CCF79107PJ::save_internal(QXmlStreamWriter *)
{
}

inline UINT16 CCF79107PJ::make_bcd_add(UINT8 arg1, UINT8 arg2)
{
    quint32 ret = (arg1&0x0f) + (arg2&0x0f);
    UINT8 carry;

    if (ret > 0x09)
    {
        ret = (ret + 0x06) & 0x0f;
        carry = 1;
    }
    else
        carry = 0;

    ret += ((arg1&0xf0) + (arg2&0xf0) + (carry<<4));

    if (ret > 0x9f)
    {
        ret = (ret + 0x60) & 0x0ff;
        carry = 1;
    }
    else
        carry = 0;

    ret += (carry<<8);

    return ret;
}

inline UINT16 CCF79107PJ::make_bcd_sub(UINT8 arg1, UINT8 arg2)
{

    quint32 ret = (arg1&0x0f) - (arg2&0x0f);
    UINT8 carry;

    if (ret > 0x09)
    {
        ret = (ret - 0x06) & 0x0f;
        carry = 1;
    }
    else
        carry = 0;

    ret += ((arg1&0xf0) - (arg2&0xf0) - (carry<<4));

    if (ret > 0x9f)
    {
        ret = (ret - 0x60) & 0x0ff;
        carry = 1;
    }
    else
        carry = 0;

    ret -= (carry<<8);

    return ret;
}

void CCF79107PJ::dumpXYW(void) {
#if 0
    QString _tmp="";
    for(int i = 0x400; i <= 0x40f; i++) {
        _tmp = _tmp+QString("%1 ").arg(mem[i],2,16,QChar('0'));
    AddLog(LOG_TEMP,"X="+_tmp);
    _tmp="";
    for(int i = 0x410; i <= 0x41f; i++)
        _tmp = _tmp+QString("%1 ").arg(mem[i],2,16,QChar('0'));
    AddLog(LOG_TEMP,"Y="+_tmp);
    _tmp="";
    for(int i = 0x420; i < 0x42f; i++)
        _tmp = _tmp+QString("%1 ").arg(mem[i],2,16,QChar('0'));
    AddLog(LOG_TEMP,"W="+_tmp);
#else
    fprintf(pPC->fp_log,"X=");
    for(int i = 0x400; i <= 0x40f; i++)
        fprintf(pPC->fp_log,"%02x ", pPC->mem[i]);
    fprintf(pPC->fp_log,"\n");
    fprintf(pPC->fp_log,"Y=");
    for(int i = 0x410; i <= 0x41f; i++)
        fprintf(pPC->fp_log,"%02x ", pPC->mem[i]);
    fprintf(pPC->fp_log,"\n");
    fprintf(pPC->fp_log,"W=");
    for(int i = 0x420; i <= 0x42e; i++)
        fprintf(pPC->fp_log,"%02x ", pPC->mem[i]);
    fprintf(pPC->fp_log,"\n");

#endif
}
#if 0
/***************************************/
/* ??:comet.c ???                */
/* ??:comet?????               */
/* ??:????                      */
/* ??:1.0 ?                        */
/* ??:???                        */
/* ??:chai2010@2002.cug.edu.cn      */
/* ??:2005-5-3                      */
/***************************************/

#include "comet.h"

/* ??comet?? */

void
comet_load(void)
{
    off_t n, tmp[2];
    fseek(source, 0, SEEK_SET);
    n = fread(tmp, sizeof(off_t), 2, source);
    if(n != 2) {
        printf("?? %s ????\n", pgmName);
        exit(1);
    }
    n = fread(&cmt.mem[tmp[0]], sizeof(off_t), tmp[1], source);
    if(n != tmp[1]) {
        printf("?? %s ????\n", pgmName);
        exit(1);
    }
    cmt.gr[4] = (off_t)sp_start;
    cmt.pc = tmp[0];
}

/* ??comet????IO?? */

void
comet_io(void)
{
    int i;
    off_t addr;
    short count, type, fio;
    char *fmt;
    count = cmt.mem[IO_FLAG] & IO_MAX;
    if(count == 0) return;
    fio = cmt.mem[IO_FLAG] & IO_FIO;
    type = cmt.mem[IO_FLAG] & IO_TYPE;
    addr = cmt.mem[IO_ADDR];
    if(type == IO_CHR) fmt = "%c";
    else if(type == IO_OCT) fmt = "%o";
    else if(type == IO_DEC) fmt = "%d";
    else if(type == IO_HEX) fmt = "%x";
    else {
        cmt.mem[IO_FLAG] &= (!IO_MAX);
        cmt.mem[IO_FLAG] |= IO_ERROR;
        return;
    }
    for(i = 0; i < count; ++i) {
        if(fio == IO_IN)
            scanf(fmt, &cmt.mem[addr++]);
        else if(fio == IO_OUT)
            printf(fmt, cmt.mem[addr++]);
    }
    cmt.mem[IO_FLAG] &= (!IO_MAX);
}

/* ????comet??? */

int
comet_step(void)
{
    off_t temp;
    off_t adr, x1, x2;
    short op, gr, xr;

    op = (off_t)cmt.mem[cmt.pc] / 0x100;
    gr = (off_t)cmt.mem[cmt.pc] % 0x100 / 0x10;
    xr = (off_t)cmt.mem[cmt.pc] % 0x10;
    adr = (off_t)cmt.mem[cmt.pc + 1];

    if(gr < 0 || gr > 4) {
        temp = cmt.mem[cmt.pc];
        printf("????:mem[%x] = %x\n", cmt.pc, temp);
        return 0;
    }
    if(xr < 0 || xr > 4) {
        temp = cmt.mem[cmt.pc];
        printf("????:mem[%x] = %x\n", cmt.pc, temp);
        return 0;
    }
    if(xr != 0) adr += cmt.gr[xr];

    comet_io();

    switch(op) {
        case HALT:
            cmt.pc += 1;
            return 0;
        case LD:
            cmt.pc += 2;
            cmt.gr[gr] = cmt.mem[adr];
            return 1;
        case ST:
            cmt.pc += 2;
            cmt.mem[adr] = cmt.gr[gr];
            return 1;

        case LEA:
            cmt.pc += 2;
            cmt.gr[gr] = adr;
            cmt.fr = cmt.gr[gr];
            return 1;
        case ADD:
            cmt.pc += 2;
            cmt.gr[gr] += cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case SUB:
            cmt.pc += 2;
            cmt.gr[gr] -= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case MUL:
            cmt.pc += 2;
            cmt.gr[gr] *= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case DIV:
            cmt.pc += 2;
            cmt.gr[gr] /= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case MOD:
            cmt.pc += 2;
            cmt.gr[gr] %= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case AND:
            cmt.pc += 2;
            cmt.gr[gr] &= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case OR :
            cmt.pc += 2;
            cmt.gr[gr] |= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case EOR:
            cmt.pc += 2;
            cmt.gr[gr] ^= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;

        case SLA:
            cmt.pc += 2;
            cmt.gr[gr] <<= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;
        case SRA:
            cmt.pc += 2;
            cmt.gr[gr] >>= cmt.mem[adr];
            cmt.fr = cmt.gr[gr];
            return 1;

        case SLL:
            cmt.pc += 2;
            x1 = cmt.mem[gr];
            x1 <<= cmt.mem[adr];
            cmt.mem[gr] = cmt.fr = x1;
            return 1;
        case SRL:
            cmt.pc += 2;
            x1 = cmt.mem[gr];
            x1 >>= cmt.mem[adr];
            cmt.mem[gr] = cmt.fr = x1;
            return 1;

        case CPA:
            cmt.pc += 2;
            cmt.fr = cmt.gr[gr] - cmt.mem[adr];
            return 1;
        case CPL:
            cmt.pc += 2;
            x1 = cmt.gr[gr];
            x2 = cmt.mem[adr];
            cmt.fr = x1 - x2;
            return 1;

        case JMP:
            cmt.pc += 2;
            cmt.pc = adr;
            return 1;
        case JPZ:
            cmt.pc += 2;
            if(cmt.fr >= 0) cmt.pc = adr;
            return 1;
        case JMI:
            cmt.pc += 2;
            if(cmt.fr < 0) cmt.pc = adr;
            return 1;
        case JNZ:
            cmt.pc += 2;
            if(cmt.fr != 0) cmt.pc = adr;
            return 1;
        case JZE:
            cmt.pc += 2;
            if(cmt.fr == 0) cmt.pc = adr;
            return 1;

        case PUSH:
            cmt.pc += 2;
            x1 = --cmt.gr[4];
            cmt.mem[x1] = cmt.mem[adr];
            return 1;
        case POP:
            cmt.pc += 1;
            x1 = cmt.gr[4]++;
            cmt.gr[gr] = cmt.mem[x1];
            return 1;

        case CALL:
            cmt.pc += 2;
            x1 = --cmt.gr[4];
            cmt.mem[x1] = cmt.pc;
            cmt.pc = cmt.mem[adr];
            return 1;
        case RET:
            cmt.pc += 1;
            x1 = cmt.gr[4]++;
            cmt.pc = cmt.mem[x1];
            return 1;

        default :
            temp = cmt.mem[cmt.pc];
            printf("????:mem[%x] = %x\n", cmt.pc, temp);
            return 0;
    }
}

/* ????,???? */

void
writeIns(off_t pc, off_t n)
{
    off_t op, gr, adr, xr;
    off_t i;

    for(i = 0; i < n; ++i) {
        op = cmt.mem[pc] / 0x100;
        gr = cmt.mem[pc] % 0x100 / 0x10;
        xr = cmt.mem[pc] % 0x10;
        adr = cmt.mem[pc + 1];
        if(op > RET) {
            printf("mem[%-4x]: ??\n", pc);
            return;
        }
        if(gr < 0 || gr > 4) {
            printf("mem[%-4x]: ??\n", pc);
            return;
        }
        if(xr < 0 || xr > 4) {
            printf("mem[%-4x]: ??\n", pc);
            return;
        }
        printf("mem[%-4x]: %s\t", pc, opTab[op].str);
        if(op == HALT || op == RET) {
            printf("\n");
            pc += 1;
            continue;
        }else if(op == POP) {
            printf("GR%d\n", gr);
            pc += 1;
            continue;
        }if(op < CPL) {
            printf("GR%d, %x", gr, adr);
            pc += 2;
        }else {
            printf("%x", adr);
            pc += 2;
        }
        if(xr != 0) printf(", GR%d", xr);
        printf("\n");
    }
}

/* ??comet????? */

void
comet_debug(void)
{
    off_t stepcnt = 0;
    off_t pntflag = 0;
    off_t traflag = 0;

    char buf[32], s[32];
    off_t x1, x2;
    int i, n, cmd;

    printf("?? (???? help)...\n\n");

LOOP:

    do {
        fflush(stdin);
        printf ("????: ");
        fgets(buf, NELEMS(buf), stdin);
        n = sscanf(buf, "%s %x %x", s, &x1, &x2);
        for(i = 0, cmd = -1; i < NELEMS(dbTab); ++i) {
            if(!strcmp(dbTab[i].s1, s) ||
                !strcmp(dbTab[i].s2, s)) {
                cmd = dbTab[i].db;
                break;
            }
        }
    }while(n <= 0);

    switch(cmd) {
        case HELP:
            printf("????:\n");
            printf("  h)elp           ???????\n");
            printf("  g)o             ????????\n");
            printf("  s)tep  <n>      ?? n ??? (??? 1 )\n");
            printf("  j)ump  <b>      ??? b ?? (???????)\n");
            printf("  r)egs           ???????\n");
            printf("  i)Mem  <b <n>>  ??? b ?? n ?????\n");
            printf("  d)Mem  <b <n>>  ??? b ?? n ?????\n");
            printf("  a(lter <b <v>>  ?? b ???????? v ?\n");
            printf("  t)race          ????????\n");
            printf("  p)rint          ????????\n");
            printf("  c)lear          ???????\n");
            printf("  q)uit           ?????\n");
            break;

        case GO:
            stepcnt = 0;
            do { stepcnt++;
                if(traflag) writeIns(cmt.pc, 1);
            }while(comet_step());
            if(pntflag)
                printf("?????? = %d\n", stepcnt);
            break;

        case STEP:
            if(n >= 2) stepcnt = x1;
            else stepcnt = 1;
            for(i = 0; i < stepcnt; ++i) {
                if(traflag) writeIns(cmt.pc, 1);
                if(!comet_step()) break;
            }
            if(pntflag)
                printf("?????? = %d\n", i);
            break;

        case JUMP:
            if(n < 2) x1 = cmt.pc;
            cmt.pc = x1;
            printf("????? %x\n", x1);
            break;

        case REGS:
            printf("???????\n");
            printf("GR[0] = %4x\tPC = %4x\n",
                (off_t)cmt.gr[0], cmt.pc);
            printf("GR[1] = %4x\tSP = %4x\n",
                (off_t)cmt.gr[1], (off_t)cmt.gr[4]);
            printf("GR[2] = %4x\t", (off_t)cmt.gr[2]);
            if(cmt.fr > 0) printf("FR =   00\n");
            else if(cmt.fr < 0) printf("FR =   10\n");
            else printf("FR =   01\n");
            printf("GR[3] = %4x\n", (off_t)cmt.gr[3]);
            break;

        case IMEM:
            printf("??????\n");
            if(n < 2) x1 = cmt.pc;
            if(n < 3) x2 = 1;
            writeIns(x1, x2);
            break;


        case DMEM:
            printf("??????\n");
            if(n < 2) x1 = cmt.pc;
            if(n < 3) x2 = 1;
            if(x2 < 0) printf("????\n");
            for(i = 0; i < x2; ++i) {
                off_t temp;
                temp = cmt.mem[x1];
                printf("mem[%-4x] = %x\n", x1, temp);
                x1++;
            }
            break;

        case ALTER:
            printf("?????? ");
            if(n == 3) {
                printf(" mem[%x] = %x\n", x1, x2);
                cmt.mem[x1] = x2;
            }else printf("??!\n");
            break;

        case TRACE:
            traflag = !traflag;
            printf("??????");
            if(traflag) printf("??\n");
            else printf("??\n");
            break;

        case PRINT:
            pntflag = !pntflag;
            printf("??????");
            if(pntflag) printf("??\n");
            else printf("??\n");
            break;

        case CLEAR:
            printf("????????\n");
            comet_load();
            break;

        case QUIT:
            printf("????...\n");
            return;

        default :
            printf("???? %s\n", s);
            break;
    }
    goto LOOP;
}

/* ??????? */

void
init(int n, char*v[])
{
    int len;
    char *s;
    if(system("cls")) system("clear");
    printf("===============\n");
    printf("COMET?????\n");
    printf("===============\n\n");
    if(n != 2 && n != 3) {
        printf("??: %s [-d(ebug] <???>\n", v[0]);
        exit(1);
    }
    len = strlen(v[n-1]);
    if(len > 16) {
        printf("?????");
        exit(1);
    }
    strcpy(pgmName, v[n-1]);
    s = strchr(pgmName, '.');
    if(s != NULL) {
        if(strcmp(s, ".comet")) {
            printf("%s ??comet??\n", pgmName);
            exit(1);
        }
        s = '\0';
    }
    strcat(pgmName, ".comet");
    if(n == 3) {
        if(strcmp(v[n-2], "-d") &&
            strcmp(v[n-2], "-debug")) {
            printf("??: %s [-d(ebug] <???>\n", v[0]);
            exit(1);
        }
        debug = 1;
    }
    source = fopen(pgmName, "rb");
    if(source == NULL) {
        printf("%s ??????\n", pgmName);
        exit(1);
    }
    comet_load();
}

/* ??? */

int
main(int argc, char *argv[])
{
    init(argc, argv);
    if(debug) comet_debug();
    else while(comet_step());
    fclose(source);
    return 0;
}

#endif
