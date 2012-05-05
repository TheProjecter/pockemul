//#include <stdlib.h>
//#include <string.h>

#include "common.h"
#include "lh5810.h"
#include "lh5801.h"
#include "pc1500.h"
#include "ce152.h"

#define PB7	((lh5810.r_opb & 0x80)?TRUE:FALSE) 

//-----------------------------------------------------
// TO DO
//-----------------------------------------------------
// ADD THE SERIAL CAPABILITIES FOR TAPE SAVE AND LOAD
//-----------------------------------------------------

CLH5810::CLH5810(CPObject *parent)	: CPObject(parent)				//[constructor]
{
	lh5810.r_g=lh5810.r_msk=lh5810.r_dda=lh5810.r_ddb=lh5810.r_opa=lh5810.r_opb=lh5810.r_opc=lh5810.r_f=0;
	lh5810.r_if=0;
	IRQ=INT=FALSE;
	OPA=OPB=0;

}

CLH5810::~CLH5810(){			//[destructor]

}


bool	CLH5810::init(void){return true;}					//initialize
bool	CLH5810::exit(void){return true;}						//end
void	CLH5810::Load_Internal(QFile *file){
    char t[16];
    QDataStream in(file);

    in.readRawData(t, 9);
    in.readRawData((char*)&lh5810,sizeof(lh5810));
}

void	CLH5810::save_internal(QFile *file){
    QDataStream out(file);

    out.writeRawData("LH5810STA", 9);					//header
    out.writeRawData((char*)&lh5810,sizeof(lh5810));		//reg
}

void CLH5810::Load_Internal(QXmlStreamReader *xmlIn)
{
    if (xmlIn->readNextStartElement()) {
        if ( (xmlIn->name()=="cpu") &&
             (xmlIn->attributes().value("model").toString() == "lh5810")) {
            QByteArray ba_reg = QByteArray::fromBase64(xmlIn->attributes().value("registers").toString().toAscii());
            memcpy((char *) &lh5810,ba_reg.data(),sizeof(lh5810));
        }
        xmlIn->skipCurrentElement();
    }
}

void CLH5810::save_internal(QXmlStreamWriter *xmlOut)
{
    xmlOut->writeStartElement("cpu");
        xmlOut->writeAttribute("model","lh5810");
        QByteArray ba_reg((char*)&lh5810,sizeof(lh5810));
        xmlOut->writeAttribute("registers",ba_reg.toBase64());
    xmlOut->writeEndElement();
}

void	CLH5810::Reset(void)
{
	lh5810.r_g=lh5810.r_msk=lh5810.r_if=lh5810.r_opa=lh5810.r_opb=lh5810.r_opc=lh5810.r_f=0X00;
	lh5810.r_dda=lh5810.r_ddb=0xFF;

};

INLINE bool CLH5810::step()
{
	INT = false;
	// If the L register change, then TD flag of the IF register down
	if (New_L)
	{
        //AddLog(LOG_TAPE,tr("L register change -> %1X").arg(lh5810.r_l,4,16,QChar('0')));
		SetRegBit(LH5810_IF,3,false);
		New_L=false;
	}
	if (IRQ)	lh5810.r_if|=0x01;
	if (PB7)	lh5810.r_if|=0x02;

	if (
		( (lh5810.r_msk & 0x01) && IRQ ) ||
		( (lh5810.r_msk & 0x02) && PB7 )
		)
	{
		INT=true;
//        if (pPC->pCPU->fp_log) fprintf(pPC->pCPU->fp_log,"INT\n");
	}
	
	
	return(1);
}





