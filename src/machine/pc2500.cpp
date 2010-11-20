#include "pc2500.h"

#define KEY(c)	( toupper(pKEYB->LastKey) == toupper(c) )
BYTE	Cpc2500::Get_PortA(void)
{
    IO_A = Cpc13XX::Get_PortA();

    BYTE ks = Get_8(0x7A00);

    if (ks & 0x20) {
        if (pKEYB->isShift)
            IO_A|=0x01;
    }

    return (IO_A);
}

void	Cpc2500::Set_PortF(BYTE data)
{
    IO_F = data;
    ProtectMemory = GET_PORT_BIT(PORT_F,1);
}

BYTE Cpc2500::Get_PC(DWORD adr)
{
    //Chk_Adr_R(&adr,bREAD);
    if ( (adr >= 0x8000) && (adr<=0xFFFF) && (RomBank & 0x02) ) {
        adr += 0x8000;
    }
    return(mem[adr]);
}

WORD Cpc2500::Get_16rPC(DWORD adr)
{
    DWORD	a;

    if ( (adr >= 0x8000) && (adr<=0xFFFF) && (RomBank & 0x02) ) {
        adr += 0x8000;
    }
    a=adr+1;
    //Chk_Adr_R(&adr,bREAD);
    //Chk_Adr_R(&a,bREAD);
    //if (pCPU->fp_log) fprintf(pCPU->fp_log,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    return((mem[adr]<<8)+mem[a]);
}

bool Cpc2500::Chk_Adr(DWORD *d,DWORD data)
{
    if (ProtectMemory) {
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"ECRITURE EXT [%04x]=%02x (%c)\n",*d,data,data);
        if ( (*d>=0x8000) && (*d<=0xFFFF) ){

            *d += 0x8000 ;
            return(1);
        }
        return (1);
    }

    if ( (*d>=0x7000) && (*d<=0x79FF) ) {
        if (pCPU->fp_log) fprintf(pCPU->fp_log,"ECRITURE [%04x]=%02x (%c)\n",*d,data,data);
    }
    if ( (*d>=0x7100) && (*d<=0x71FF) )	{
        RomBank = data;
        return(1);
    }

    if ( (*d>=0x7000) && (*d<=0x79FF) )	{pLCDC->SetDirtyBuf(*d-0x7000);return(1);}
    if ( (*d>=0x7A00) && (*d<=0x7AFF) )
    {
        pKEYB->Set_KS( (pKEYB->Get_KS() & 0xF0) | ((BYTE) data & 0x0F ));
        return(1);
    }
    if ( (*d>=0x7B00) && (*d<=0x7BFF) )
    {
        pKEYB->Set_KS( (pKEYB->Get_KS() & 0x0f) | (((BYTE) data & 0x0F )<<4));
        return(1);
    }
    if (pCPU->fp_log) fprintf(pCPU->fp_log,"ECRITURE [%04x]=%02x (%c)\n",*d,data,data);

    if ( (*d>=0x2000) && (*d<=0x3FFF) && EXTENSION_CE201M_CHECK ) { *d+=0x2000;}										// 8Kb Ram Card Image
    if ( (*d>=0x2000) && (*d<=0x3FFF) )	{ return( EXTENSION_CE202M_CHECK | EXTENSION_CE203M_CHECK); }							// 16K
    if ( (*d>=0x4000) && (*d<=0x5FFF) )	{ return( EXTENSION_CE201M_CHECK | EXTENSION_CE202M_CHECK | EXTENSION_CE203M_CHECK); }	// 32K
    if ( (*d>=0x6000) && (*d<=0x7FFF) ) return(1);																				// Internal RAM area(6000-8000)

    return(0);

}

bool Cpc2500::init(void) {
    Cpc1350::init();
    pce515p->init();
}

