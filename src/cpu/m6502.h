#ifndef M6502_H
#define M6502_H

#include "cpu.h"

typedef union {
#ifdef POCKEMUL_BIG_ENDIAN
    struct { quint8 h3,h2,h,l; } b;
    struct { quint16 h,l; } w;
#else
    struct { quint8 l,h,h2,h3; } b;
    struct { quint16 l,h; } w;
#endif
    quint32 d;
}	PAIR;

class Cm6502:public CCPU{
Q_OBJECT
public:
    virtual const char*	GetClassName(){ return("Cm6502");}

    virtual	bool	init(void);						//initialize
    virtual	bool	exit(void);						//end
    virtual void	step(void);						//step SC61860

    virtual	void	Load_Internal(QXmlStreamReader *){}
    virtual	void	save_internal(QXmlStreamWriter *){}

//    virtual	DWORD	get_mem(DWORD adr,int size);		//get memory
//    virtual	void	set_mem(DWORD adr,int size,DWORD data);	//set memory

    virtual	bool	Get_Xin(void) { return true;}
    virtual	void	Set_Xin(bool) {}
    virtual	bool	Get_Xout(void) { return true;}
    virtual	void	Set_Xout(bool) {}

    virtual	DWORD	get_PC(void) =0;					//get Program Counter
    virtual void	Regs_Info(UINT8) =0;


    virtual void	Reset(void);

    Cm6502(CPObject *parent);
    virtual ~Cm6502(){}

    int run(int clock);
    void write_signal(int id, quint32 data, quint32 mask);
private:
    PAIR pc, sp, zp, ea;
    quint16 prev_pc;
    quint8 a, x, y, p;
    bool pending_irq, after_cli;
    bool nmi_state, irq_state, so_state;
    int icount;
    bool busreq;

    void run_one_opecode();
    void OP(quint8 code);
    void update_irq();
    BYTE read_data8( DWORD address);
    void write_data8( DWORD address, BYTE value);

};
#if 0
/*
    Skelton for retropc emulator

    Origin : MAME
    Author : Takeda.Toshiya
    Date   : 2010.08.10-

    [ M6502 ]
*/


#define SIG_M6502_OVERFLOW	0

class Cm6502 : public CCPU
{
private:
    DEVICE *d_mem, *d_pic;

    pair pc, sp, zp, ea;
    uint16 prev_pc;
    uint8 a, x, y, p;
    bool pending_irq, after_cli;
    bool nmi_state, irq_state, so_state;
    int icount;
    bool busreq;

    void run_one_opecode();
    void OP(uint8 code);
    void update_irq();

public:
    Cm6502(VM* parent_vm, EMU* parent_emu) : CCPU(parent_vm, parent_emu) {
        busreq = false;
    }
    virtual ~Cm6502() {}

    // common functions
    void initialize();
    void reset();
    int run(int clock);
    void write_signal(int id, uint32 data, uint32 mask);
    void set_intr_line(bool line, bool pending, uint32 bit) {
        write_signal(SIG_CPU_IRQ, line ? 1 : 0, 1);
    }
    uint32 get_pc() {
        return prev_pc;
    }

    // unique function
    void set_context_mem(DEVICE* device) {
        d_mem = device;
    }
    void set_context_intr(DEVICE* device) {
        d_pic = device;
    }
};


#endif

#endif // M6502_H