UINT8 CLH5810::GetReg(LH5810_REGS reg)
{
	UINT8 t=0;

	switch (reg)
	{
	case LH5810_U:		return(lh5810.r_u);
	case LH5810_L:		return(lh5810.r_l);
	case LH5810_G:		return(lh5810.r_g);
	case LH5810_MSK:	t=(lh5810.r_msk);
						if (IRQ)	t|=0x10;
						if (PB7)	t|=0x20;
						return(t);
	case LH5810_IF:		t=(lh5810.r_if);
//						if (IRQ)	t|=0x01;
//						if (PB7)	t|=0x02;
						return(t);
	case LH5810_DDA:	return(lh5810.r_dda);
	case LH5810_DDB:	return(lh5810.r_ddb);
	case LH5810_OPA:	return(lh5810.r_opa & (~lh5810.r_dda));	//OK
	case LH5810_OPB:	return(lh5810.r_opb & (~lh5810.r_ddb));	//OK
	case LH5810_OPC:	return(lh5810.r_opc);
	case LH5820_F:		return(lh5810.r_f);

	}
	return(0);
}

	
UINT8 CLH5810::SetReg(LH5810_REGS reg, UINT8 data)	// return the value written
{
	switch (reg)
	{
	case LH5810_RESET:	return(lh5810.reset	= data);	break;
	case LH5810_U:		return(lh5810.r_u	= data);	break;
	case LH5810_L:		return(lh5810.r_l	= data);	break;
	case LH5810_G:		return(lh5810.r_g	= data);	break;
	case LH5810_MSK:	return(lh5810.r_msk = data&0x0F);	break;
	case LH5810_IF:		return(lh5810.r_if	= data);	break;
	case LH5810_DDA:	return(lh5810.r_dda = data);	break;
	case LH5810_DDB:	return(lh5810.r_ddb = data);	break;
	case LH5810_OPA:	return(lh5810.r_opa = ( (lh5810.r_opa & (~lh5810.r_dda)) | (data & (lh5810.r_dda))) );	break;
	case LH5810_OPB:	return(lh5810.r_opb = ( (lh5810.r_opb & (~lh5810.r_ddb)) | (data & (lh5810.r_ddb))) );	break;
	case LH5810_OPC:	return(lh5810.r_opc = data);	break;
	case LH5820_F:		return(lh5810.r_f	= data);	break;
	}

	return (0); 
}
 
UINT8 CLH5810::SetRegBit(LH5810_REGS reg,UINT8 bit, bool value)
{
	if (value)
	{
		switch (reg)
		{
		case LH5810_U:		return(lh5810.r_u	|= (0x01 << bit));	break;
		case LH5810_L:		return(lh5810.r_l	|= (0x01 << bit));	break;
		case LH5810_G:		return(lh5810.r_g	|= (0x01 << bit));	break;
		case LH5810_MSK:	return(lh5810.r_msk |= (0x01 << bit));	break;
		case LH5810_IF:		return(lh5810.r_if	|= (0x01 << bit));	break;
		case LH5810_DDA:	return(lh5810.r_dda |= (0x01 << bit));	break;
		case LH5810_DDB:	return(lh5810.r_ddb |= (0x01 << bit));	break;
		case LH5810_OPA:	return(lh5810.r_opa |= (0x01 << bit));	break;
		case LH5810_OPB:	return(lh5810.r_opb |= (0x01 << bit));	break;
		case LH5810_OPC:	return(lh5810.r_opc |= (0x01 << bit));	break;
		case LH5820_F:		return(lh5810.r_f	|= (0x01 << bit));	break;
		}
	}
	else
	{
		switch (reg)
		{
		case LH5810_U:		return(lh5810.r_u	&= ((0x01 << bit) ^ 0xff));	break;
		case LH5810_L:		return(lh5810.r_l	&= ((0x01 << bit) ^ 0xff));	break;
		case LH5810_G:		return(lh5810.r_g	&= ((0x01 << bit) ^ 0xff));	break;
		case LH5810_MSK:	return(lh5810.r_msk &= ((0x01 << bit) ^ 0xff));	break;
		case LH5810_IF:		return(lh5810.r_if	&= ((0x01 << bit) ^ 0xff));	break;
		case LH5810_DDA:	return(lh5810.r_dda &= ((0x01 << bit) ^ 0xff));	break;
		case LH5810_DDB:	return(lh5810.r_ddb &= ((0x01 << bit) ^ 0xff));	break;
		case LH5810_OPA:	return(lh5810.r_opa &= ((0x01 << bit) ^ 0xff));	break;
		case LH5810_OPB:	return(lh5810.r_opb &= ((0x01 << bit) ^ 0xff));	break;
		case LH5810_OPC:	return(lh5810.r_opc &= ((0x01 << bit) ^ 0xff));	break;
		case LH5820_F:		return(lh5810.r_f	&= ((0x01 << bit) ^ 0xff));	break;
		}
	}
	return (0);
}

void CLH5810::Regs_Info(UINT8 Type)
{

	switch(Type)
	{
	case 0:			// Monitor Registers Dialog
		sprintf(Regs_String,	"\r\nLH5810 :\r\nG:%.2x  MSK:%.2x  IF:%.2x\r\nDDA:%.2x  DDB:%.2x\r\nOPA:%.2x  OPB:%.2x  OPC:%.2x\r\nF:%.2x  IRQ:%c",
							lh5810.r_g,lh5810.r_msk,lh5810.r_if,lh5810.r_dda,lh5810.r_ddb,
							lh5810.r_opa,lh5810.r_opb,lh5810.r_opc,lh5810.r_f,(IRQ?'1':'0')
							);
		break;
	}

}