bool Cpc2500::Chk_Adr_R(DWORD *d,DWORD data)
{


    if (ProtectMemory) {
        if ( (*d>=0x8000) && (*d<=0xFFFF) )	{
            *d += 0x8000 ;
            if (pCPU->fp_log) {
                fprintf(pCPU->fp_log,"READ EXT ROM [%04x] : %c (%02x)\n",*d-0x8000,mem[*d],mem[*d]);
            }
            return (1);
        }
        return (1);
    }
    if ( (*d>=0x2000) && (*d<=0x3FFF) && EXTENSION_CE201M_CHECK ) { *d+=0x2000;}										// 8Kb Ram Card Image

    if (pCPU->fp_log) fprintf(pCPU->fp_log,"LECTURE [%04x]=%02x (%c)\n",*d,mem[*d],mem[*d]);
    if ( (*d>=0x0000) && (*d<=0x1FFF) ) { return(0);}
    return(1);
}

bool Cpc2500::run(void)
{
    Set_Port_Bit(PORT_B,8,1);
// Connect CE126
    //pCONNECTOR->Set_values( pce515p->pCONNECTOR->Get_values());

    Cpc1350::run();

    //pce515p->pCONNECTOR->Set_values(pCONNECTOR->Get_values());
    pce515p->set_SD(Get_Port_bit(PORT_F,3));
    pce515p->run();

    return true;
}

#define SIO_GNDP	1
#define SIO_SD 		2
#define SIO_RD		3
#define SIO_RS		4
#define SIO_CS		5

#define SIO_GND		7
#define SIO_CD		8
#define SIO_VC1		10
#define SIO_RR		11
#define SIO_PAK		12
#define SIO_VC2		13
#define SIO_ER		14
#define SIO_PRQ		15

bool Cpc2500::Set_Connector(void)
{
#if 0
    // MANAGE STANDARD CONNECTOR
    pCONNECTOR->Set_pin(PIN_MT_OUT2	,0);
    pCONNECTOR->Set_pin(PIN_GND		,0);
    pCONNECTOR->Set_pin(PIN_VGG		,1);
    pCONNECTOR->Set_pin(PIN_BUSY	,GET_PORT_BIT(PORT_F,3));		// F03
    pCONNECTOR->Set_pin(PIN_D_OUT	,GET_PORT_BIT(PORT_F,2));		// F02
    pCONNECTOR->Set_pin(PIN_MT_OUT1	,pCPU->Get_Xout());
    pCONNECTOR->Set_pin(PIN_SEL2	,0);
    pCONNECTOR->Set_pin(PIN_SEL1	,0);
#endif

    // MANAGE SERIAL CONNECTOR
    // TO DO
    pSIOCONNECTOR->Set_pin(SIO_SD	,GET_PORT_BIT(PORT_F,4));	//	pSIO->Set_SD((IO_F & 0x08) ? 1 : 0);
    pSIOCONNECTOR->Set_pin(SIO_RR	,GET_PORT_BIT(PORT_B,3));	//( READ_BIT(IO_B,2) );
    pSIOCONNECTOR->Set_pin(SIO_RS	,GET_PORT_BIT(PORT_B,2));	//( READ_BIT(IO_B,1) );
    pSIOCONNECTOR->Set_pin(SIO_ER	,GET_PORT_BIT(PORT_B,1));	//( READ_BIT(IO_B,0) );

    return(1);
}

bool Cpc2500::Get_Connector(void)
{
    // MANAGE STANDARD CONNECTOR
    Set_Port_Bit(PORT_B,8,0);	// DIN	:	IB8
    Set_Port_Bit(PORT_B,7,1);	// ACK	:	IB7
    //pCPU->Set_Xin(pCONNECTOR->Get_pin(PIN_MT_IN));

    // MANAGE SERIAL CONNECTOR
    // TO DO
//	Set_Port_Bit(PORT_B,4,pSIOCONNECTOR->Get_pin(SIO_RD));
//	Set_Port_Bit(PORT_B,5,pSIOCONNECTOR->Get_pin(SIO_CS));
//	Set_Port_Bit(PORT_B,6,pSIOCONNECTOR->Get_pin(SIO_CD));
    // PAK
    return(1);
}
