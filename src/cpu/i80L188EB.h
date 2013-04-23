#ifndef I80L188EB_H
#define I80L188EB_H

#include "i80x86.h"

class C8253PIT;

class Ci80L188EB : public Ci80x86
{

public:
    Ci80L188EB(CPObject *parent);
    virtual ~Ci80L188EB();

    virtual	bool	init(void);						//initialize
    virtual	bool	exit(void);						//end
    virtual void	step(void);						//step SC61860
    virtual void	Reset(void);



    C8253PIT *p8253;

    quint16 eoi;

};




#endif // I80L188EB_H
