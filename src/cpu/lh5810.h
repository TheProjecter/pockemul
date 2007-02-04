
/**********************************************************/
/* lh5801 CPU emulation class                            */
/**********************************************************/

#ifndef LH5810_H
#define LH5810_H

#include "pobject.h"
#include "pd1990ac.h"

enum LH5810_REGS{
	LH5810_RESET,
	LH5810_U,
	LH5810_L,
	LH5810_G,
	LH5810_MSK,
	LH5810_IF,
	LH5810_DDA,
	LH5810_DDB,
	LH5810_OPA,
	LH5810_OPB,
	LH5810_OPC,
	LH5820_F
};

typedef struct
{
	UINT8 reset,r_g,r_u,r_l,r_msk,r_if,r_dda,r_ddb,r_opa,r_opb,r_opc,r_f;
} LH5810REG;

class CLH5810:public CPObject{
public:

	virtual bool	init(void);						//initialize
	bool	exit(void);						//end
	void	Reset(void);
	virtual bool	step(void);

	void	Load_Internal(FILE *ffile);
	void	save_internal(FILE *file);

	void	Regs_Info(UINT8);

	char*	GetClassName(){ return("CLH5810");};

	UINT8	GetReg(LH5810_REGS);
	UINT8	SetReg(LH5810_REGS,UINT8);	// return the value written
	UINT8	SetRegBit(LH5810_REGS reg,UINT8 bit, bool value);	// return the value written


	LH5810REG	lh5810;
	char	Regs_String[255];
	bool	New_L;
	bool	IRQ,INT;
	UINT8	OPA,OPB;

	CLH5810(CPObject *parent);
	~CLH5810();

private:
};



#endif

