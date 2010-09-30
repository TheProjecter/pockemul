#include "hd61102.h"
#include "pcxxxx.h"

#define MASK_on_off     0x3e
#define MASK_setY       0x40
#define MASK_setX       0xb8
#define MASK_displaySL  0xc0
#define MASK_write      0x100
#define MASK_status     0x200
#define MASK_read       0x300

CHD61102::CHD61102(CpcXXXX *parent)
{
    pPC = parent;

    for (int i = 0 ; i < 0x200 ; i++)
    {
        imem[i] = 0;
    }

    on_off = 0;
    displaySL = 0;
    Xadr = 0;
    Yadr = 0;
    status = 0;
}

BYTE CHD61102::get8(qint16 adr)
{
    if (adr >= 0x200)
    {
        // ERROR
        if (pPC->fp_log) fprintf(pPC->fp_log,"LCD : ERROR adr [%04x] out of range [0,200h]\n",adr);
        return 0;
    }
    return imem[adr];
}

void CHD61102::set8(qint16 adr,BYTE val)
{
    if (adr >= 0x200)
    {
        // ERROR
        if (pPC->fp_log) fprintf(pPC->fp_log,"LCD : ERROR adr [%04x] out of range [0,200h]\n",adr);

        return;
    }
    imem[adr] = val;
}

BYTE CHD61102::instruction(qint16 cmd)
{

    if ((cmd & MASK_read) == MASK_read ) { return cmd_read(cmd); }
    else
    if ((cmd & MASK_status) == MASK_status ) { return cmd_status(cmd); }
    else
    if ((cmd & MASK_write) == MASK_write ) { cmd_write(cmd); }
    else
    if ((cmd & MASK_displaySL) == MASK_displaySL ) { cmd_displaySL(cmd); }
    else
    if ((cmd & MASK_setX) == MASK_setX ) { cmd_setX(cmd); }
    else
    if ((cmd & MASK_setY) == MASK_setY ) { cmd_setY(cmd); }
    else
    if ((cmd & MASK_on_off) == MASK_on_off ) { cmd_on_off(cmd); }

    return 0;
}

void CHD61102::cmd_on_off(qint16 cmd)
{
    on_off = cmd & 0x01;
//    if (pPC->fp_log) fprintf(pPC->fp_log,"LCD :%s\n",on_off?"on":"off");
}

void CHD61102::cmd_displaySL(qint16 cmd)
{
    displaySL = cmd & 0x3f;
//    if (pPC->fp_log) fprintf(pPC->fp_log,"LCD DisplaySL:%i\n",displaySL);
}

void CHD61102::cmd_setX(qint16 cmd)
{
    Xadr = cmd & 0x07;
//    if (pPC->fp_log) fprintf(pPC->fp_log,"LCD X:%i\n",Xadr);
}

void CHD61102::cmd_setY(qint16 cmd)
{
    Yadr = cmd & 0x3f;
//    if (pPC->fp_log) fprintf(pPC->fp_log,"LCD Y:%i\n",Yadr);
}

BYTE CHD61102::cmd_status(qint16 cmd)
{
//    if (pPC->fp_log) fprintf(pPC->fp_log,"LCD request status\n");
    return on_off ? 0x00 : 0x20 ;
}

void CHD61102::cmd_write(qint16 cmd)
{
//    if ((pPC->pCPU->fp_log) && (cmd & 0xff))fprintf(pPC->pCPU->fp_log,"LCD Write:%02x\n",cmd & 0xff);
//    if ((pPC->fp_log) && (cmd & 0xff)) fprintf(pPC->fp_log,"LCD Write:x=%02x y=%02x val=%02x\n",Xadr,Yadr,cmd & 0xff);
    set8( (Xadr * 0x40) + Yadr , (cmd & 0xff));
    Yadr++;
    if (Yadr == 64) {
        Yadr=0;
    }
}

BYTE CHD61102::cmd_read(qint16 cmd)
{

    BYTE value = get8((Xadr * 0x40) + (Yadr==0 ? 63 : Yadr-1) );
    Yadr++;
    if (Yadr == 64) {
        Yadr=0;
    }

//    if (pPC->fp_log) fprintf(pPC->fp_log,"LCD Read:%02x\n",value);
    return value;
}
