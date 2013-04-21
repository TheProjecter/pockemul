
#include "i80L188EB.h"


Ci80L188EB::Ci80L188EB(CPObject *parent):Ci80x86(parent)
{
}

Ci80L188EB::~Ci80L188EB()
{
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
    Ci80x86::step();

    // incremennt timers

}


void Ci80L188EB::Reset()
{
    Ci80x86::Reset();

}
