#include "cf79107pj.h"
#include "pcxxxx.h"
#include "cpu.h"

#define VAR_X 0x400
#define VAR_Y 0x410
#define VAR_W 0x420

CCF79107PJ::CCF79107PJ(CpcXXXX *parent)
{
    pPC = parent;

}

CCF79107PJ::~CCF79107PJ()
{
}


bool CCF79107PJ::init()
{
    return true;
}

bool CCF79107PJ::exit()
{
    return true;
}

void CCF79107PJ::Reset()
{
}

bool CCF79107PJ::step()
{
    return true;
}
UINT8 CCF79107PJ::get_status()
{
    return (BCDz?0x40:0x00) | (BCDaf?0x10:0x00) | (BCDc?0x1:0x00);
}
bool CCF79107PJ::instruction1(UINT8 cmd)
{
    regSelected = 0;
    switch(cmd) {
    case 0x04: Write_TMP(VAR_X); /*regSelected = VAR_X*/;break;
    case 0x05: Write_TMP(VAR_Y); /*regSelected = VAR_Y*/;break;
    case 0x08: Write_TMP(VAR_X); /*regSelected = VAR_X*/;break;
    case 0x0e:
        if (pPC->fp_log) fprintf(pPC->fp_log,"\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        cmd_0e();
        if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        break;
    case 0x10:  // X <- 0
        if (pPC->fp_log) fprintf(pPC->fp_log,"\nX<-0\nbeforeCCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        for(int i = 0x400; i <= 0x408; i++)
            pPC->mem[i]= 0;
//        pPC->mem[0x418] |= 0x06;
        if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        break;
    case 0x11: // Y <- 0
        if (pPC->fp_log) fprintf(pPC->fp_log,"\nY<-0\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
        dumpXYW();
        for(int i = 0x410; i <= 0x418; i++)
            pPC->mem[i]= 0;
//        pPC->mem[0x408] |= 0x06;
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
    if (pPC->fp_log) fprintf(pPC->fp_log,"\nbefore CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
    dumpXYW();
    last_cmd = cmd;
    regSelected = 0;


    switch (cmd & 0x02) {
    case 0x00: Read_TMP(VAR_X); break;
    case 0x02: Read_TMP(VAR_Y); break;
    }

    switch (cmd) {
    case 0x00: push(VAR_X);cmd_add_exp();         break;
    case 0x01: push(VAR_X);cmd_sub_exp();         break;
    case 0x41: cmd_sub_mantisseXY();              break;
    case 0x43: cmd_sub_mantisseYX();              break;
    case 0x48: cmd_shiftR_mantisse(); break;
    case 0x4a: cmd_shiftR_mantisse(); break;
    case 0x4c: cmd_shiftL_mantisse(); break;
    case 0x4e: cmd_shiftL_mantisse(); break;
    case 0x90: cmd_inc_exp();               break;
    case 0x91: cmd_dec_exp();               break;
    case 0x99: cmd_shiftR_mantisse();
               cmd_dec_exp();
               break;
    case 0x9c: cmd_shiftL_mantisse();
               cmd_inc_exp();
               break;
    case 0xc0: cmd_add_mantisse();    break;
    case 0xc1: cmd_sub_mantisseXY(); /*BCDc |= BCDaf;*/       break;
    case 0xc2: cmd_add_mantisse();    break;
    case 0xc3: cmd_sub_mantisseYX();        break;
    case 0xd0: // X -> Y
        for(int i = 0; i <= 8; i++)
            TMP[i] = pPC->mem[i+VAR_X];
//        Write_TMP(regSelected);
        break;
    default:
        if (pPC->fp_log) fprintf(pPC->fp_log,"UNKNOWN");
    }

    regSelected = 0;

    if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
    dumpXYW();
    return true;
}

bool CCF79107PJ::instruction3(UINT16 cmd)
{
    if (pPC->fp_log) fprintf(pPC->fp_log,"\nbefore CCF79107[1]=%04x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
    dumpXYW();

    regSelected = 0;
//    switch(cmd>>8) {
//    case 0x04: regSelected = VAR_X; Read_TMP(VAR_X); break;
//    case 0x05: regSelected = VAR_Y; Read_TMP(VAR_Y); break;
//    case 0x08: regSelected = VAR_X; Read_TMP(VAR_X); break;
//    default:   if (pPC->fp_log) fprintf(pPC->fp_log,"UNKNOWN HB");
//        return true;
//    }

    switch (cmd & 0x02) {
    case 0x00: Read_TMP(VAR_X); break;
    case 0x02: Read_TMP(VAR_Y); break;
    }

    switch (cmd&0xff) {
    case 0x00: push(VAR_X);cmd_add_exp(); break;
    case 0x01: push(VAR_X);cmd_sub_exp(); break;
    case 0x41: cmd_sub_mantisseXY();        break;
    case 0x43: cmd_sub_mantisseYX();        break;
    case 0x48: cmd_shiftR_mantisse(); break;
    case 0x4a: cmd_shiftR_mantisse(); break;
    case 0x4c: cmd_shiftL_mantisse(); break;
    case 0x4e: cmd_shiftL_mantisse(); break;
    case 0x90: cmd_inc_exp();         break;
    case 0x91: cmd_dec_exp();         break;
    case 0x99: cmd_shiftR_mantisse();
               cmd_dec_exp();
               break;
    case 0x9c: cmd_shiftL_mantisse();
               cmd_inc_exp();
               break;
    case 0xc0: cmd_add_mantisse();    break;
    case 0xc1: cmd_sub_mantisseXY();        break;
    case 0xc2: cmd_add_mantisse();    break;
    case 0xc3: cmd_sub_mantisseYX();        break;
    case 0xd0: // X -> Y
        for(int i = 0; i <= 8; i++)
            TMP[i] = pPC->mem[i+VAR_X];
//        Write_TMP(regSelected);
        break;
    default:
        if (pPC->fp_log) fprintf(pPC->fp_log,"UNKNOWN");
    }

    switch(cmd>>8) {
    case 0x04: Write_TMP(VAR_X); /*regSelected = VAR_X*/;break;
    case 0x05: Write_TMP(VAR_Y); /*regSelected = VAR_Y*/;break;
    case 0x08: Write_TMP(VAR_X); /*regSelected = VAR_X*/;break;
    default:   if (pPC->fp_log) fprintf(pPC->fp_log,"UNKNOWN HB");
        return true;
    }

    regSelected = 0;

    if (pPC->fp_log) fprintf(pPC->fp_log,"after CCF79107[1]=%02x\tpc=%08x\n",cmd,pPC->pCPU->get_PC());
    dumpXYW();
    return true;
}

void CCF79107PJ::cmd_shiftL_mantisse(void) {
//    Read_TMP(src);

    for(int i = 0; i < 6; i++) {
        quint8 _tmp = TMP[i]&0xf0;
        TMP[i] = (TMP[i+1]&0x0f)<<4;
        TMP[i] |= _tmp>>4;
    }
    TMP[6] = (TMP[6]&0xf0)>>4;

//    Write_TMP(adr);
}

void CCF79107PJ::cmd_shiftR_mantisse(void) {
//    Read_TMP(src);

    for(int i = 6; i > 0; i--){
        quint8 _tmp = TMP[i]&0x0f;
        TMP[i] = (TMP[i-1]&0xf0)>>4;
        TMP[i] |= _tmp<<4;
    }
    TMP[0]=(TMP[0]&0x0f)<<4;
//    Write_TMP(adr);
}

void CCF79107PJ::Read_TMP(UINT16 adr) {
    memcpy((char*)&TMP,(char*)&(pPC->mem[adr]),0x10);
}

void CCF79107PJ::Write_TMP(UINT16 adr) {
    if (adr != 0) {
        memcpy((char*)&(pPC->mem[adr]),(char*)&TMP,0x10);
    }
}

void CCF79107PJ::pop(UINT16 adr) {
    memcpy((char*)&(pPC->mem[adr]),(char*)&STACK,0x10);
}
void CCF79107PJ::push(UINT16 adr) {
    memcpy((char*)&STACK,(char*)&(pPC->mem[adr]),0x10);
}

void CCF79107PJ::cmd_inc_exp() //sbbw
{
    UINT16 arg = 7;
    UINT16 res0, res1;

//    Read_TMP(source);

    res0 = make_bcd_add(TMP[7], 1);
    TMP[7] = res0 & 0xff;
    res1 = (res0>0xff) ? 1 : 0 ;
    res1 = make_bcd_add(TMP[8],  res1);
    TMP[8] = res1 & 0xff;

    BCDz = ((res0 || res1)==0 );
    BCDc = (res1 > 0xff);


//    Write_TMP(adr);
}

void CCF79107PJ::cmd_dec_exp(void) //sbbw
{
    UINT16 arg = 7;
    UINT16 res0, res1;

//    Read_TMP(source);

    res0 = make_bcd_sub(TMP[7], 1);
    TMP[7] = res0 & 0xff;
    res1 = (res0>0xff) ? 1 : 0 ;
    res1 = make_bcd_sub(TMP[8],  res1);
    TMP[8] = res1 & 0xff;

    BCDz = ((res0 || res1)==0);
    BCDc = (res1 > 0xff);

//    Write_TMP(adr);
}

void CCF79107PJ::cmd_add_exp(void) //adbw
{
    UINT16 res0, res1;

//    Read_TMP(target);

    res0 = make_bcd_add(pPC->mem[VAR_X+7], pPC->mem[VAR_Y+7]);
    TMP[7] = res0 & 0xff;
    res1 = (res0>0xff) ? 1 : 0 ;
    res1 = make_bcd_add(pPC->mem[VAR_X+8], pPC->mem[VAR_Y+8] + res1);
    TMP[8] = res1 & 0xff;

    BCDz = ((res0 || res1)==0);
    BCDc = (res1 > 0xff);

//    Write_TMP(target);
}

void CCF79107PJ::cmd_sub_exp() //sbbw
{
    UINT16 res0, res1;

//    Read_TMP(target);

    res0 = make_bcd_sub(pPC->mem[VAR_X+7], pPC->mem[VAR_Y+7]);
    TMP[7] = res0 & 0xff;
    res1 = (res0>0xff) ? 1 : 0 ;
    res1 = make_bcd_sub(pPC->mem[VAR_X+8], pPC->mem[VAR_Y+8] + res1);
    TMP[8] = res1 & 0xff;

    BCDz = ((res0 || res1)==0);
    BCDc = (res1 > 0xff);

//    pPC->mem[VAR_X+7] = TMP[7];
//    pPC->mem[VAR_X+8] &= 0x0F;
//    pPC->mem[VAR_X+8] |= (TMP[8] & 0x0f)<<4;
//    Write_TMP(target);
}

// Not satisfied by this function. It seems to be called at init. I think it is a BCDret clear function.
void CCF79107PJ::cmd_0e(void) //adbw
{

    pop(VAR_X);

    BCDret = 0;
    BCDz = 0;
    BCDc = 0;
    BCDaf = 0;
}

// adbm	$10,$sz,7
// X = X + Y    return 40h if ok
void CCF79107PJ::cmd_add_mantisse(void) {

    UINT8 c, f;
    UINT16 res = 0;

//    Read_TMP(source);

    f = 0;
    c = 0;
    for (int n=0; n<7; n++)
    {
        res = make_bcd_add(pPC->mem[VAR_X+n], pPC->mem[VAR_Y+n] + c);
        c = (res > 0xff) ? 1 : 0;
        f |= (res&0xff);
        TMP[n] = res&0xff;
        fprintf(pPC->fp_log,"mem[%04x]=%02X  f=%c c=%c\n",n,res&0xff,(f==0?'1':'0'),res>0xff?'1':'0');
    }
    BCDz = (f==0);
    BCDc = (res > 0xff);

//    Write_TMP(target);
}

void CCF79107PJ::cmd_sub_mantisseXY(void) {

    UINT8 c, f;
    UINT16 res = 0;

//    Read_TMP(VAR_X);

    c = 0;
    f = 0;
    for (int n=0; n<7; n++)
    {
        res = make_bcd_sub(pPC->mem[VAR_X+n], pPC->mem[VAR_Y+n] + c);
        c = (res > 0xff) ? 1 : 0;
        f |= (res&0xff);
        TMP[n] = res&0xff;
        fprintf(pPC->fp_log,"mem[%04x]=%02X  f=%c c=%c af=%c\n",n,res&0xff,(f==0?'1':'0'),res>0xff?'1':'0',
                BCDaf?'1':'0');
    }
    BCDz = (f==0);
    BCDc = (res > 0xff);
//    BCDc = BCDaf;
//    BCDc = (f==0);

//    Write_TMP(target);
}


void CCF79107PJ::cmd_sub_mantisseYX(void) {
    UINT8 c, f;
    UINT16 res = 0;

//    Read_TMP(VAR_Y);

    c = 0;
    f = 0;
    for (int n=9; n<7; n++)
    {
        res = make_bcd_sub(pPC->mem[VAR_Y+n], pPC->mem[VAR_X+n] + c);
        c = (res > 0xff) ? 1 : 0;
        f |= (res&0xff);
        TMP[n] = res&0xff;
        fprintf(pPC->fp_log,"mem[%04x]=%02X  f=%c c=%c\n",n,res&0xff,(f==0?'1':'0'),res>0xff?'1':'0');
    }
    BCDz = (f==0);
    BCDc = (res > 0xff);
//    BCDc = (f==0);

//    Write_TMP(target);
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
        BCDaf = true;
    }
    else {
        carry = 0;
        BCDaf = false;
    }

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

    if (ret > 0x09) {
        ret = (ret - 0x06) & 0x0f;
        carry = 1;
        BCDaf = true;
    }
    else {
        carry = 0;
        BCDaf = false;
    }

    ret += ((arg1&0xf0) - (arg2&0xf0) - (carry<<4));
    if (ret > 0x9f) {
        ret = (ret - 0x60) & 0x0ff;
        carry = 1;
    }
    else carry = 0;

    ret -= (carry<<8);
    return ret;
}

void CCF79107PJ::dumpXYW(void) {
#if 0
    QString _tmp="";
    for(int i = 0; i <= 0x0f; i++) {
        _tmp = _tmp+QString("%1 ").arg(TMP[i],2,16,QChar('0'));
    AddLog(LOG_TEMP,"T="+_tmp);
    _tmp="";
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
    if (pPC->fp_log) {
        fprintf(pPC->fp_log,"BCDz=%02x  BCDc=%02x\n",BCDz,BCDc);
        fprintf(pPC->fp_log,"T=");
        for(int i = 0x0; i <= 0x0f; i++)
            fprintf(pPC->fp_log,"%02x ", TMP[i]);
        fprintf(pPC->fp_log,"\n");
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
    }
#endif
}


/*
      0  /  1
Bit 0 : Add  / Sub
Bit 1 :
Bit 2 : Left / Right
Bit 3 : no   / Shift
Bit 4 :   Exponent

Bit 6 :   Mantisse

04  X
05  Y
08  ???

41	01000001	sub mantisse flags
43	01000011	sub mantisse flags and ???
48	01001000	Shift Left
4A	01001010	Shift Left and ???
4C	01001100	Shift Right
4E	01001110	Shift Right and ???
90	10010000	Add Exp 1
91	10010001	Sub Exp 1
99	10011001	Shift Left and sub Exp 1
9c	10011100	Shift Right X and add Exp 1
C0	11000000	Add Mantisse
C1	11000001	Sub Mantisse
C2	11000010	Add Matisse and ???
C3	11000011	Sub Matisse and ???
D0	11010000	Cpy X to

08 00	Add exp
08 01	Sub exp

port 221:
0E	00001110	???
10	00010000	clear X
11	00010001	clear Y
80	10000000	swap X Y

RESET X : 221h à 10h	semble mettre a 0  400h-41Fh
RESET Y : 221h à 11h
SET Y = 1 :   RESET Y puis mise à jours de 416h et 418h à 1

220h 41h : equivalent of HD61700 sbbcm instuction (
220h 43h : compare mantissa ???
221h à 80h : Y <-> X
220h 04C0h : X = X + Y:  return 40h if ok
220h 04C1h : X = X - Y:  return 40h if ok
220h 0491h : equi sbbw	$17,$sy
220h 044Ch : divide mantisse by 10 eq. shift left 400h-406h
220h 0448h : shift right 400h-406h
220h 0499h : shift right 400h-406h
220h 054Eh : ???
220h 05D0h : X->Y (used by X*X -> 05D0 and Multiply)

*/
/*
0005 00C1
00010203040506070E090A0B0C0D0E0F
908989898989890708191A1B1C1D1E1F
202122232425262728292A2B2C2D2E2F

  */
