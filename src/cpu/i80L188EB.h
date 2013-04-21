#ifndef I80L188EB_H
#define I80L188EB_H

#include "i80x86.h"



class Ci80L188EB : public Ci80x86
{
    class timer {

        quint16 TCON;
        quint16 TCNT;
        quint16 TCMPA;
        quint16 TCMPB;

    };


public:
    Ci80L188EB(CPObject *parent);
    virtual ~Ci80L188EB();

    virtual	bool	init(void);						//initialize
    virtual	bool	exit(void);						//end
    virtual void	step(void);						//step SC61860
    virtual void	Reset(void);

    timer timer0;
    timer timer1;
    timer timer2;



};




#endif // I80L188EB_H
