
#include "i80L188EB.h"
#include "pit8253.h"
#include "pcxxxx.h"
#include "Inter.h"
#include "Log.h"

Ci80L188EB::Ci80L188EB(CPObject *parent):Ci80x86(parent)
{
    p8253 = new C8253PIT(this);

}

Ci80L188EB::~Ci80L188EB()
{
    delete p8253;
}

bool Ci80L188EB::init()
{
    Ci80x86::init();

    return true;
}

bool Ci80L188EB::exit()
{
    Ci80x86::exit();

    return true;
}

void Ci80L188EB::step()
{
    quint64 _states = pPC->pTIMER->state;
    Ci80x86::step();
    p8253->step(pPC->pTIMER->state - _states);


}


void Ci80L188EB::Reset()
{
    Ci80x86::Reset();

}


