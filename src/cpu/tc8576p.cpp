
#include "tc8576p.h"
#include "pcxxxx.h"


CTC8576P::CTC8576P(CpcXXXX *parent, int clk)
{
    pPC = parent;

    xclk = clk;

    psr=0;
    ssr=0;
}

bool CTC8576P::init(void)
{
    return true;
}

bool CTC8576P::exit(void)
{
    return true;
}

void CTC8576P::Reset(void)
{
    if (pPC->fp_log) fprintf(pPC->fp_log,"TC8576P - RESET\n");
    psr = ssr = 0;

    IntF = false;
    XBUSY = false;
    BUSY = false;
    PRIME = false;
    P5V = false;
    PE = false;
    SLCT = false;
    FAULT = false;
    IM1 = false;
    IM2 = false;

    DSR = false;
    RBRK = false;
    FE = false;
    OE = false;
    PERR = false;
    TxE = false;
    RxRDY = false;
    TxRDY = false;

    TxEN = false;
    DTR = false;
    RxEN = false;
    SBRK = false;
    ERS = false;
    RTS = false;

    SO = false;
    TxINTM = false;
    CL = 5;
    PEN = false;
    EP = false;
    ERINTM = false;
    RXINTM = false;


}

bool CTC8576P::step(void)
{
// Get the connector Informations depending of the correct comm device (RS232 - SIO)

    // Set ssr
//    ssr = 0;

    // Set prs
//    psr = 0;

    TxE = true;
//    BUSY = true;

    return true;
}


// Write operation
bool CTC8576P::instruction(BYTE cmd)
{
    if (pPC->fp_log) fprintf(pPC->fp_log,"TC8576P - COMMAND - %02x\n",cmd);
    // Serial Command Register
    // b7 = 0; command byte=b0->b6
    if (!(cmd & 0x80)) {
        SCR(cmd & 0x7f);
    }

    switch (cmd >> 6) {
    case 0x02:  // Parallel Command Register
                PCR(cmd & 0x3f);
                break;
    case 0x03: // Parameter Address Set Register
                if (cmd & 0x20) Reset();
                else
                    par = (cmd & 0x07);
                break;
    }

    return true;
}

BYTE CTC8576P::get_psr(void)
{
    psr = 0;

    PUT_BIT(psr,7,IntF);
    PUT_BIT(psr,6,XBUSY);
    PUT_BIT(psr,5,BUSY);
    PUT_BIT(psr,4,PRIME);
    PUT_BIT(psr,3,P5V);
    PUT_BIT(psr,2,PE);
    PUT_BIT(psr,1,SLCT);
    PUT_BIT(psr,0,FAULT);

    return psr;
}

BYTE CTC8576P::get_ssr(void)
{
    ssr = 0;

    PUT_BIT(ssr,7, DSR);
    PUT_BIT(ssr,6,RBRK);
    PUT_BIT(ssr,5,FE);
    PUT_BIT(ssr,4,OE);
    PUT_BIT(ssr,3,PERR);
    PUT_BIT(ssr,2,TxE);
    PUT_BIT(ssr,1,RxRDY);
    PUT_BIT(ssr,0,TxRDY);

    //if (pPC->fp_log) fprintf(pPC->fp_log,"TC8576P - getSSR - %02x\n",ssr);
    return ssr;
}

void CTC8576P::SCR(BYTE cmd)
{
    //if (pPC->fp_log) fprintf(pPC->fp_log,"TC8576P - SCR - %02x\n",cmd);

    scr = cmd;
    TxEN = READ_BIT(scr,0);
    DTR = READ_BIT(scr,1);
    RxEN = READ_BIT(scr,2);
    SBRK = READ_BIT(scr,3);
    ERS = READ_BIT(scr,4);
    RTS = READ_BIT(scr,5);

}

void CTC8576P::PCR(BYTE cmd)
{
    if (pPC->fp_log) fprintf(pPC->fp_log,"TC8576P - PCR - %02x\n",cmd);

    pcr = cmd;

    switch (cmd & 0x07){
    case 0: FAULT = 0; break;
    case 1: SLCT = 0; break;
    case 2: PE = 0; break;
    case 3: P5V = 0; break;
    case 4: PRIME = 1; break;
    case 5: PRIME = 1; break;
    case 6: PRIME = 0; break;
    case 7: break;
    }

    IM1 = READ_BIT(cmd,5);
    IM2 = READ_BIT(cmd,4);
}

void CTC8576P::SPR(void)
{
    SO = READ_BIT(pr[5],0);
    TxINTM = READ_BIT(pr[5],1);
    CL = ((pr[5]>> 2) & 0x03) + 5;
    PEN = READ_BIT(pr[5],4);
    EP = READ_BIT(pr[5],5);
    ERINTM = READ_BIT(pr[5],6);
    RXINTM = READ_BIT(pr[5],7);
}

#define NB_STOP_BITS    ((pr[5] & 0x01) ? 2 : 1)
#define INT_CTRL_TxRDY  (!((pr[5]>>1) &0x01 ))
#define CHAR_LENGTH     ( 5 + ((pr[5]>>2) & 0x03))


bool CTC8576P::in(BYTE data)
{
    if (pPC->fp_log) fprintf(pPC->fp_log,"TC8576P - IN - %02x, par=%i\n",data,par);

    pr[par] = data;

    switch (par)
    {
    case 0x00: baud(data); break;       // Baud divider BL
    case 0x01: baud((data&0x0f) << 8); break;  // Baud divider BH
    case 0x02: break;      // Delay time
    case 0x05: SPR(); break;
    case 0x07: prescaler(data & 0x0f); break;  // Prescaler set
    }

    return true;
}

void CTC8576P::baud(qint16 data)
{


    int b = pr[0] + ((pr[1] & 0x0f) << 8); 

    switch (b) {
        case 0: baudrate = fsysclk / 4096 / 8; break;
        case 1: baudrate = 0; break;
        default: baudrate = fsysclk / b / 8; break;
    }
    if (pPC->fp_log) fprintf(pPC->fp_log,"TC8576P - BAUD - %02x = %d\n",data,baudrate);
}

void CTC8576P::prescaler(qint8 data)
{
    switch (data){
        case 0: fsysclk = xclk / 16; break;
        default: fsysclk = xclk / data; break;
    }
    if (pPC->fp_log) fprintf(pPC->fp_log,"TC8576P - PRESCALER - %02x\n",data);
}
