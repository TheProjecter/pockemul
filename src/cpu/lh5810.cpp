//#include <stdlib.h>
//#include <string.h>

#include "common.h"
#include "lh5810.h"



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
//	OPA=OPB=0;

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
    lh5810.r_dda=lh5810.r_ddb=0x00;

}

bool CLH5810::step()
{
	INT = false;
	// If the L register change, then TD flag of the IF register down
	if (New_L)
	{
        //AddLog(LOG_TAPE,tr("L register change -> %1X").arg(lh5810.r_l,4,16,QChar('0')));
        SetRegBit(IF,3,false);
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



