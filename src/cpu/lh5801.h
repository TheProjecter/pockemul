#ifndef LH5801_H
#define LH5801_H


#include "cpu.h"


enum {
	LH5801_T=1,
	LH5801_P,
	LH5801_S,
	LH5801_U,
	LH5801_X,
	LH5801_Y,
	LH5801_A,

	LH5801_TM,
	LH5801_IN,
	LH5801_BF,
	LH5801_PU,
	LH5801_PV,
	LH5801_DP,
	LH5801_IRQ_STATE
};

typedef union {
	struct
	{
#ifdef BIG_ENDIAN
		UINT8 h,l;
#else
		UINT8 l,h;
#endif
	} b;
	UINT16 w;
}	PAIR;

typedef struct
{
	PAIR s, p, u, x, y;
	int tm; //9 bit

	UINT8 t, a;

	bool bf, dp, pu, pv;

	UINT16 oldpc;
	
	bool irq_state;

	UINT8 IR0,IR1,IR2,HLT;

	bool idle;
} LH5801REG;



class CLH5801:public CCPU{
Q_OBJECT
public:

	bool	init(void);						//initialize
	bool	exit(void);						//end
	void	step(void);						//step SC61860
	void	Reset(void);

	void	Load_Internal(QFile *file);
	void	save_internal(QFile *file);
//	void	save_internal(void);

	DWORD	get_mem(DWORD adr,int size);		//get memory
	void	set_mem(DWORD adr,int size,DWORD data);	//set memory
	
	bool	Get_Xin(void);
	void	Set_Xin(bool);
	bool	Get_Xout(void);	
	void	Set_Xout(bool);

	DWORD	get_PC(void);					//get Program Counter
	void	Regs_Info(UINT8);

	void	TIMER_INC(void);
	bool	Is_Timer_Reached;

	void	AddState(UINT8);

	UINT8	cpu_readop(DWORD);


	LH5801REG lh5801;
	long	ticks;

    const char*	GetClassName(){ return("CLH5801");};


    CLH5801(CPObject *parent);

	void	RTN(void);
	void	Set_C(void);
	void	UnSet_C(void);
	void	LDA(UINT8);


private:

	UINT8	cpu_readmem(DWORD);
	void	cpu_writemem(DWORD, UINT8);
	void	change_pc(UINT16);

	UINT8	add_generic(int, int, int);
	UINT16	readop_word(void);
	void	ADC(UINT8);
	void	ADD_MEM(DWORD, UINT8);
	void	ADR(PAIR *);
	void	SBC(UINT8);
	void	CPA(UINT8, UINT8);
	UINT8	decimaladd_generic(int, int, int);
	void	DCA(UINT8);
	void	DCS(UINT8);
	void	AND(UINT8);
	void	AND_MEM(DWORD, UINT8);
	void	BIT(UINT8, UINT8);
	void	EOR(UINT8);
	void	ORA(UINT8);
	void	ORA_MEM(DWORD, UINT8);
	void	LDE(PAIR *);
	void	SDE(PAIR *);
	void	LIN(PAIR *);
	void	SIN(PAIR *);
	void	DEC(UINT8 *);
	void	INC(UINT8 *);
	void	POP(void);
	void	POP_WORD(PAIR *);
	void	RTI(void);
	void	PUSH(UINT8);
	void	PUSH_WORD(UINT16);
	void	JMP(DWORD);
	void	BRANCH_PLUS(int);
	void	BRANCH_MINUS(int);
	void	LOP(void);
	void	SJP(void);
	void	VECTOR(int, int);
	void	AEX(void);
	void	DRL(DWORD);
	void	DRR(DWORD);
	void	ROL(void);
	void	ROR(void);
	void	SHL(void);
	void	SHR(void);
	void	AM(int);
	void	ITA(void);
	void	instruction_fd(void);
	void	instruction(void);

	qint64	step_Previous_State;

};

#endif

