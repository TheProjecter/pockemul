#ifndef I8085_H
#define I8085_H

#include "cpu.h"

enum {
        I8085_PC=1, I8085_SP, I8085_AF ,I8085_BC, I8085_DE, I8085_HL,
        I8085_HALT, I8085_IM, I8085_IREQ, I8085_ISRV, I8085_VECTOR,
        I8085_TRAP_STATE, I8085_INTR_STATE,
        I8085_RST55_STATE, I8085_RST65_STATE, I8085_RST75_STATE};

#define I8085_INTR_LINE     0
#define I8085_RST55_LINE        1
#define I8085_RST65_LINE        2
#define I8085_RST75_LINE        3

#define I8085_NONE      0
#define I8085_TRAP      0x01
#define I8085_RST55     0x02
#define I8085_RST65     0x04
#define I8085_RST75     0x08
#define I8085_SID       0x10
#define I8085_INTR      0xff

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

typedef struct {
        int     cputype;        /* 0 8080, 1 8085A */
        PAIR    PC,SP,AF,BC,DE,HL,XX;
        quint8   HALT;
        quint8   IM;             /* interrupt mask */
        quint8   IREQ;           /* requested interrupts */
        quint8   ISRV;           /* serviced interrupt */
        quint32  INTR;           /* vector for INTR */
        quint32  IRQ2;           /* scheduled interrupt address */
        quint32  IRQ1;           /* executed interrupt address */
        qint8    nmi_state;
        qint8    irq_state[4];
        qint8    filler; /* align on dword boundary */
        int     (*irq_callback)(int);
        void    (*sod_callback)(int state);
}       I85regs;

typedef union {
    I85regs regs;
} I85stat;

class Ci8085 : public CCPU
{
public:
    Ci8085(CPObject *parent);
    virtual ~Ci8085();

    virtual	bool	init(void);						//initialize
    virtual	bool	exit(void);						//end
    virtual void	step(void);						//step SC61860
    virtual void	Reset(void);

    virtual	void	Load_Internal(QXmlStreamReader *);
    virtual	void	save_internal(QXmlStreamWriter *);

    virtual bool Get_Xin(){}
    virtual void Set_Xin(bool){}
    virtual bool Get_Xout(){}
    virtual void Set_Xout(bool){}
    virtual DWORD get_PC();
    virtual void Regs_Info(quint8);

    quint8 i85read8(const I85stat *, quint16, quint16);
    void i85write8(I85stat *, quint16, quint16, quint8);
    quint16 i85read16(const I85stat *, quint16, quint16);
    void i85write16(I85stat *, quint16, quint16, quint16);
    quint8 i85inp8(const I85stat *, quint16);
    quint16 i85inp16(const I85stat *, quint16);
    void i85out8(I85stat *, quint16, quint8);
    void i85out16(I85stat *, quint16, quint16);
    void i85reset(I85stat *);
    void i85trace(const I85stat *);

    int i85disasm(char *, const I85stat *, quint16, quint16);
    char *i85regs(char *, const I85stat *);

    int i85nmi(I85stat *);
    int i85int(I85stat *, int);
    int i85exec(I85stat *);

    static const int op_length[];
    static const int regrm_length[];
    static const int op_states[];
    static const int op_mem_states[];
    static const int math_states[];
    static const int math_mem_states[];
    static const int shift_states[];
    static const int shift_mem_states[];
    static const int shift1_states[];
    static const int shift1_mem_states[];
    static const int math16s_states[];
    static const int math16s_mem_states[];
    static const int grp1_8_states[];
    static const int grp1_8_mem_states[];
    static const int grp1_16_mem_states[];
    static const int grp1_16_states[];
    static const int grp2_8_mem_states[];
    static const int grp2_8_states[];
    static const int grp2_16_states[];
    static const int grp2_16_mem_states[];
    static const quint16 parity[];

    quint8 getreg8(const I85stat *i85, quint8 reg);
    void setreg8(I85stat *i85, quint8 reg, quint8 x);
    quint16 getreg16(const I85stat *i85, quint8 reg);
    void setreg16(I85stat *i85, quint8 reg, quint16 x);
    quint16 getsreg(const I85stat *i85, quint8 reg);
    void setsreg(I85stat *i85, quint8 reg, quint16 x);
    void getsegoff(const I85stat *i85, quint8 rm, quint16 *seg, quint16 *off);
    quint8 getrm8(const I85stat *i85, quint8 rm);
    void setrm8(I85stat *i85, quint8 rm, quint8 x);
    quint16 getrm16(const I85stat *i85, quint8 rm);
    void setrm16(I85stat *i85, quint8 rm, quint16 x);


    I85stat i85stat;

    UINT8 CROP();
    UINT8 ARG();
    UINT16 ARG16();
    UINT8 RM(UINT32 a);
    void WM(UINT32 a, UINT8 v);
    void illegal();
    void execute_one(int opcode);
    void Interrupt();
    int i8085_execute(int cycles);
    void init_tables();
    void i8085_reset(void *param);
    void i8085_exit();
    unsigned i8085_get_pc();
    void i8085_set_pc(unsigned val);
    unsigned i8085_get_sp();
    void i8085_set_sp(unsigned val);
    unsigned i8085_get_reg(int regnum);
    void i8085_set_reg(int regnum, unsigned val);
    void i8085_set_SID(int state);
    void i8085_set_TRAP(int state);
    void i8085_set_RST75(int state);
    void i8085_set_RST65(int state);
    void i8085_set_RST55(int state);
    void i8085_set_INTR(int state);
};


extern int i8085_ICount;

extern void i8085_set_SID(int state);
extern void i8085_set_SOD_callback(void (*callback)(int state));
extern void i8085_reset(void *param);
extern void i8085_exit(void);
extern int i8085_execute(int cycles);
extern unsigned i8085_get_context(void *dst);
extern void i8085_set_context(void *src);
extern unsigned i8085_get_pc(void);
extern void i8085_set_pc(unsigned val);
extern unsigned i8085_get_sp(void);
extern void i8085_set_sp(unsigned val);
extern unsigned i8085_get_reg(int regnum);
extern void i8085_set_reg(int regnum, unsigned val);
extern void i8085_set_nmi_line(int state);
extern void i8085_set_irq_line(int irqline, int state);
extern void i8085_set_irq_callback(int (*callback)(int irqline));
extern void i8085_state_save(void *file);
extern void i8085_state_load(void *file);
extern const char *i8085_info(void *context, int regnum);
extern unsigned i8085_dasm(char *buffer, unsigned pc);




#endif // I8085_H
