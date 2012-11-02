#ifndef CE122_H
#define CE122_H

#include "Ce126.h"

class Cce122:public Cce126
{
public:
    Cce122();
    bool run();
    void ComputeKey();

    bool UpdateFinalImage();
    virtual void TurnON();
    virtual void TurnOFF();
};


#endif // CE122_H
